#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef PR_SET_CHILD_SUBREAPER
#define PR_SET_CHILD_SUBREAPER 36
#endif

int
main(int argc, char** argv)
{
    if(argc < 2) {
        fprintf(stderr, "usage: fork-wrapper command...\n");
        exit(EXIT_FAILURE);
    }

    if(prctl(PR_SET_CHILD_SUBREAPER, 1) < 0) {
        perror("could not set subreaper process attribute");
        exit(EXIT_FAILURE);
    }

    int rc = fork();

    if(rc < 0) {
        perror("could not fork");
        exit(EXIT_FAILURE);
    }

    if(rc == 0) {
        execvp(argv[1], argv + 1);

        perror("could not exec");
        exit(EXIT_FAILURE);
    }

    int last_exit_status = 0;

    while(1) {
        int stat;
        int pid = wait(&stat);

        if(pid < 0) {
            if(errno == EINTR) {
                continue;
            }

            if(errno == ECHILD) {
                // there are no more children, exit with the exit value of the
                // last child to terminate
                exit(last_exit_status);
            }

            // something went terribly wrong...
            perror("could not wait");
            exit(EXIT_FAILURE);
        }

        last_exit_status = WEXITSTATUS(stat);
    }
}
