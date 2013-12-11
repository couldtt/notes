/*************************************************************************
	> File Name: thrdcreat.c
	> Author: couldtt(fyby)
	> Mail: xywrbs@163.com 
	> Created Time: 2013年12月11日 星期三 22时16分27秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

void task1(int *counter);
void task2(int *counter);
void cleanup(int counter1, int counter2);

int g1 = 0;
int g2 = 0;

int main(int argc, char *argv[])
{
    pthread_t thrd1, thrd2;
    int ret;
    void *retval;

    /*Create the first thread*/
    ret = pthread_create(&thrd1,NULL, (void *)task1, (void *)&g1);
    printf("I'm in main function\n");

    /*Create the second thread*/
    ret = pthread_create(&thrd2, NULL, (void*)task2, (void *)&g2);
    //pthread_join(thrd2, NULL);
    //pthread_join(thrd1, NULL)
    cleanup(g1, g2);
    getchar();
    int tmp;

    if ((tmp = pthread_join(thrd1, &retval)) != 0){
        switch(tmp) {
            case EINVAL:
                printf("pthread_join return value is EINVAL: %d\n", tmp);
                break;
            case ESRCH:
                printf("pthread_join return value is ESRCH: %d\n", tmp);
                break;
            default:
                printf("pthread_join return value is others: %d\n", tmp);
                break;
        }
    }
    printf("return val of task1 is %d\n", (int)retval);
    cleanup(g1,g2);
    // sleep(2);
    exit(EXIT_SUCCESS);
}

void task1(int *counter)
{
    while (* counter < 5) {
        printf("task1 count: %d\n", *counter);
        (*counter)++;
        sleep(1);
    }
    pthread_exit((void *)100);
}

void task2(int *counter)
{
    while (* counter < 5) {
        printf("task2 count: %d\n", *counter);
        (*counter)++;
    }
}

void cleanup(int counter1, int counter2)
{
    printf("total iterations: %d\n", counter1 + counter2);
}

