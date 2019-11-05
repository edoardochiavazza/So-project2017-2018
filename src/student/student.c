#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../lib/config.h"
#include "../lib/gestore_utils.h"
#include "../lib/student_utils.h"
#include "../lib/sem_utils.h"
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#define ACCEPT 1
#define REFUSE 0

int main(int argc, char *argv[]){
        int shm_student_id = atoi(argv[0]);
        conf *config;
        ipcs *memory;
        student *showcase, *student;
        int simulation_time;
        int invitati = 0;
        int matr_found;
        int fase;
        answer answer1;
        invite invite;
        int num_member = 1;
        int close_group = 0;

        //attach to all shared memory
        if((memory = shmat(shm_student_id, NULL, 0)) == (void *) -1) // collego gli studenti alla zona di memoria dove ci sono le chiavi.
                throw_error(errno);
        if((config = shmat(memory -> shm_id_config, NULL, 0)) == (void *) -1) // collego gli studenti alla zona di memoria dove ci sono le chiavi.
                throw_error(errno);
        if((showcase = shmat(memory -> showcase_id_config, NULL, 0)) == (void *) -1) // collego gli studenti alla zona di memoria dove ci sono le chiavi.
                throw_error(errno);

        barrier_sem(memory -> config_sem_id);

        //link the student pointer to the corrisponding showcase.
        for(int i = 0; i < config -> pop_size; i++)
                if((getpid() + MATR) == (showcase + i) -> matr)
                        student = showcase + i;

        barrier_sem(memory -> sem_id);
        simulation_time = time(NULL);

        while(1){
                reserve_sem(memory -> critic_sem_id);
                if(free_to_enter(student) && close_group == 0){
                    fase = calculate_percentage(remaining_simulation_time(simulation_time), config -> sim_time);
                    if(fase <= 85){
                        if(invitati > 0){
                            while(msgrcv(memory -> id_code_answer, &answer1,(sizeof(answer) - sizeof(long)),(getpid() + MATR), IPC_NOWAIT) !=(ssize_t) -1){
                                invitati--;
                                if(answer1.accept == 1){
                                    printf("Studente [%d]: {v} [%d] ha accettato il mio invito\n", student -> matr, answer1.mia_matr);
                                    student -> leader = 1;
                                    student -> free = 0;
                                    num_member++;
                                    create_message_to_gestore(memory -> id_code_student_gestore, student -> matr, answer1.mia_matr, 0);
                                    if((close_group = group_is_full(student, num_member))){
                                        create_message_to_gestore(memory -> id_code_student_gestore, student -> matr, 1, 0);
                                    }
                                }else
                                        printf("Studente [%d]: {x} Matr[%d] ha rifiutato il mio invito\n", student -> matr, answer1.mia_matr);
                            }
                        }else{
                            while(msgrcv(memory -> id_code_invite, &invite,(sizeof(invite) - sizeof(long)),(getpid() + MATR), IPC_NOWAIT) !=  (ssize_t) -1){
                                if(student -> free){
                                    printf("Studente [%d]: {<} ricevuto invito da [%d] con voto %d. Mio voto è  %d\n",student -> matr, invite.sender, invite.grade, student -> grade);
                                    if(student -> max_reject == 0){
                                        student -> free = 0;
                                        create_answer(1, invite.sender, (getpid() + MATR), memory -> id_code_answer);
                                    }else{
                                        if(evaluate_message(invite, student -> grade, memory ->id_code_answer, fase))
                                            student -> free = 0;
                                        else
                                            student -> max_reject--;
                                        }
                                }else{
                                    create_answer(0, invite.sender, (getpid() + MATR), memory -> id_code_answer);
                                }
                            }
                            if(permission_to_invite(student -> free, student -> leader, close_group, student -> nof_invites)){
                                if((matr_found = search_student_to_invite(student, showcase, fase, config ->pop_size))){
                                    student -> nof_invites--;
                                    invitati++;
                                    create_message_to_student(matr_found, student -> matr, student -> grade , memory -> id_code_invite);
                                }
                            }
                        }
                }else{
                    create_message_to_gestore(memory -> id_code_student_gestore, student -> matr ,0 , 1);
                    student -> free = 0;
                    student -> leader = 1;
                }
            }
            release_sem(memory -> critic_sem_id);
        }
}
