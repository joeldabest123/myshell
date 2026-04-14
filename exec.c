#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "structs.h"
#include "exec.h"

void run_pipeline(Pipeline* pipeline) {
    int num = pipeline->commandCount;
    int prev_fd = -1;

    for(int i=0; i<num; i++) {
        //read and write data from pipe populating pipefd
        int pipefd[2];
        if(i<num-1){
            int x = pipe(pipefd);
            if (x<0) {
                perror("pipe");
                return;
            }
        }

        pid_t pid= fork(); //duplicate process
        if(pid==0){ //if in child process...


            //input from the prev pipe:
            if(prev_fd!=-1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            //output to the next pipe:
            if(i<num-1) {
                dup2(pipefd[1],STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            //input redirection...
            if(pipeline->commands[i].inputFile){
                //new stdin file is fd of the inputFile for the command
                int fd=open(pipeline->commands[i].inputFile, O_RDONLY);
                
                //add perror if fd<0
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            //output redirection...
            if(pipeline->commands[i].inputFile){
                //new stdin file is fd of the inputFile for the command
                int fd=open(pipeline->commands[i].outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                
                //add perror if fd<0

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            execv(pipeline->commands[i].arguments[0],pipeline->commands[i].arguments);
            perror("execv"); //<-- should never execute
            exit(1);
        }

        //after pid==0 if statement, so now at parent
        if (prev_fd!=-1) { //if there's a prev_fd, close it
            close(prev_fd);
        }

        if(i<num-1){
            close(pipefd[1]);
            prev_fd=pipefd[0];
        }
    }

    for(int i=0; i<num; i++) { //wait for all children to terminate
        wait(NULL);
    }
}