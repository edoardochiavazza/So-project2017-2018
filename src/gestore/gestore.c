#include <stdlib.h>
#include "../lib/config.h"
#include "../lib/sem_utils.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include "../lib/gestore_utils.h"
#include "../lib/student_utils.h"


conf *config;
student* showcase = NULL;
int shm_id_config, sem_id, shm_id_ipcs, id_code_answer, id_code_invite, showcase_id_config, config_sem_id, critic_sem_id, pop_size, id_code_student_gestore;
list* group_list;

//manage the handler with SIGALRM and SIGINT.
void alarmHandler(int sig) {
    signal(sig,SIG_IGN);
    if(sig == SIGALRM){

        for (int i = 0; i < config -> pop_size; i++) {
               kill((showcase + i) -> matr - MATR, SIGKILL);
        }
        calc_average(group_list);
        print_groups(group_list);
        print_result(group_list);
    }

    if(semctl(sem_id, 0, IPC_RMID) == -1) throw_error(errno);
    if(semctl(config_sem_id, 0, IPC_RMID) == -1) throw_error(errno);
    if(semctl(critic_sem_id, 0, IPC_RMID) == -1) throw_error(errno);
    if(msgctl(id_code_answer, IPC_RMID, NULL) == -1) throw_error(errno);
    if(msgctl(id_code_invite, IPC_RMID, NULL) == -1) throw_error(errno);
    if(msgctl(id_code_student_gestore, IPC_RMID, NULL) == -1) throw_error(errno);
    if(shmctl(shm_id_ipcs, 0, IPC_RMID) == -1) throw_error(errno);
    if(shmctl(showcase_id_config, 0, IPC_RMID) == -1) throw_error(errno);
    if(shmctl(shm_id_config, 0, IPC_RMID) == -1) throw_error(errno);

    list_free(group_list);
    exit(EXIT_SUCCESS);

}


int main(){

        ipcs *memory = NULL;
        int num_alive_student, matr_leader, matr_to_add, close_all;
        pid_t child_pid;
        message message;
        student* to_add;
        student* leader;

        //clear the screen
        system("clear");
        //disable output buffering
	    setbuf(stdout, NULL);

        if(signal(SIGALRM, alarmHandler)== SIG_ERR) {
          printf("Errore della disposizione dell'handler\n");
          exit(EXIT_SUCCESS);
        }

        if(signal(SIGINT, alarmHandler)== SIG_ERR) {
          printf("Errore della disposizione dell'handler\n");
          exit(EXIT_SUCCESS);
        }


        //shared memory for config
        if((shm_id_config = shmget(IPC_PRIVATE, sizeof(conf) , IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        config = fill_config_struct(shm_id_config);
        //shared memory for the group
        if((showcase_id_config = shmget(IPC_PRIVATE, sizeof(student) * config -> pop_size, IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        //list for student
        group_list = (list*)malloc(sizeof(list));
        init_list(group_list);
        pop_size = config -> pop_size;

        if((sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666 )) == -1)
                throw_error(errno);
        if((config_sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666 )) == -1)
                throw_error(errno);
        if((critic_sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666 )) == -1)
                throw_error(errno);
        //init all sem
        if(init_sem(sem_id, config -> pop_size + 1) == -1) {
                throw_error(errno);
        }
        if(init_sem(config_sem_id, config -> pop_size + 1) == -1) {
                throw_error(errno);
        }
        if(init_sem(critic_sem_id, 1) == -1) {
                throw_error(errno);
        }
        //queue for student to comunicate with gestore
        if((id_code_student_gestore = msgget(IPC_PRIVATE, IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        //queue for student
        if((id_code_invite = msgget(IPC_PRIVATE,IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        if((id_code_answer = msgget(IPC_PRIVATE,IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        //fill struct for student
        if((shm_id_ipcs = shmget(IPC_PRIVATE, sizeof(ipcs) , IPC_CREAT | 0666)) == -1)
                throw_error(errno);
        memory = fill_ipcs_struct(shm_id_config, sem_id,shm_id_ipcs,id_code_answer,id_code_invite, showcase_id_config, config_sem_id, critic_sem_id, id_code_student_gestore);
        //create student_argument pass to
        if((showcase = shmat(showcase_id_config, NULL, 0)) == (void *) -1) // collego gli studenti alla zona di memoria dove ci sono le chiavi.
                throw_error(errno);

        //make sons
        for (num_alive_student = 0; num_alive_student < pop_size; num_alive_student++) {
    		switch (child_pid = fork()) {
        		case -1:
                                /* Handler error */
        			exit(EXIT_FAILURE);
                    break;
        		case 0:
                    execl(PROGRAM, integer_to_string(shm_id_ipcs), 0);
        			/* CHILD CODE */
        			break;
        		default:
                    fill_student_struct(config, (showcase + num_alive_student), child_pid);
        			/* PARENT CODE */
        			break;
    		}
        }
        barrier_sem(config_sem_id);
        barrier_sem(sem_id);
        alarm(config -> sim_time);

        while(1) {
            while(msgrcv(id_code_student_gestore, &message, (sizeof(message) - sizeof(long)), 0, 0) !=  (ssize_t) -1){
                matr_leader = message.matr_leader;
                matr_to_add = message.matr_to_add;
                to_add = search_student_add(matr_to_add, showcase, pop_size);
                leader = search_student_add(matr_leader, showcase, pop_size);
                close_all = message.close_all;
                if(close_all == 0){
                    if(matr_to_add == 1){
                        node* prt = group_list -> head;
                        assert(prt != NULL);
                        while(matr_leader != prt -> team[0] -> matr){
                            prt = (node*)prt -> next;
                        }
                        prt -> group_closed = 1;
                    }else if(control_existing_group(matr_leader, group_list)){
                        add_student_to_group(matr_leader, to_add, group_list);
                    }else{
                        create_new_group(leader, group_list);
                        add_student_to_group(matr_leader, to_add, group_list);
                    }
                }else {
                    close_all_groups(leader, group_list);

                }
            }
        }
}
