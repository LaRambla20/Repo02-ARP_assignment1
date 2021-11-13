#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

// Define some macros to describe the hoist's x axis geometry
#define MAX_ABS_DISPLACEMENT 2000  // mm, so -MAX_ABS_DISPLACEMENT < x_pos < MAX_ABS_DISPLACEMENT
#define INCREMENT_STEP 1           // mm
#define INIT_POS_Z 0               // mm
#define WAIT_TIME 100000           // us, which are 0.01s

#define STOP_CMD 'q'               // comand to stop the motion along the z-axis (used as default comand)
#define UP_CMD 'w'
#define DOWN_CMD 's'

char command= STOP_CMD;
float pos_z= INIT_POS_Z;

// Reset procedure
void reset_procedure(){
  pos_z= (int) pos_z;   // removing the random error
  while(pos_z != INIT_POS_Z){
    if(pos_z > INIT_POS_Z){
      pos_z -= INCREMENT_STEP;
    }
    else{
      pos_z += INCREMENT_STEP;
    }
    printf("Current position: %f\n", pos_z);
    usleep(50000);
  }
}

// Handler for emergency stop signal (SIGUSR1)
static void stop_handler(int sig_num){
  if(sig_num == SIGUSR1){
    printf("Caught the emergency stop signal!\n");
    // Code to handle the signal
    command= STOP_CMD;
  }
}

// Handler for reset signal (SIGUSR2)
static void reset_handler(int sig_num){
  if(sig_num == SIGUSR2){
    printf("Caught the reset signal!\n");
    // Code to handle the signal
    reset_procedure();
    command= STOP_CMD;
  }
}

// Function to generate a float random number in a simmetric interva (-max, max)
float error(float max){
  // firstly generate randomly {0, 1} to decide the sign of the error
  int sign= rand() % (1 + 1 - 0) + 0;
  if(sign == 0){
    sign= -1;
  }

  float err= sign * (float)rand()/((float)RAND_MAX/max);
  return err;
}


int main(int argc, char * argv[]){
  // Print the motor_z pid on a file
  pid_t pid_z;
  FILE *fp;

  pid_z= getpid();
  fp= fopen("pids.txt", "a");
  fprintf(fp, "%d\n", pid_z);
  fclose(fp);

  // Define a Fifo for reading input commands from K1 (command console)
  //mkfifo(argv[1], 0666);
  mkfifo("/tmp/cmd_fifo_z", 0666);
  int fd_r;

  // Define a Fifo for sending the current position along x direction to K2 (monitoring
  // console)
  //mkfifo(argv[2], 0666);
  mkfifo("/tmp/pos_fifo_z", 0666);
  int fd_w;

  // Set timeval structure to use with the select() syscall. In particular the
  // timeout is set to 0, which means that the select() won't block the process for
  // any amount of time if the pipe is not ready for read
  struct timespec tv;
  tv.tv_sec= 0;
  tv.tv_nsec= 50000000;

  // Variable to decide whether adding a random error to pos_z or not
  int add_err= 1;

  // Register a custom handler for the emrgency stop signal
  if(signal(SIGUSR1, stop_handler) == SIG_ERR){
    perror("Emergency stop signal error");
    exit(EXIT_FAILURE);
  }

  // Register a custom handler for the reset signal
  if(signal(SIGUSR2, reset_handler) == SIG_ERR){
    perror("Reset signal error");
    exit(EXIT_FAILURE);
  }

  // Entering the infinite loop
  while(1){
    // Open the FIFOs
    // fd_r= open(argv[1], O_RDONLY);
    fd_r= open("/tmp/cmd_fifo_z", O_RDONLY);
    if(fd_r == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }

    // fd_w= open(argv[2], O_WRONLY);
    fd_w= open("/tmp/pos_fifo_z", O_WRONLY);
    if(fd_w == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }

    // Add fd_r to the set of the file descriptors to monitor for reading
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd_r, &read_fds);

    // Define the set of signals to mask during the pselect()
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);
    sigaddset(&sigmask, SIGUSR2);

    //tv.tv_sec= 0;
    //tv.tv_usec= WAIT_TIME;

    int retval= pselect(fd_r+1, &read_fds, NULL, NULL, &tv, &sigmask);

    switch(retval){
      // if an error has occured in the select
      case -1:
        perror("select() error");
        exit(EXIT_FAILURE);
        break;

      // if there isn't any new command (named pipe not ready for reading)
      case 0:
        switch(command){
          // move up
          case UP_CMD:
            if(pos_z < MAX_ABS_DISPLACEMENT){
              // printf("Moving up.\n");
              pos_z= pos_z + INCREMENT_STEP;
            }
            else{
              printf("Cannot move further in this direction.\n");
              fflush(stdout);
            }
            break;

          // move down
          case DOWN_CMD:
            if(pos_z > -MAX_ABS_DISPLACEMENT){
              // printf("Moving down.\n");
              pos_z= pos_z - INCREMENT_STEP;
            }
            else{
              printf("Cannot move further in this direction.\n");
              fflush(stdout);
            }
            break;

          // by default every other command (even invalid ones) are treated as a STOP_CMD
          default:
            add_err= 0;
            break;
        }

        // Model error on the estimated position before sending it to K2
        if(add_err == 1){
          pos_z= pos_z + error(1.0);
        }

        // Verify if the position exceed the displacement bounds after adding the
        // error. If so, pos_x is set to the maximum possible displacement.
        if(pos_z > MAX_ABS_DISPLACEMENT){
          pos_z= MAX_ABS_DISPLACEMENT;
          command= STOP_CMD;
        }
        if(pos_z < -MAX_ABS_DISPLACEMENT){
          pos_z= -MAX_ABS_DISPLACEMENT;
          command= STOP_CMD;
        }

        // Send pos_z to K2
        write(fd_w, &pos_z, sizeof(pos_z));
        fsync(fd_w);

        printf("Current position: %f\n", pos_z);
        add_err= 1;
        break;

      // If there is a new command available (named pipe ready for reading)
      default:
        // Updating the command
        read(fd_r, &command, sizeof(command));
        //sscanf(in_buff, "%c", &command);
        printf("Received command: \'%c\'\n", command);
        break;
    }

    close(fd_w);
    close(fd_r);
  }
}
