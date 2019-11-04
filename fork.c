#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void doSomeWork(char* name){

    for(int i=0;i<2;i++){
        sleep(rand() % 4);
        printf("Done pass %d for %s\n", i, name);
    }
}

int main(int argc, char *argv[]){

    printf("I am: %d\n", (int) getpid());

    pid_t pid = fork();
    srand((int) pid);
    printf("Fork returned: %d\n", (int) pid);

    if(pid == 0){
        printf("I am the child with pid %d\n", (int) getpid());
        doSomeWork("Child");
        printf("Child exiting\n");
        exit(42);
    }
    if(pid == 0){
        perror("Fork failed\n");
    }

    //we must be the parent
    printf("I am the parent, waiting for the child to end\n");
    sleep(30);
    doSomeWork("Parent");
    int status = 0;
    pid_t childpid = wait(&status);
    printf("Parent know child %d finished with status %d\n", (int) childpid, status);
    int childReturnValue = WEXITSTATUS(status);
    printf("     Return value: %d\n", childReturnValue);

    return 0;
}
