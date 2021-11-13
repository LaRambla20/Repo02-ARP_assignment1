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
    // char format_str_in[100]="%d,%d,%d,%d";
    // int pid1;
    // int pid2;
    // int pid3;
    // int pid4;

    // int a;
    
    printf("watch-dog\n");
    fflush(stdout);

    // sscanf(argv[1], format_str_in, &pid1, &pid2, &pid3, &pid4);
    // printf("the read pids are:\n%d\n%d\n%d\n%d\n", pid1, pid2, pid3, pid4);
    // fflush(stdout);

    sleep(10);

    // a=kill(pid1, SIGUSR1);
	// if (a<0){
    // 	printf("Kill error\n");
    //     fflush(stdout);
    // }

    sleep(50);

    exit (0);
}