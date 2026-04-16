#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "structs.h"
#include "exec.h"

//Used to search directories in order
char* find_exec(char *command) {

    if (!command) {
        return NULL;
    }

    if (strchr(command, '/')) {
        return strdup(command);
    }

    //used for searching the 3 required directories
    const char *dirs[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    char path[1024];

    //runs through em
    for (int i = 0; i < 3; i++) {

        snprintf(path, sizeof(path), "%s/%s", dirs[i], command);

        if (access(path, X_OK) == 0) {
            return strdup(path);
        }
    }

    return NULL;
}


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
                
                if(fd<0) {
                    perror("input");
                    exit(1);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            //output redirection...
            if(pipeline->commands[i].outputFile){
                //new stdin file is fd of the inputFile for the command
                int fd = open(pipeline->commands[i].outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                
                if(fd < 0) {
                    perror("output");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (i == 0 && !isatty(STDIN_FILENO) && pipeline->commands[i].inputFile == NULL) {
                int fd = open("/dev/null", O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }


            char *cmd= pipeline->commands[i].arguments[0];

            if(!cmd) {
                fprintf(stderr, "mysh: empty command\n");
                exit(1);
            }

            char * path = find_exec(cmd);
            if (!path) {
                fprintf(stderr, "mysh: command not found: %s\n", cmd);
                exit(1);
            }

            execv(path, pipeline->commands[i].arguments);

            char * sh_args[] = {"sh",path,NULL};
            execv("/bin/sh",sh_args);


            perror("execv"); //<-- should never execute
            free(path);
            _exit(1); //avoids flushing stdio buffers twice
        }

        //after pid==0 if statement: so now at parent
        if (prev_fd!=-1) { //if there's a prev_fd, close it
            close(prev_fd);
        }

        if(i<num-1){
            close(pipefd[1]);
            prev_fd=pipefd[0];
        }
    }

    for(int i=0; i<num; i++) { //wait for all children to terminate
        int status;
        wait(&status);

        if(isatty(STDIN_FILENO)) {
            if(WIFEXITED(status)) {
                int code = WEXITSTATUS(status);
                if (code != 0) {
                    printf("Exited with status %d\n", code);
                }
            } else if (WIFSIGNALED(status)) {
                printf("Terminated by signal %s\n",
                    strsignal(WTERMSIG(status)));
            }
        }
    }
}