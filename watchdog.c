#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int main(int argc, char * argv[])
{
    //VARIABLES TO READ FROM pids.txt FILE
    FILE * fp1; //file pointer
    pid_t pid_motor1;
    pid_t pid_motor2;

    //VARIABLES TO READ FROM LOG.txt FILE
    FILE * fp2;
    long n_written_bytes_before=0;
    long n_written_bytes_after=0;
    time_t start_timer;

    //DEBUGGING VARIABLES
    int a;


    
    printf("watch-dog\n");
    fflush(stdout);

    sleep(1); //Wait for the motors to write their pids on the pids.txt file

    // READ FROM pids.txt THE PIDS OF THE TWO MOTORS
    fp1 = fopen("pids.txt", "r");

    fscanf(fp1, "%d\n%d\n", &pid_motor1, &pid_motor2);

    printf("the read pids are:\n%d\n%d\n", pid_motor1, pid_motor2);
    fflush(stdout);

    fclose(fp1);

    //READ THE LOG.txt FILE TO DETECT INACTION
    while(1){
        fp2 = fopen("LOG.txt", "r");

        fseek(fp2, 0L, SEEK_END); // sets the position to an offset of 0 bytes from the file end. That is, it sets the position to the end of the file. 
        n_written_bytes_before = ftell(fp2); // assigns to n_written_bytes the number of bytes from the beginning to the point of the file pointed by fp2 (here the end of the file).
        printf("(before) the written bytes on LOG.txt from the beginning to the end are: %ld\n", n_written_bytes_before);
        fflush(stdout);

        fclose(fp2);

        start_timer = time(NULL);

        while((n_written_bytes_after==n_written_bytes_before)&&(difftime(time(NULL), start_timer)<60)){
            fp2 = fopen("LOG.txt", "r");

            fseek(fp2, 0L, SEEK_END); // sets the position to an offset of 0 bytes from the file end. That is, it sets the position to the end of the file. 
            n_written_bytes_after = ftell(fp2); // assigns to n_written_bytes the number of bytes from the beginning to the point of the file pointed by fp2 (here the end of the file).

            fclose(fp2);
        }

        printf("(after) the written bytes on LOG.txt from the beginning to the end are: %ld\n", n_written_bytes_after);
        fflush(stdout);

        if(difftime(time(NULL), start_timer)>=60){
            printf("\n\nWE'RE FUCKED!\n\n");
            fflush(stdout);

            a=kill(pid_motor1, SIGUSR1);
	        if (a<0){
    	        printf("Kill error\n");
                fflush(stdout);
            }
            a=kill(pid_motor2, SIGUSR1);
	        if (a<0){
    	        printf("Kill error\n");
                fflush(stdout);
            }
        }

    }
    exit (0);
}

//the motors print on the LOG.txt when the x and z positions are reset with a certain tag (Es. MOTOR1RESET), se quando apre il file per fare l'fseek, il watchdog rileva la presenza
// di uno dei due tag, mette un booleano a true. Se entrambi i booleani sono a true, viene stoppato il check (while((n_written_bytes_after...))), fintanto che i motori non tornano 
// a spostarsi -> periodicamente quindi deve comunque essere eseguito il primo check sul file e letta l'ultima riga, per vedere se ci sono i tag di sblocco dei booleani (Es. MOTOR1START)