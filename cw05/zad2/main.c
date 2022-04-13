#define _GNU_SOURCE
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 4){
        fprintf(stderr, "Provide: email-address title content");
        fprintf(stderr, "Provide: sort-mode");
        exit(1);
    }

    if(argc == 2){
//        DISPLAY SORTED

        char* mode = argv[1];
        char* command;
        if ((command = calloc(PATH_MAX, sizeof(char)))==NULL){
            perror("Error allocating memory");
            exit(1);
        }

        if(strcmp(mode, "date") == 0){
            strcpy(command, "echo | mail -H | sort -k 4");
        } else if (strcmp(mode, "sender") == 0){
            strcpy(command, "echo | mail -H | sort -k 3");
        } else {
            fprintf(stderr, "Wrong mode: %s\n", mode);
            exit(1);
        }

        FILE* file;
        if((file = popen(command, "r")) == NULL){
            perror("Could not open pipe");
            exit(1);
        }
        free(command);
        command = NULL;


        char* line = NULL;
        size_t len = 0;
        while(getline( &line, &len, file) != -1){
            printf("%s", line);
        }

    } else {
//        SEND MAIL

        char* address = argv[1];
        char* title = argv[2];
        char* content = argv[3];

        char* command = malloc((strlen(address)+ strlen(title)+ strlen(content)+50) * sizeof(char));

        sprintf(command, "echo %s | mail %s -s %s", content, address, title);
        printf("Sending mail with command:\n%s\n", command);

        if(popen(command, "r") == NULL){
            perror("Could not open pipe");
            exit(1);
        }
        free(command);

    }
}