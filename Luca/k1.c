#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define STOP_CMD 'q'
#define RIGHT_CMD 'd'
#define LEFT_CMD 'a'
#define UP_CMD 'w'
#define DOWN_CMD 's'


int main(int argc, char * argv[]){
  // Declare two FIFOs for sending commands to the motors
  char *fifo_x= "/tmp/cmd_fifo_x";           // comm. channel with motor_x
  char *fifo_z= "/tmp/cmd_fifo_z";           // comm. channel with motor_z
  mkfifo(fifo_x, 0666);
  mkfifo(fifo_z, 0666);

  int fd_x, fd_z;
  char cmd;
  char prev_cmd= STOP_CMD;          // initialize the prev_cmd as a stop one

  // Infinite loop
  while(1){
    // Open the FIFO to communicate with motor_x
    fd_x= open(fifo_x, O_WRONLY);
    if(fd_x == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }
    // Open the FIFO to communicate with motor_z
    fd_z= open(fifo_z, O_WRONLY);
    if(fd_z == -1){
      perror("open() error");
      exit(EXIT_FAILURE);
    }

    printf("Insert a command: ");
    fflush(stdout);
    scanf(" %c", &cmd);

    // Variable to keep track if an invalid command has been entered and not memorize it
    int invalid_cmd= 0;

    if(cmd == RIGHT_CMD || cmd == LEFT_CMD){      // x-axis commands
      // Send commands to motor_x
      if(prev_cmd != STOP_CMD){
        printf("First stop the motion along the x-axis.\n");
        fflush(stdout);
        invalid_cmd= 1;
      }
      else{
        // Send RIGHT_CMD or LEFT_CMD to motor_x
        write(fd_x, &cmd, sizeof(cmd));
        fsync(fd_x);
      }
    }
    else if(cmd == UP_CMD || cmd == DOWN_CMD){    // z-axis commands
      // Send commands to motor_z
      if(prev_cmd != STOP_CMD){
        printf("First stop the motion along the z-axis.\n");
        fflush(stdout);
        invalid_cmd= 1;
      }
      else{
        // Send UP_CMD or DOWN_CMD to motor_z
        write(fd_z, &cmd, sizeof(cmd));
        fsync(fd_z);
      }
    }
    else if(cmd == STOP_CMD){                     // stop command for both axis
      if(prev_cmd == RIGHT_CMD || prev_cmd == LEFT_CMD){
        printf("Stopping motor_x.\n");
        fflush(stdout);
        // Send STOP_CMD to motor_x
        write(fd_x, &cmd, sizeof(cmd));
        fsync(fd_x);
      }
      else if(prev_cmd == UP_CMD || prev_cmd == DOWN_CMD){
        printf("Stopping motor_z.\n");
        fflush(stdout);
        // Send STOP_CMD to motor_z
        write(fd_z, &cmd, sizeof(cmd));
        fsync(fd_z);
      }
    }
    else{                                         // invalid commands
      invalid_cmd= 1;
    }

    // If the command that was entered was valid, save it in prev_cmd
    if(invalid_cmd == 0){
      prev_cmd= cmd;
    }
    invalid_cmd= 0;

    close(fd_x);
    close(fd_z);
  }
}
