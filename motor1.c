#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h>

char* format_time(){
    time_t rawtime; //Unix and POSIX-compliant systems implement the time_t type as a signed integer (typically 32 or 64 bits wide) which represents the number of seconds since the start 
    // of the Unix epoch
    struct tm * timeinfo; // Structure containing a calendar date and time broken down into its components: tm_sec, tm_min, tm_hour, tm_mday (day pf the month), tm_month, tm_year
    char* timedate;

    time ( &rawtime ); //time_t time( time_t *second ) This function returns the time since 00:00:00 UTC, January 1, 1970 (Unix timestamp) in seconds. 
    // If second is not a null pointer, the returned value is also stored in the object pointed to by second.
    timeinfo = localtime ( &rawtime ); //struct tm *localtime(const time_t *timer) function that uses the time pointed by timer to fill a tm structure with the values that represent the corresponding local time. 
    // The value of timer is broken up into the structure tm and expressed in the local time zone.
    // AKA Convert time_t to tm as local time (function ) -> the inverse function is mktime

    timedate = asctime(timeinfo); //char *asctime(const struct tm *timeptr); The asctime function returns a pointer to a null-terminated string that is of the form:
    // Fri Feb 15 14:45:01 2013\n
    return timedate;
}


void sig_handler(int sig_number)
{
    if (sig_number == SIGUSR1){
        printf("\nMOTOR1: received SIGUSR1 from the watchdog -> RESET\n");
    }
}


int main(int argc, char * argv[])
{
    int num;

    //REGISTER THE SIGUSR1 SIGNAL
    if (signal(SIGUSR1, sig_handler)== SIG_ERR){
        printf("\nError in catching the signal\n");
    }

    //VARIABLES TO PRINT THE PID ON THE pids.txt FILE
    pid_t pid;
    FILE * fp1; //file pointer 1


    //LOOP VARIABLES
    int n_iter1=0;
    int n_iter2=0;
    //VARIABLES TO PRINT THE ACTION ON THE LOG.txt FILE
    char *action_time;
    FILE * fp2;



    
    //PRINT THE NAME OF THE KONSOLE
    num=atoi(argv[1]);

    printf("motor %d\n", num);
    fflush(stdout);

    //RETRIEVE THE PID OF THE PROCESS
    pid=getpid();
    printf("my pid is:%d\n\n", pid);
    fflush(stdout);


    //PRINT ON pids.txt THE PID OF THIS PROCESS
    fp1 = fopen("pids.txt", "a");

    fprintf(fp1, "%d\n", pid);

    fclose(fp1);

    //ACTION OF THE MOTOR AND PRINT OF THE LOG
    while(1){
        n_iter1 = n_iter1+1;

        printf("MOTOR1: %d-th trip\n", n_iter1);
        fflush(stdout);

        if(n_iter1<=10){
            printf("sorr't\n");
            fflush(stdout);

            action_time = format_time();
            printf("MOTOR1: Action current local time and date: %s\n", action_time);
            fflush(stdout);

            n_iter2 = n_iter2+1;

            if(n_iter2==5){
                fp2 = fopen("LOG.txt", "a");
                fprintf(fp2, "MOTOR1: doing nothing as always at %s",action_time);
                fclose(fp2);
                n_iter2=0;
            }
            sleep(3);
        }
        else{
            printf("MOTOR1: goin' to sleep\n\n");
            fflush(stdout);
            sleep(60);
            n_iter1=0;
        }
    }

    exit (0);
}

