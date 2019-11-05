#ifndef GESTORE_UTILS_H
#define GESTORE_UTILS_H

#define PROGRAM "./student"
#include "student_utils.h"

 typedef struct _ipcs{
        int shm_id_config;
        int sem_id;
        int id_code_answer;
        int id_code_invite;
        int config_sem_id;
        int showcase_id_config;
        int critic_sem_id;
        int id_code_student_gestore;

} ipcs;

typedef struct _node{                       //group Structure
        student* team[4];                   //array of the students members of the group
        struct node* next;                  //pointer at the next group
        float mean_grade_SO;                //operative System grade
        float mean_grade_AdE;               //average Architecture grade
        float increment;                    //increment of the grades of the group
        int num_member;                     //number of student of the gruoup
        int group_closed;                   //flag for group closed/open
        int grade_max;                      //maximum Architecture grade
}node;

typedef struct _list{                       //list of the groups Structure
        node* head;                         //pointer at the top of the list
        float increment_average;            //average increment across the groups
        int list_lenght;                    //number of groups on the list
        float avg_group_AdE;                //average Architecture grade
        float avg_group_SO;
}list;

ipcs* fill_ipcs_struct(int, int, int, int, int, int, int, int, int);    //fill a shared memory with IPCS's keys
void fill_student_struct(conf*, student*, int);                         //fill the student structure
node* init_new_group(student*);                                         //return a new group
void init_list(list*);                                                  //fill the list of groups structure
void add_student_to_group(int, student*, list*);                        //add the student to the group
int control_existing_group(int, list*);                                 //check if there is a group matching the leader id
void create_new_group(student* ,list*);                                 //create a new empty group
void close_all_groups(student*, list*);                                 //close the still existing groups, create and close one for each student groupless
student* search_student_add(int, student* , int);                       //return the address of the student in the showcase, ready to be added in a group
void calc_average(list*);                                               //compute and set the various grades for each group of the list
void print_groups(list*);                                               //print on screen each group created, its members, grades of each subject and the increment of it
void print_result(list*);                                               //print on screen the number of students that get each grades in the two subjects and the average grade and increment
void list_free(list*);                                                  //free the memory occupied by the groups on the list of them
char *integer_to_string(int);                                           //integer to String conversion needed for execl


#endif
