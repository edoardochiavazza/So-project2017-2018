#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include "student_utils.h"
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>


int groupPreference(conf*);
int same_turn(int, int);


int calculate_percentage(int simulation_leftovers, int sim_time){
        return (int) ((simulation_leftovers * 100) / sim_time);
}

int remaining_simulation_time(int simulation_time){
        int current_time = time(NULL);
        return (current_time - simulation_time);
}

void create_answer(int choose, int matr_sender, int mia_matr, int mes_id){
        answer answer;
        answer.mtype = matr_sender;
        answer.accept = choose;
        answer.mia_matr = mia_matr;
        if(msgsnd(mes_id, &answer,(sizeof(answer) - sizeof(long)),0) == -1){
                printf("Error to send message \n");
                exit(EXIT_FAILURE);
        }
}

void create_message_to_student (int matr_found, int matr_sender, int max_grade_group, int id_code_invite){
        invite invite;
        invite.mtype = matr_found;
        invite.grade = max_grade_group;
        invite.sender = matr_sender;
        if(msgsnd(id_code_invite, &invite,(sizeof(invite) - sizeof(long)),0) == -1){
                printf("Errore create_message_to_student\n");
                throw_error(errno);
        }
}

int evaluate_message(invite invite, int studente_grade, int id_code_answer, int fase){

        if(fase < 25){
                if(invite.grade  >= studente_grade - 1){
                        create_answer(1, invite.sender, (getpid() + MATR), id_code_answer);
                        return 1;
                }else{
                        create_answer(0, invite.sender, (getpid() + MATR), id_code_answer);
                        return 0;
                }

        }else if(fase >= 25 && fase < 50 ){
                if(invite.grade  >= studente_grade - 2){
                        create_answer(1, invite.sender, (getpid() + MATR), id_code_answer);
                        return 1;
                }else{
                        create_answer(0, invite.sender, (getpid() + MATR), id_code_answer);
                        return 0;
                }

        }else if(fase >= 50 && fase < 65 ){
                if(invite.grade  >= studente_grade - 3){
                        create_answer(1, invite.sender, (getpid() + MATR), id_code_answer);
                        return 1;
                }else
                        create_answer(0, invite.sender, (getpid() + MATR), id_code_answer);
                        return 0;
        }else{
                        create_answer(1, invite.sender, (getpid() + MATR), id_code_answer);
                        return 1;
        }
}

int same_turn(int matr1, int matr2){
        if((matr1 % 2 != 0 && matr2 % 2 == 0) || (matr1 % 2 == 0 && matr2 % 2 != 0))
                return 0;
        else
                return 1;
}

void create_message_to_gestore(int id_coda_messaggi, int matr_leader, int matr_to_add, int close_all){
        message message;
        message.mtype = matr_leader;
        message.matr_leader = matr_leader;
        message.matr_to_add = matr_to_add;
        message.close_all = close_all;
        if(msgsnd(id_coda_messaggi, &message,(sizeof(message) - sizeof(long)),0) < 0){
            printf("Errore create_message_to_gestore\n");
            throw_error(errno);
        }
}

int search_student_to_invite(student* studente, student* showcase, int fase, int pop_size){
        int index;
        if(fase < 25){
                for(index = 0; index < pop_size; index++){
                                if((showcase + index) -> grade > studente -> grade + 3 && (showcase + index) -> matr != studente -> matr && (showcase + index) -> free && same_turn(studente -> matr, (showcase + index) -> matr) && (showcase + index) -> nof_elems == studente -> nof_elems){
                                        return (showcase + index) -> matr;
                                }
                }
        }else if(fase >= 25 && fase < 50 ){

                for(index = 0; index < pop_size; index++){
                                if((showcase + index) -> grade > studente -> grade + 2 && (showcase + index) -> matr != studente -> matr && (showcase + index) -> free && same_turn(studente -> matr, (showcase + index) -> matr) && (showcase + index) -> nof_elems == studente -> nof_elems){
                                        return (showcase + index) -> matr;
                                }
                }
        }else if(fase >= 50 && fase < 65 ){
                for(index = 0; index < pop_size; index++){
                                if((showcase + index) -> grade > studente -> grade + 1 && (showcase + index) -> matr != studente -> matr && (showcase + index) -> free && same_turn(studente -> matr, (showcase + index) -> matr) && (showcase + index) -> nof_elems == studente -> nof_elems){
                                        return (showcase + index) -> matr;
                                }
                }
        }else{
                for(index = 0; index < pop_size; index++){
                                if((showcase + index) -> matr != studente -> matr && (showcase + index) -> free && same_turn(studente -> matr, (showcase + index) -> matr)){
                                        return (showcase + index) -> matr;
                                }
                }
        }
        return 0;
}

int permission_to_invite(int free, int leader, int close_group, int nof_invites){
    if(free == 1 && leader == 0 && close_group == 0 && nof_invites > 0)
        return 1;
    else if(free == 0 && leader == 1 && close_group == 0 && nof_invites > 0)
        return 1;
    else
        return 0;
}

int free_to_enter(student* studente){
    if(studente -> free == 1 && studente -> leader == 0 )
        return 1;
    else if(studente -> free == 0 && studente -> leader == 1)
        return 1;
    else
        return 0;
}

int group_is_full(student* student, int num_member){
    if(student -> nof_elems > num_member) return 0;
    else{
        return 1;
    }
}
