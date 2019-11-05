#ifndef SEM_UTILS_H
#define SEM_UTILS_H
#include <sys/types.h>          /* for portability */
#include <sys/sem.h>

#if ! defined(__FreeBSD__) && ! defined(__OpenBSD__) && ! defined(__sgi) && ! defined(__APPLE__)
        union semun {                   /* used in calls to semctl() */
                int val;
                struct semid_ds * buf;
                unsigned short * array;
#if defined(__linux__)
        struct seminfo * __buf;
#endif
};
#endif

int barrier_sem(int);           // reserve semaphore - decrement it by 1
int init_sem(int, int);         // create a semaphore
int release_sem(int);           // release semaphore - increment it by 1
int reserve_sem(int);           // reserve the semaphore


#endif
