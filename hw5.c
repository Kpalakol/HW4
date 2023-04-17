#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

struct Job {
    pid_t pid;
    char *jname, *cmdArgs[1024], output[1024], error[1024];
    struct timeval ttime1,  ttime2;
    int status; 
};

struct Job jobs[10];
int job_count = 0,maximumLevelOfJobs = 0;

void cmdone(char *line);
void cmdtwo();
void cmdthree();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s P\n", argv[0]);
        exit(1);
    }
    maximumLevelOfJobs = atoi(argv[1]);
    char line[1024];
    while (true) {
        printf("Kavya >> ");
        fflush(stdout);
        if (fgets(line, 1024, stdin) == NULL) {
            printf("\n");
            exit(0);
        }
        if (strncmp(line, "submit ", 7) == 0) {cmdone(line + 7);
        } else if (strcmp(line, "showjobs\n") == 0) {cmdtwo();
        } else if (strcmp(line, "submithistory\n") == 0) {cmdthree();
        }
    }
}

void cmdone(char *line) {
    int n = strlen(line);
    if (line[n - 1] == '\n') {
        line[n - 1] = '\0';
    }
    char *jname = strtok(line, " ");
    jobs[job_count].jname = strdup(jname);
    jobs[job_count].cmdArgs[0] = strdup(jname);
    for (int loopCntr = 1; loopCntr < 1024; loopCntr++) {
        char *arg = strtok(NULL, " ");
        if (arg == NULL || arg[0] == '\n') {
            jobs[job_count].cmdArgs[loopCntr] = NULL;
            break;
        } else {
            jobs[job_count].cmdArgs[loopCntr] = strdup(arg);
        }
    }
    snprintf(jobs[job_count].output, 1024, "%d.out", job_count);
    snprintf(jobs[job_count].error, 1024, "%d.err", job_count);
    gettimeofday(&jobs[job_count].ttime1, NULL);
    pid_t pid = fork();
    if (pid == 0) {
        freopen(jobs[job_count].output, "w", stdout);
        freopen(jobs[job_count].error, "w", stderr);
        execvp(jname, jobs[job_count].cmdArgs);
        exit(1);
    } else if (pid > 0) {
        jobs[job_count].pid = pid;
        jobs[job_count].status = 1;
        printf("Job %d started.\n", job_count);
        job_count++;
    } else {
        printf("Error\n");
    }
}
long ttime[100];
void cmdtwo() {
    int loopCntr = 0;
    while(loopCntr<job_count){
        int status = jobs[loopCntr].status;
        if (status == 0) {
            printf("\tJobid : %d \tStatus : (waiting)\t Command: %s\n", loopCntr, jobs[loopCntr].jname);
        } else if (status == 1) {
            printf("\tJob : %d \tStatus : (running)\t Command: %s\n", loopCntr, jobs[loopCntr].jname);
        } else if (status == 2) {
            
            ttime[loopCntr] = (jobs[loopCntr].ttime2.tv_sec - 
                                        jobs[loopCntr].ttime1.tv_sec) * 1000 +
                                 (jobs[loopCntr].ttime2.tv_usec - jobs[loopCntr].ttime1.tv_usec) / 1000;
            printf("\tJob : %d \tStatus :(completed)\t Command : %s\t, %ldms\n", loopCntr, jobs[loopCntr].jname, ttime[loopCntr]);
        }
        loopCntr++;
    }
}


void cmdthree() {

    int val = job_count;
    while(val>0){
        val =val-1;
        printf("\tjobid : %d \tstatus : Completed \tCommand : %s  \ttime :%ldms \n",val, jobs[val].jname, ttime[val]);
    }
}