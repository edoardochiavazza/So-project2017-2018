#include "config.h"
#include "gestore_utils.h"
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <sys/shm.h>
#include <assert.h>


int groupPreference(conf*);

ipcs* fill_ipcs_struct(int shm_id_config, int sem_id, int shm_id_ipcs, int id_code_answer, int id_code_invite, int showcase_id_config, int config_sem_id, int critic_sem_id, int id_code_student_gestore){
        ipcs *memory;
        if((memory = shmat(shm_id_ipcs, NULL, 0)) == (void *) -1)
                throw_error(errno);
        memory -> shm_id_config = shm_id_config;
        memory -> sem_id = sem_id;
        memory -> id_code_answer = id_code_answer;
        memory -> id_code_invite = id_code_invite;
        memory -> showcase_id_config = showcase_id_config;
        memory -> config_sem_id = config_sem_id;
        memory -> critic_sem_id = critic_sem_id;
        memory -> id_code_student_gestore = id_code_student_gestore;
        return memory;
}

void fill_student_struct(conf* config, student* s ,int child_pid){
        srand48(getpid());
        s -> grade = rand() % 13 + 18;
        s -> matr = MATR + child_pid;
        s -> nof_elems = groupPreference(config);
        s -> nof_invites = config -> nof_invites;
        s -> max_reject = config -> max_rejects;
        s -> leader = 0;
        s -> free = 1;
}

int groupPreference(conf* config){
        srand48(getpid());
        int nof_elems_two = config -> perc_two_people;
        int nof_elems_three = config -> perc_three_people;
        int random_number = rand() % 100;
        if( random_number >= 0 && random_number < nof_elems_two)
                return 2;
        else if( random_number < (nof_elems_three + nof_elems_two) && random_number >= nof_elems_two)
                return 3;
        else
                return 4;
}

node* init_new_group(student* student_leader){
        node* new_group = (node*)malloc(sizeof(node));
        assert(new_group != NULL);
        int i;
        new_group -> team[0] = student_leader;
        new_group -> mean_grade_SO = 0;
        new_group -> mean_grade_AdE = 0;
        new_group -> next = NULL;
        new_group -> grade_max = 0;
        new_group -> num_member = 1;
        new_group -> increment  = 0;
        for(i = 1; i < 4; i++){
                new_group -> team[i] = NULL;
        }
        return new_group;
}

void init_list(list* group_list){
        group_list -> head = NULL;
        group_list -> increment_average = 0;
        group_list -> list_lenght = 0;
        group_list -> avg_group_SO = 0;
        group_list -> avg_group_AdE = 0;
}

void add_student_to_group(int matr_leader, student* student_to_add, list* group_list){
        node* prt = group_list -> head;
        assert(prt != NULL);
        while(matr_leader != prt -> team[0] -> matr){
            prt = (node*)prt -> next;
        }
        int j;
        for(j = 0; prt -> team[j] != NULL; j++){
        }
        prt -> team[j] = student_to_add;
        prt -> num_member++;
}

int control_existing_group(int matr_leader, list* group_list){
    node* prt = group_list -> head;
    while(prt != NULL && matr_leader != prt -> team[0] -> matr){
        prt = (node*)prt -> next;
    }
    if(prt == NULL)
        return 0;
    else
        return 1;
}

void create_new_group(student* matr_leader,list* group_list){
    node* new_group = init_new_group(matr_leader);
    node* prt = group_list -> head;
    group_list -> list_lenght ++;
    if(prt == NULL){
        group_list -> head = new_group;
    }
    else{
        while (prt -> next != NULL) {
            prt = (node*)prt -> next;
        }
        prt -> next = (struct node*) new_group;
    }
}

void close_all_groups(student* student_leader, list* group_list){
    node* prt = group_list -> head;
    assert(prt != NULL);
    int matr = student_leader -> matr;
    if(control_existing_group(matr, group_list)){
        while(matr != prt -> team[0] -> matr){
            prt = (node*)prt -> next;
        }
        prt -> group_closed = 1;
    }else
        create_new_group(student_leader, group_list);
        while(matr != prt -> team[0] -> matr){
                prt = (node*) prt -> next;
        }
        prt -> group_closed = 1;
}

