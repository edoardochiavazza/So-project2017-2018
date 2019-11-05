#ifndef STUDENT_UTILS_H
#define STUDENT_UTILS_H

#define MATR 8000000

typedef struct _student{                    //student structure
        int grade;                          //AdE grade of the student
        int matr;                           //ID of the student
        int nof_elems;                      //preferred size of group
        int nof_invites;                    //maximum number of invites that can send
        int max_reject;                     //aximum number of refuse to an invite
        int free;                           //student availability to enter in a group
        int leader;                         //student is leader of a group
} student;


typedef struct _invite {                    //invite request message Structure
        long mtype;                         //ID of the addressed student
        int grade;                          //grade of the sender
        int sender;                         //ID of the sender student
} invite;

typedef struct _answer{                     //answer message Structure
        long mtype;                         //ID of the sender to answer
        int accept;                         //invite accepted or refused
        int mia_matr;                       //ID of the answering student

} answer;

typedef struct _message{                    //message from student to gestore that adds member to the group
        long mtype;                         //PID of gestore process
        int matr_leader;                    //ID of the leader's group
        int matr_to_add;                    //ID of the student that has to be added in the group
        int close_all;                      //message that close the group when the simulation has reached a certain time
} message;

int calculate_percentage(int, int);                                     //return the remaining simulation time percentage
int remaining_simulation_time(int);                                     //return the remaining simulation time
void create_answer(int, int ,int ,int);                                 //create the answer to a received message
void create_message_to_student (int, int, int, int);                    //create the message to send
int evaluate_message(invite, int, int, int);                            //analyze and answer accordingly a received message
int same_turn(int, int);                                                //check if the students are in the same turn
void create_message_to_gestore(int, int, int, int );                    //function that allows the comunication between student and gestore
int search_student_to_invite(student*, student*, int, int);             //return the student id to invite
int permission_to_invite(int, int, int, int);                           //check the student permission ti send messages
int free_to_enter(student*);                                            //check the student permission to enter the critical zone
int group_is_full(student*, int);                                       //check if the group is full
int groupPreference(conf*);                                             //return the group's size preference
#endif
