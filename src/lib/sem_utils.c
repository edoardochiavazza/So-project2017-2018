#include "sem_utils.h"
#include <sys/sem.h>
#include <stdio.h>

int barrier_sem(int);

int init_sem(int sem_id, int initval) {
        union semun arg;
        arg.val = initval;
        return semctl(sem_id, 0, SETVAL, arg);
}

int barrier_sem(int sem_id) {
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        semop(sem_id, &sops, 1);
        sops.sem_flg = 0;
        sops.sem_op = 0;
        return semop(sem_id, &sops, 1);

}

int reserve_sem(int sem_id){
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_flg = 0;
        sops.sem_op = -1;
        return semop(sem_id, &sops, 1);
}

int release_sem(int sem_id) {
   struct sembuf sops;
   sops.sem_num = 0;
   sops.sem_op = 1;
   sops.sem_flg = 0;
   return semop(sem_id, &sops, 1);
}
