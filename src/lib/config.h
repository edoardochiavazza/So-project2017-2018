#ifndef CONFIG_H
#define CONFIG_H

#define DELIMITER "="
#define CONFIG_PATH "../config/opt.conf"


typedef struct _conf{                   //configuration values as in opt.config
        int pop_size;
        int sim_time;
        int perc_four_people;
        int perc_three_people;
        int perc_two_people;
        int nof_invites;
        int max_rejects;
} conf;

void throw_error(int);                  //fuction of error signal.
conf *fill_config_struct();             //fill the configuration structure.


#endif
