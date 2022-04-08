#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>


/**
Create table of function which will be run in order
Parse input, for each skladnik_k find, parse and add commands to array
Run in loop until end of commands creating children, parent should die in each iteration

todo need to figure it out
while not all commands executed
    fork
    pipe
    child
        if commands left
            read from parent input and execute command
    parent
        send input either to child or to stdout if end of pipes
**/

char FILENAME[] = "../language.txt";
char** commands;
size_t commands_capacity;
size_t commands_size;

char* get_line_alias(const char* line);
char* get_line_commands(const char* line);
void append_command(char* command);
char *rstrstrip(char *s);
char *strstrip(char *s);
void parse_line_commands(char* line_commands);

int main(int argc, char* argv[]) {

    if (argc<2){
        fprintf(stderr, "Provide: string of piped commands\n");
        fprintf(stderr, "Example: %s składnik1 | składnik2 | składnik3", argv[0]);
        exit(1);
    }

    commands_capacity = 1;
    commands_size = 0;
    if ((commands = calloc(commands_capacity, sizeof(char*)))==NULL){
        perror("Error allocating file");
        exit(1);
    }

    FILE* fp;
    if ((fp = fopen(FILENAME, "r")) == NULL){
        perror("Error opening file");
        exit(1);
    }

    char* line_alias = NULL;
    char* line_commands = NULL;
    char* line = NULL;
    char* command;
    char* alias = NULL;
    if ((command = calloc(PATH_MAX, sizeof(char))) == NULL ||
            (alias = calloc(PATH_MAX, sizeof(char))) == NULL){
        perror("Error allocating memory");
        exit(1);
    }

    ssize_t read;
    size_t len = 0;
    char* alias_eptr = NULL;
    char* alias_sptr = argv[1];
    while (alias_sptr != NULL) {

        alias_eptr = strchr(alias_sptr, '|');

        strncpy(alias, alias_sptr, alias_eptr == NULL ? strlen(alias_sptr)+1 : (int)(alias_eptr-alias_sptr));

        fseek(fp, 0, SEEK_SET);
        while ((read = getline(&line, &len, fp)) != -1) {

            line_alias = get_line_alias(line);
            if (strcmp(strstrip(alias), strstrip(line_alias)) != 0)
                continue;

            line_commands = get_line_commands(line);

            char* sptr = line_commands;
            char* eptr = NULL;
            while (sptr != NULL){

                eptr = strchr(sptr, '|');

                strncpy(command, sptr, eptr == NULL ? strlen(sptr)+1 : (int)(eptr-sptr));
                append_command(command);

                sptr = eptr == NULL ? eptr : eptr+1;

            }


            free(line_alias);
            free(line_commands);
            line_alias = NULL;
            line_commands = NULL;
        }

        alias_sptr = alias_eptr == NULL ? alias_eptr : alias_eptr+1;

    }

    free(alias);
    alias = NULL;
    line_alias = NULL;
    line_commands = NULL;
    line = NULL;


    for (int i = 0; i < commands_size; ++i) {
        printf("c: %s\n", commands[i]);
    }
    printf("cap: %lu, size: %lu\n", commands_capacity, commands_size);

    return 0;
}


char* get_line_alias(const char* line){
    char DELIM[] = "=";
    char* dindex = strstr(line, DELIM);
    char* line_alias;
    if ((line_alias = calloc(PATH_MAX, sizeof(char))) == NULL){
        perror("Error allocating memory");
        exit(1);
    }

    strncpy(line_alias, line, (int)(dindex - line));
    rstrstrip(line_alias);
    return line_alias;
}

char* get_line_commands(const char* line){
    char DELIM[] = "=";
    char* dindex = strstr(line, DELIM);
    char* line_commands;
    if ((line_commands = calloc(PATH_MAX, sizeof(char))) == NULL){
        perror("Error allocating memory");
        exit(1);
    }

    strcpy(line_commands, dindex+1);
    rstrstrip(line_commands);
    return line_commands;
}

void append_command(char* command){
    if (commands_size >= commands_capacity){
        if ((commands = realloc(commands, 2*commands_size* sizeof(char *)))==NULL){
            perror("Error allocating memory");
            exit(1);
        }
        commands_capacity *= 2;
    }
    if ((commands[commands_size] = calloc(strlen(command)+1, sizeof(char)))==NULL){
        perror("Error allocating memory");
        exit(1);
    }
    strcpy(commands[commands_size], command);
    commands_size += 1;
}


char *rstrstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    return s;
}

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}