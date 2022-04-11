#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

#define ARG_LIMIT 10
#define READ 0
#define WRITE 1

char FILENAME[] = "language.txt";
char** commands;
size_t commands_capacity;
size_t commands_size;

char* get_line_alias(const char* line);
char* get_line_commands(const char* line);
void append_command(char* command);
char *rstrstrip(char *s);
char *strstrip(char *s);
char** parse_command(char* command);

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

//    preparing pipes
    size_t count = commands_size;
    int **pipes = calloc(count, sizeof(int *));
    for (int i = 0; i < count; ++i) {
        pipes[i] = calloc(2, sizeof(int));
        if (pipe(pipes[i]) < 0) {
            fprintf(stderr, "Cannot make the pipe\n");
            exit(1);
        }
    }

    char** result;
    for (int i = 0; i < commands_size; ++i) {
        printf("c: %s\n", commands[i]);
        result = parse_command(commands[i]);

//        execute command and connect it to pipes
        pid_t pid = fork();
        if (pid == 0){
//            set descriptors
            if (i > 0){
                dup2(pipes[i-1][READ], STDIN_FILENO);
            }
            if (i + 1 < count){
                dup2(pipes[i][WRITE], STDOUT_FILENO);
            }

//            close pipe in children
            for (int j = 0; j < count-1; ++j) {
                close(pipes[j][READ]);
                close(pipes[j][WRITE]);
            }
            execvp(result[0], result);
            exit(0);
        }


        for (int j = 0; result[j] != NULL; ++j) {
            free(result[j]);
        }
        free(result);
        result = NULL;
    }


//    close pipes
    for (int i = 0; i < count; ++i) {
        close(pipes[i][READ]);
        close(pipes[i][WRITE]);
    }
    for (int i = 0; i < count; ++i) {
        wait(0);
    }
    for(int i = 0; i < count; i++){
        free(pipes[i]);
    }
    free(pipes);


    int status;
    while (wait(&status)>0);

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

char** parse_command(char* command){
    char** result;
    if ((result = calloc(ARG_LIMIT+1, sizeof(char*)))==NULL){
        perror("Error");
        exit(1);
    }
    result[ARG_LIMIT] = NULL;


    char delimeters[] = " ";
    char * string;
    string = strtok(command, delimeters );
    size_t index = 0;
    while(string != NULL )
    {
        if ((result[index] = calloc(PATH_MAX, sizeof(char)))==NULL){
            perror("Error");
            exit(1);
        }
        strcpy(result[index], string);
        string = strtok(NULL, delimeters );
        index++;
    }
    result[index] = NULL;

    return result;
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
