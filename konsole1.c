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
    int pid;

    //PRINT THE NAME OF THE KONSOLE
    pid=atoi(argv[1]);

    printf("konsole %d\n", pid);
    fflush(stdout);

    sleep(60);

    exit (0);
}

