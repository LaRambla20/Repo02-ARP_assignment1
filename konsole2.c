#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int main(int argc, char * argv[])
{
    int num;

    //VARIABLES TO READ FROM FILE
    FILE * fp; //file pointer
    pid_t pid_motor1;
    pid_t pid_motor2;

    //DEBUGGING VARIABLES
    int a;
    
    


    //PRINT THE NAME OF THE KONSOLE
    num=atoi(argv[1]);

    printf("konsole %d\n", num);
    fflush(stdout);




    sleep(5);




    // READ FROM pids.txt THE PIDS OF THE TWO MOTORS
    fp = fopen("pids.txt", "r");

    fscanf(fp, "%d\n%d\n", &pid_motor1, &pid_motor2);

    printf("the read pids are:\n%d\n%d\n", pid_motor1, pid_motor2);
    fflush(stdout);

    fclose(fp);




    sleep(10);



    

    a=kill(pid_motor1, SIGUSR1);
	if (a<0){
    	printf("Kill error\n");
        fflush(stdout);
    }

    sleep(45);

    exit (0);
}

