#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define EMERGENCY_STOP_CMD 'e'
#define RESET_CMD 'r'


int main(int argc, char * argv[]){
  // Reads the pids of motor_x and motor_z from the file pids.txt
  pid_t pid_motor_x;
  pid_t pid_motor_z;
  sleep(1);
  FILE * fp;      // initialize the file pointer
  fp = fopen("pids.txt", "r");
  fscanf(fp, "%d\n%d\n", &pid_motor_x, &pid_motor_z);
  fclose(fp);

  // Declare two FIFOs for sending commands to the motors
  char *fifo_x= "/tmp/pos_fifo_x";           // comm. channel with motor_x
  char *fifo_z= "/tmp/pos_fifo_z";           // comm. channel with motor_z
  mkfifo(fifo_x, 0666);
  mkfifo(fifo_z, 0666);

  int fd_x, fd_z, fd_stdin;
  char cmd;
  float pos_x, pos_z;

  // Set timeval structure to use with the select() syscall
  struct timeval tv;

  // Infinite loop
  while(1){
    // Open the FIFO to communicate with motor_x
    fd_x= open(fifo_x, O_RDONLY);
    if(fd_x == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }
    // Open the FIFO to communicate with motor_z
    fd_z= open(fifo_z, O_RDONLY);
    if(fd_z == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }

    // The timeout to be used with the select() is set to 0
    tv.tv_sec= 0;
    tv.tv_usec= 50000;

    // Add stdin to the set of the file descriptors to monitor for reading
    fd_set read_fds;
    fd_stdin= fileno(stdin);
    FD_ZERO(&read_fds);
    FD_SET(fd_stdin, &read_fds);

    int retval= select(fd_stdin+1, &read_fds, NULL, NULL, &tv);

    switch(retval){
      case -1:
        perror("select() error");
        exit(EXIT_FAILURE);
        break;

      case 0:
        // Read the position of motor_x and motor_z, thne print it
        read(fd_x, &pos_x, sizeof(pos_x));
        read(fd_z, &pos_z, sizeof(pos_z));

        printf("Hoist position: (%f, %f)\n", pos_x, pos_z);
        fflush(stdout);
        break;

      default:
        scanf(" %c", &cmd);

        if(cmd == EMERGENCY_STOP_CMD){
          // Send a stop signal to both motor_x and motor_y
          printf("Stopping motion on both axes.\n");
          fflush(stdout);
          int ret1= kill(pid_motor_x, SIGUSR1);
          int ret2= kill(pid_motor_z, SIGUSR1);
        }
        else if(cmd == RESET_CMD){
          // Send a reset signal to both motor_x and motor_y
          printf("Returning to the inital position.\n");
          fflush(stdout);
          kill(pid_motor_x, SIGUSR2);
          kill(pid_motor_z, SIGUSR2);
        }
        else{
          printf("Invalid command.\n");
          fflush(stdout);
        }
        break;
    }

    close(fd_x);
    close(fd_z);
  }
}
