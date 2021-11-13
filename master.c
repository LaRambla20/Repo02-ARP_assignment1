#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

//FUNCTION THAT IS CALLED TO FORK AND EXEC A PROCESS 
int spawn(const char * program, char ** arg_list) {
  pid_t child_pid = fork();
  if (child_pid != 0) // father is acting
    return child_pid;
  else { // son is acting
    execvp (program, arg_list);
    perror("exec failed");
    return 1;
  }
}

int main() {

  //CLEAR THE pids.txt FILE
  fclose(fopen("pids.txt", "w"));
  //CLEAR THE LOG.txt FILE
  fclose(fopen("LOG.txt", "w"));

  char * arg_list_1[] = { "/usr/bin/konsole",  "-e", "./konsole1", "1", (char*)NULL }; //argv[] argument that need to be passed to the function "execvp" in order to execute the executable
  //"tobexecuted" in a new konsole with argument 10
  char * arg_list_2[] = { "/usr/bin/konsole",  "-e", "./konsole2", "2", (char*)NULL };
  char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./motor1", "1", (char*)NULL };
  char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./motor2", "2", (char*)NULL };
  int pid1;
  int pid2;
  int pid3;
  int pid4;
  int pid_wd;

  //RUN THE FOUR PROCESSES (WATCHDOG NOT INCLUDED) IN FOUR DIFFERENT KONSOLES AND STORE THEIR PIDS IN FOUR DIFFERENT VARIABLES
  pid1 = spawn("/usr/bin/konsole", arg_list_1);
  printf("1st konsole pid = %d\n", pid1);
  fflush(stdout);
  pid2 = spawn("/usr/bin/konsole", arg_list_2);
  printf("2nd konsole pid = %d\n", pid2);
  fflush(stdout);
  pid3 = spawn("/usr/bin/konsole", arg_list_3);
  printf("3rd konsole pid = %d\n", pid3);
  fflush(stdout);
  pid4 = spawn("/usr/bin/konsole", arg_list_4);
  printf("4th konsole pid = %d\n", pid4);
  fflush(stdout);

  char * arg_list_5[] = { "/usr/bin/konsole",  "-e", "./watchdog", (char*)NULL }; //add "myfifo to pass the name of the pipe opened"
  pid_wd = spawn("/usr/bin/konsole", arg_list_5);
  printf("watch-dog konsole pid = %d\n", pid_wd);
  fflush(stdout);

  //EXIT
  printf ("Main program exiting...\n");
  return 0;
}

