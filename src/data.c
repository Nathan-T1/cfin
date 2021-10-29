#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"

#define BUFFER_SIZE 200
#define HEADER_SIZE 30

double get_h(char freq[]);

struct Stack_ {
    int init;
    struct timeval *timeArray;
    int rows, columns; 
    char** headers;
    double** points; 
};
void test(){
    char new_time[20];
    struct timeval tv;
    tv.tv_sec = 1629571806;
    tv.tv_usec = 0;
    printf("%i, %i \n", tv.tv_sec, tv.tv_usec);
    strcpy(new_time,timeval_to_string(tv));
    printf("%s \n \n", new_time);
}
int resample_stack(struct Stack_ stack, char freq[]){
    double **rs_points = NULL;
    struct timeval *rs_timeArray = NULL;
    double h, initTime, t0;
    int rs_rows = 1;
    h = get_h(freq);
    
    if(h == 0){
        fprintf(stderr, "Error: Failed to parse frequency");
    }
    
    initTime = timeval_to_double(stack.timeArray[0]);
    double remainder = fmod(initTime, h);
    t0 = initTime - remainder;
    
    int row_guess = (int) stack.rows / 2;
    if(!(rs_points = calloc(row_guess, sizeof *rs_points))){
        fprintf(stderr, "Error: Error allocating resample points to memory");
        return 0;
    }
    if(!(rs_timeArray = calloc(row_guess,sizeof(struct timeval)))){
        fprintf(stderr, "Error allocating resample timeval array");
        return 0;
    };
    
    
    
    rs_timeArray[0] = double_to_tv(t0);
    double timeMid;
    int idx = 0;
    while(idx < stack.rows){
        timeMid = timeval_to_double(stack.timeArray[idx]);
        if(timeMid > t0 + h *(rs_rows)){
            rs_timeArray[rs_rows] = double_to_tv(t0 + h*rs_rows);
            rs_rows++;
        }
        idx++;
    }


    for(int i = 0; i < rs_rows; i++){
        char new_time[20];
        char *timePtr = timeval_to_string(rs_timeArray[i]);
        timeval_to_string(rs_timeArray[i]);
        strcpy(new_time,timePtr);
        printf("%i, %i, %s \n", rs_timeArray[i].tv_sec, rs_timeArray[i].tv_usec, new_time);
        free(timePtr);
    }
 
    
    printf("resample_stack success \n");
    return 1;
    
}
struct Stack_ read_csv(char* file, const char* const delim){
    FILE* fp = NULL;
    char* buffer = NULL;
    double** points = NULL;
    struct timeval *timeArray = NULL;
    int columns = 0; 
    int rows = 0;
    struct Stack_ Stack;
    
    char **headers = (char**) calloc(1, sizeof(char*));
 
    if((fp = fopen(file, "r")) == NULL){
        fprintf(stderr, "Error: File does not exist");
        return Stack;
    }
    if(!(buffer = (char*)calloc(BUFFER_SIZE, sizeof(char)))){
        fprintf(stderr, "Error: Error allocating buffer");
        return Stack;
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
        return Stack;
    }
    if(!(timeArray = calloc(linesInt,sizeof(struct timeval)))){
        fprintf(stderr, "Error allocating timeval array");
        return Stack;
    };
    while(feof(fp) != true){
        fgets(buffer, BUFFER_SIZE, fp);
        points[rows] = (double*)malloc((columns-1) * sizeof(double));
        
        char* token = strtok(buffer, delim);
        struct timeval tm;
        tm = string_to_timeval(token);
        if(tm.tv_sec == 0){
            return Stack;
        }
        timeArray[rows] = tm;
        
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
            struct timeval *temp_tr = realloc(timeArray, (linesInt + 100)*sizeof(struct timeval));
            if(!temp){
                fprintf(stderr,"Error: Error reallocting memory");
                return Stack;
            }
            points = temp;
            timeArray = temp_tr;
            linesInt = linesInt + 100;
            
        }
    }
    Stack.points = points;
    Stack.columns = columns;
    Stack.rows = rows;
    Stack.timeArray = timeArray;
    Stack.init = 1;
    
    if(fp) fclose(fp);
    free(buffer);
    return Stack;
}
void free_stack(struct Stack_ Stack){
    for(int k = 0; k < Stack.rows; k++){
        free(Stack.points[k]);
    }
    free(Stack.points);
    free(Stack.timeArray);
    free_char_array(Stack.headers, Stack.columns);
    
}
void print_stack(struct Stack_ Stack){
    int m = Stack.columns;
    int n = Stack.rows;
    
    int idx = 0;
    while(idx < m){
        char new_time[20];
        strcpy(new_time,Stack.headers[idx]); 
        printf("%s, ",new_time);
        idx++;
    }
    printf(" \n");
    idx = 0;
    int jdx;
    while(idx < n){
        char new_time[20];
        struct timeval tm = Stack.timeArray[idx];
        strcpy(new_time,timeval_to_string(tm));
        printf("%s, ",new_time);
        
        jdx = 0;
        while(jdx < m - 1){
            printf("%f, ",Stack.points[idx][jdx]);
            jdx++;
        }
        printf(" \n");
        idx++;
    }
    printf(" \n");
    
    
}
double get_h(char freq[]){
    int ret;
    double h;
    ret = strcmp(freq, "S5");
    if(ret == 0){
        h = 5;
        return h;
    }
    ret = strcmp(freq, "S15");
    if(ret == 0){
        h = 15;
        return h;
    }
    ret = strcmp(freq, "S30");
    if(ret == 0){
        h = 30;
        return h;
    }
    ret = strcmp(freq, "M1");
    if(ret == 0){
        h = 60;
        return h;
    }
    ret = strcmp(freq, "M3");
    if(ret == 0){
        h = 180;
        return h;
    }
    ret = strcmp(freq, "M5");
    if(ret == 0){
        h = 300;
        return h;
    }
    return 0;
}






