#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

double f(double x){
    return 4/(1+x*x);
}

double min(double x, double y){
    if (x<y)
        return x;
    return y;
}

int main(int argc, char* argv[]){

    if (argc < 5){
        fprintf(stderr, "Provide: interval_start interval_end step number\n");
        exit(1);
    }

//    printf("%s %s %s %s\n", argv[4], argv[1], argv[2], argv[3]);

    double in_start = strtod(argv[1], NULL);
    double in_end = strtod(argv[2], NULL);
    double step = strtod(argv[3], NULL);

    double start = in_start;
    double end;

    double sum = 0;
    double mid;

    while (start<in_end){

        end = min(start+step, in_end);
        mid = (start + end)/2;
        sum += f(mid)*(end-start);

        start+=step;
    }

    char* filename = calloc(strlen(argv[4])+6,sizeof(char));
    strcpy(filename, "w");
    strcat(filename, argv[4]);
    strcat(filename, ".txt");


    char value[51];
    snprintf(value, 50, "%f", sum);

    FILE* file = fopen(filename, "w");
    fwrite(value, sizeof(char), strlen(value), file);

    free(filename);
    fclose(file);

    return 0;
}