student* search_student_add(int matr, student* showcase, int pop_size){
    for(int i = 0; i < pop_size; i++){
        if(matr == (showcase + i) -> matr){
            return (showcase + i);
        }
    }
        return (student*) -1;
}

void calc_average(list* group_list){
    node* prt = group_list -> head;
    assert(prt != NULL);
    while(prt != NULL){
        int groupsize = 0;
        prt -> grade_max = prt -> team[0] -> grade;
        for(int i = 0; i < prt -> num_member; i++){
            prt -> mean_grade_AdE += prt -> team[i] -> grade;
            groupsize++;
            if(prt -> grade_max < prt -> team[i] -> grade)
                prt -> grade_max = prt -> team[i] -> grade;
            }
        for(int i = 0; i < prt -> num_member; i++){
            if(prt -> team[i] -> nof_elems == groupsize)
                prt -> mean_grade_SO += prt -> grade_max;
            else
                prt -> mean_grade_SO += (prt -> grade_max - 3);
        }
        prt -> mean_grade_AdE = prt -> mean_grade_AdE / prt -> num_member;
        group_list -> avg_group_AdE += prt -> mean_grade_AdE;

        prt -> mean_grade_SO =  prt -> mean_grade_SO / prt -> num_member;
        group_list -> avg_group_SO += prt -> mean_grade_SO;
        prt -> increment  = prt -> mean_grade_SO - prt -> mean_grade_AdE;
        group_list -> increment_average += prt -> increment;
        prt = (node*) prt -> next;
        }
    group_list -> avg_group_AdE = group_list -> avg_group_AdE / group_list -> list_lenght;
    group_list -> avg_group_SO = group_list -> avg_group_SO / group_list -> list_lenght;
    group_list -> increment_average = group_list -> increment_average / group_list -> list_lenght;
}

void print_groups(list* group_list){
    system("clear");
    printf("____________________________________________________________________________\n");
    node* prt = group_list -> head;
    int i;
    int cont = 1;
    assert(prt != NULL);
    while(prt != NULL){
        printf("\nGruppo[%d] : Matricole ", cont);
        for(i = 0; i < prt -> num_member; i++){
            printf("[%d];", prt -> team[i] -> matr);
        }
        cont ++;
        printf("\n    Voto Architettura: max[%d], medio(%.2f); Voto Sistemi Operativi [%.2f]; Incremento = [%.2f]\n", prt -> grade_max, prt -> mean_grade_AdE, prt -> mean_grade_SO, prt -> increment );
        prt = (node*) prt -> next;
    }
}

void  print_result(list* group_list) {
    printf("____________________________________________________________________________\n");
    printf("\nNUMERO DI STUDENTI PER OGNI VOTO:\n");
    printf("    _________________________\n" );
    printf("    | VOTO | N° AdE | N° SO |\n" );
    for(int i = 15; i <= 30;  i++){
        node* prt = group_list -> head;
        int AdEnum = 0;
        int SOnum = 0;
        while(prt != NULL){
            for(int j = 0; j < prt -> num_member; j++){
                AdEnum += (prt -> team[j] -> grade == i);
                SOnum += (prt -> mean_grade_SO == i);
            }
            prt = (node*) prt -> next;
        }
        printf("    |-----------------------|\n");
        printf("    |%*d%*s|%*d%*s|%*d%*s| \n", 3, i, 3, "", 4, AdEnum, 4, "", 4, SOnum, 3, "");
    }
    printf("    '-----------------------'\n\n VALORI MEDI:\n=============================================\n");
    printf("| Voto medio d'Architettura       : %.2f \n", group_list -> avg_group_AdE);
    printf("| Voto medio di Sistemi Operativi : %.2f \n", group_list -> avg_group_SO);
    printf("| Incremento medio dei voti       : %.2f \n", group_list -> increment_average);
    printf("=============================================\n");
}

void list_free(list* group_list){
    while (group_list ->head != NULL){
        node* tmp = group_list ->head;
        group_list ->head =(node*) group_list ->head->next;
        free(tmp);
    }
    free(group_list);
}
