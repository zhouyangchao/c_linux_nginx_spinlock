#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "spinlock.h"

int * shared;
ngx_atomic_t * shareLock;



void workerEvent(pid_t pid)
{
    //printf("workerEvent ： %d\n", pid);
    //sleep(1);
    while(*shared < 199)
    {
        //正常乱序
        //*shared = *shared + 1;
        //printf("workerEvent %d： %d\n", pid, *shared);


//
//        if(ngx_spinlock_trylock(shareLock, pid))
//        {
//            *shared = *shared + 1;
//            printf("workerEvent %d： %d\n", pid, *shared);
//            ngx_spinlock_unlock(shareLock, pid);
//        }


        ngx_spinlock_lock(shareLock, pid, 1024);
        *shared = *shared + 1;
        printf("workerEvent %d： %d\n", pid, *shared);
        ngx_spinlock_unlock(shareLock, pid);
    }
    printf("while end===================\n");
}



int fork_test ()
{
    int i;
    pid_t p;
    for(i = 0; i <= 2; i++)
    {
        if((p=fork())==0)
        {
            workerEvent(getpid());
            getchar();
            return 0;
        }

        //wait(p, NULL, 0);
    }
    sleep(3);
    printf("main end===================\n");


    getchar();
    return 0;
}


int main ()
{
    shareLock = ngx_spinlock_create();
    shared = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    fork_test();
    return 0;
}


