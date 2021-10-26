#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"

#define BUFFER_SIZE 200
#define HEADER_SIZE 30
#define END_DT 19

struct Stack_ {
    struct timeval **time_array;
    int rows, columns; 
    char** headers;
    double** points; 
};

struct timeval *string_to_timeval(char *time){
    struct timeval *tv = malloc(sizeof(struct timeval));
    char *tv_sec = malloc(END_DT*sizeof(char));
    char *tv_usec = malloc(sizeof(char));
    tv_sec[END_DT] = '\0';
    strncpy(tv_sec, time, END_DT);
    
    time_t result = 0;
    int year, month, day, hour, min, sec;

    if(sscanf(tv_sec, "%d-%d-%dT%d:%d:%d", &year,&month,&day,&hour,&min,&sec) == 6){
        struct tm tm_mid = {0};
        tm_mid.tm_year = year - 1900;
        tm_mid.tm_mon = month - 1;
        tm_mid.tm_mday = day;
        tm_mid.tm_hour = hour;
        tm_mid.tm_min = min;
        tm_mid.tm_sec = sec;
        
        if((result = mktime(&tm_mid)) == (time_t)-1){
            fprintf(stderr, "Failed to convert datetime to time_t \n");
            goto fail;
        }
    }
    else{
        fprintf(stderr, "Failed to parse datetime to time_t \n");
        goto fail;
    }
    tv->tv_sec = result;
    int idx = 0;
    while(time[END_DT + idx] != '\0'){
        idx ++;
    }
    if(!idx){
        free(tv_sec);
        return tv;
    }

    tv_usec = realloc(tv_usec,(idx)*sizeof(char));
    tv_usec[idx] = '\0';
    strncpy(tv_usec, time+END_DT, idx);
    
    double usec = atof(tv_usec) * 1000000;    
    tv->tv_usec = usec;

    if(tv){
        free(tv_sec);
        free(tv_usec);
        return tv;
    }
    else{goto fail;}
fail: 
    if( tv_sec ) free( tv_sec );
    if( tv_usec ) free( tv_usec );
    if( tv ) free( tv );
    fprintf(stderr, "Function failed: string_to_timeval\n");
    return NULL;
}
int read_csv(char* file, const char* const delim){
    FILE* fp = NULL;
    char* buffer = NULL;
    double** points = NULL;
    struct timeval **time_array = NULL;
    int columns = 0; 
    int rows = 0;
    struct Stack_ Stack;
    
    char **headers = (char**) calloc(1, sizeof(char*));
 
    if((fp = fopen(file, "r")) == NULL){
        fprintf(stderr, "Error: File does not exist");
        return 0;
    }
    if(!(buffer = (char*)calloc(BUFFER_SIZE, sizeof(char)))){
        fprintf(stderr, "Error: Error allocating buffer");
        return 0;
    }
    
    fgets(buffer, BUFFER_SIZE, fp);
    remove_NL_Char(buffer);
    
    char *token = strtok(buffer, delim);
    while (token) {
        headers = (char**) realloc(headers, (columns+1)*sizeof(char*));
        size_t length = strlen(token);
        headers[columns] = (char*)calloc(length, sizeof(char));
        strcpy(headers[columns],token);
        columns++;
        token = strtok(NULL, ",");
    }
    
    Stack.headers = headers;
    int fileSize = file_size(fp);
    int linesInt = (int) fileSize / ((columns-1)*sizeof(double)+20*sizeof(char));
    if(!(points = calloc(linesInt, sizeof *points))){
        fprintf(stderr, "Error: Error allocating csv to memory");
        return 0;
    }
    if(!(time_array = calloc(linesInt,sizeof *time_array))){
        fprintf(stderr, "Error allocating timeval array");
        return 0;
    };
    while(feof(fp) != true){
        fgets(buffer, BUFFER_SIZE, fp);
        remove_NL_Char(buffer);
        points[rows] = (double*)malloc((columns-1) * sizeof(double));
        
        char* token = strtok(buffer, delim);
        struct timeval *tm = NULL;
        tm = string_to_timeval(token);
        time_array[rows] = tm;
        
        token = strtok(NULL,",");
        int idx = 0;
        while(token){
            points[rows][idx] = atof(token);
            token = strtok(NULL,",");
            idx++;   
        }
        rows++;
        
        if(rows == linesInt){
            double** temp = realloc(points, (linesInt + 100)*sizeof(*points));
            struct timeval **temp_tr = realloc(time_array, (linesInt + 100)*sizeof(*time_array));
            if(!temp){
                fprintf(stderr,"Error: Error reallocting memory");
                return 0;
            }
            points = temp;
            time_array = temp_tr;
            linesInt = linesInt + 100;
            
        }
    }
    Stack.points = points;
    printf("%f",Stack.points[0][0]);
    Stack.time_array = time_array;
    
    if(fp) fclose(fp);
    for(int k = 0; k < linesInt; k++){
        free(points[k]);
    }
    free(points);
    free(time_array);
    free(buffer);
    free_char_array(headers, columns);
    return 1;
}

int main() {
    printf("Hello");
    
    char *time_string = "2021-08-21T13:45:40.105";
    struct timeval *tm = NULL;
    tm = string_to_timeval(time_string);
    
    if(tm){
        char mytime[20];                                         
        printf("%ld \n",(tm->tv_sec)); 
        time_t mid = tm->tv_sec;
        printf("%s", ctime(&mid));
    }
    
    
	char* path = "../data/test.csv";
    const char* const s = ",";
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    
    read_csv(path, s);
    
    end = clock();
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);
   

    return 0;
}