#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/shm.h>
#include <math.h>

//return numeric value of the string as parameter (POPSIZE = 10 return 10)
int get_numeric_value(char *);

int get_numeric_value(char *line) {
        char *token;
        token = strtok(line, DELIMITER);
        token = strtok(NULL, DELIMITER);
        return atoi(token);
}

conf* fill_config_struct(int shm_id_config) {
        FILE *fp;
        size_t buffer_size = 0;
        char *buffer = NULL;
        conf *config;
        if((config = shmat(shm_id_config, NULL, 0)) == (void *) -1)
                throw_error(errno);


        if((fp = fopen(CONFIG_PATH, "r")) == NULL) {
                printf("Errore apertura file %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        while(getline(&buffer, &buffer_size, fp) != -1) {
                if((strstr(buffer, "POP_SIZE")) != NULL) {
                        config->pop_size = get_numeric_value(buffer);
                } else if((strstr(buffer, "SIM_TIME")) != NULL) {
                        config->sim_time = get_numeric_value(buffer);
                } else if((strstr(buffer, "perc_four_people")) != NULL) {
                        config->perc_four_people = get_numeric_value(buffer);
                } else if((strstr(buffer, "perc_three_people")) != NULL) {
                        config->perc_three_people = get_numeric_value(buffer);
                } else if((strstr(buffer, "perc_two_people")) != NULL) {
                        config->perc_two_people = get_numeric_value(buffer);
                } else if((strstr(buffer, "NOF_INVITES")) != NULL) {
                        config->nof_invites = get_numeric_value(buffer);
                } else if((strstr(buffer, "NOF_REJECT")) != NULL) {
                        config->max_rejects = get_numeric_value(buffer);
                }
        }
        fclose(fp);
        return config;
}

void throw_error(int errNumber){
        fprintf(stderr, "Error in %s (%d) : %s\n",__FILE__,__LINE__,strerror(errNumber));
        exit(EXIT_FAILURE);
}

char *integer_to_string(int arg) {
	int arg_length = (arg == 0) ? 1 : (int)((ceil(log10(arg))+1)*sizeof(char));
	char *string = malloc(sizeof(arg) * arg_length);
	sprintf(string, "%d", arg);
	return string;
}
