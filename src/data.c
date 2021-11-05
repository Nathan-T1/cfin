#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"

#define BUFFER_SIZE 200
#define HEADER_SIZE 30

struct Indicator_ {
    char* name; 
    int init;
    int lookback;
    double* vals;
};

struct Stack_ {
    int init;
    int ind_count;
    struct timeval *timeArray;
    int rows, columns; 
    char** headers;
    double** points;   
    struct Indicator_* indicators;
};

double get_h(char freq[]);
void print_stack(struct Stack_ Stack);

struct Stack_ resample_stack(struct Stack_ stack, char freq[]){
    double **rs_points = NULL;
    struct timeval *rs_timeArray = NULL;
    double h, initTime, t0, bin_max;
    struct Stack_ rs_stack;
    int rs_rows = 1;
    h = get_h(freq);
    
    if(h == 0){
        fprintf(stderr, "Error: Failed to parse frequency");
        return rs_stack;
    }
    
    initTime = timeval_to_double(stack.timeArray[0]);
    double remainder = fmod(initTime, h);
    t0 = initTime - remainder;
    
    int row_guess = (int) stack.rows / 2;
    if(!(rs_points = calloc(row_guess, sizeof *rs_points))){
        fprintf(stderr, "Error: Error allocating resample points to memory");
        return rs_stack;
    }
    if(!(rs_timeArray = calloc(row_guess,sizeof(struct timeval)))){
        fprintf(stderr, "Error allocating resample timeval array");
        return rs_stack;
    };
    
    rs_timeArray[0] = double_to_tv(t0);
    double timeMid;
    int idx;
    while(idx < stack.rows){
        timeMid = timeval_to_double(stack.timeArray[idx]);
        while(timeMid >= t0 + h *(rs_rows)){
            rs_timeArray[rs_rows] = double_to_tv(t0 + h*rs_rows);
            rs_rows++;
            if(rs_rows == row_guess){
                fprintf(stderr, "Resample array approaching allocated size, resizing array \n");
                row_guess = row_guess + 100;
                double** temp = realloc(rs_points, (row_guess)*sizeof(*rs_points));
                struct timeval *temp_tr = realloc(rs_timeArray, (row_guess)*sizeof(struct timeval));
                
                if(!temp){
                    fprintf(stderr,"Error: Error reallocting memory");
                    return rs_stack;
                }
                rs_points = temp;
                rs_timeArray = temp_tr;
            }
        }
        idx++;
    }
    
    idx = 0;
    timeMid = timeval_to_double(stack.timeArray[idx]); 
    for(int rs_idx = 0; rs_idx < rs_rows; rs_idx++){
        bin_max = timeval_to_double(rs_timeArray[rs_idx]);
        rs_points[rs_idx] = (double*)malloc((stack.columns-1) * sizeof(double));
        int jdx = 0;
        while(timeMid <= bin_max && idx < stack.rows){
            if(jdx == 0){
                rs_points[rs_idx][0] = stack.points[idx][0];
                rs_points[rs_idx][1] = stack.points[idx][1];
                rs_points[rs_idx][2] = stack.points[idx][2];
                rs_points[rs_idx][4] = stack.points[idx][4];
                jdx++;
            }
            if(stack.points[idx][1] > rs_points[rs_idx][1]){
                rs_points[rs_idx][1] = stack.points[idx][1];
            }
            if(stack.points[idx][2] < rs_points[rs_idx][2]){
                rs_points[rs_idx][2] = stack.points[idx][2];
            }
            rs_points[rs_idx][4] += stack.points[idx][4];
            idx++;
            timeMid = timeval_to_double(stack.timeArray[idx]); 
        }
        if(idx != 0){
            rs_points[rs_idx][3] = stack.points[idx-1][3];
        }
        
        printf("\n");
    }
    rs_stack.headers = stack.headers;
    rs_stack.points = rs_points;
    rs_stack.columns = stack.columns;
    rs_stack.rows = rs_rows;
    rs_stack.timeArray = rs_timeArray;
    rs_stack.init = 1;
    
    printf("resample_stack success \n");
    return rs_stack;
}
struct Stack_ read_csv(char* file, const char* const delim, char* dt_format, char* dt_order){
    FILE* fp = NULL;
    char* buffer = NULL;
    double** points = NULL;
    struct timeval *timeArray = NULL;
    int columns = 0; 
    int rows = 0;
    struct Stack_ Stack;
    memset(&Stack, 0, sizeof(struct Stack_));
    
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
        tm = string_to_timeval(token, dt_format, dt_order);
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
int write_csv(struct Stack_ Stack, char* file){
    FILE* fptr = NULL;
    int m = Stack.columns;
    int n = Stack.rows;
    if((fptr = fopen(file, "w")) == NULL){
        fprintf(stderr, "Error: Error writing to file");
        return 0;
    }
    char* headers = (char*) malloc(100 * sizeof(char));
    int idx = 0;
    int jdx = 0;
    stpcpy(headers, Stack.headers[idx]);
    strcat(headers, ",");
    idx++;
    while(idx < m){
        strcat(headers, Stack.headers[idx]);
        strcat(headers,",");
        idx++;
        if(idx == m){
            strcat(headers, "\n");
        }
    }
    fprintf(fptr, "%s", headers);
    
    idx = 0;
    while(idx < n){
        char time[30];
        struct timeval tm = Stack.timeArray[idx];
        strcpy(time,timeval_to_string(tm));
        strcat(time,",");
        fprintf(fptr, "%s", time);
        
        jdx = 0;
        while(jdx < m - 1){
            jdx++;
            if(jdx < m - 1){
                fprintf(fptr, "%f,", Stack.points[idx][jdx-1]); 
            }
            else{
                fprintf(fptr, "%f\n", Stack.points[idx][jdx-1]); 
            }
        }    
        idx++;
    }
    fclose(fptr);
}
void free_indicator(struct Indicator_ indicator){
    free(indicator.vals);
}
void free_stack(struct Stack_ Stack){
    for(int k = 0; k < Stack.rows; k++){
        free(Stack.points[k]);
    }
    free(Stack.points);
    free(Stack.timeArray);
    free_char_array(Stack.headers, Stack.columns);
    
    if(Stack.ind_count > 0){
        for(int i = 0; i < Stack.ind_count; i++){
            printf("%i \n",i);
            free_indicator(Stack.indicators[i]);
        }
        free(Stack.indicators);
    }
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
    while(idx < 25){
        char new_time[20];
        struct timeval tm = Stack.timeArray[idx];
        strcpy(new_time,timeval_to_string(tm));
        printf("%s, ",new_time);
        
        jdx = 0;
        while(jdx < m - 1){
            printf("%f, ",Stack.points[idx][jdx]);
            jdx++;
        }
        if(Stack.ind_count > 0){
            for(int i = 0; i < Stack.ind_count; i++){
                printf("%f ", Stack.indicators[i].vals[idx]);
            }
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

