#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "macro.h"

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
struct Backtest_{
    int init;
    int sources;
    char** files; 
    struct Stack_* stacks;
};

double get_h(char freq[]);
void print_stack(struct Stack_ Stack);

int get_idx(struct Stack_ stack, const char* col ,int indicator){
    if(!indicator){
        for(int i = 1; i < stack.columns - 1; i++){
            const char* header = stack.headers[i];
            int result= strcmp(col, header);
            if(result == 0){
                return i-1;
            }
        }   
    }
    if(indicator == 1){
        for(int i = 1; i < stack.ind_count; i++){
            const char* header = stack.indicators[i].name;
            int result= strcmp(col, header);
            if(result == 0){
                return i;
            }
        }
    }
    fprintf(stderr, "Failed to find column passed");
    return 0;
}
int add_indicator(struct Stack_* stack, struct Indicator_ indicator){
    if(stack->ind_count == 0){
        struct Indicator_* indicators = malloc(sizeof(struct Indicator_));
        indicators[stack->ind_count] = indicator;
        stack->indicators = indicators;
        stack->ind_count++;
        return 1;
    }
    else if (stack->ind_count >= 1){
        stack->ind_count++;
        struct Indicator_* indicators = stack->indicators;
        struct Indicator_* temp = realloc(indicators, (stack->ind_count)*sizeof(struct Indicator_));
        if(!temp){
            fprintf(stderr,"Error: Error reallocting memory");
            return 0;
        }
        stack->indicators = temp;
        stack->indicators[stack->ind_count] = indicator;
        return 1;
    }
}
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
    
    int columns = 0; 
    int rows = 0;
    
    struct timeval *timeArray = NULL;
    struct Stack_ Stack;
    struct timeval tm;
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
            printf("Increaseing memory allocated");
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
    char* headers = (char*) calloc(250,sizeof(char));
    int idx = 0;
    int jdx = 0;
    stpcpy(headers, Stack.headers[idx]);
    strcat(headers, ",");
    idx++;
    while(idx < m){
        char header[20];
        stpcpy(header, Stack.headers[idx]);
        if(idx == m-1){
            header[strlen(header)-1]='\0'; 
        }  
        strcat(headers, header);
        if(idx < m - 1 || (idx == m - 1 && Stack.ind_count)){
            strcat(headers, ",");
        }
        idx++;
        if(idx == m && !Stack.ind_count){
            strcat(headers, "\n");
        }

    }
    if(Stack.ind_count){
        for(int i = 0; i < Stack.ind_count; i++){
            char header[20];
            stpcpy(header, Stack.indicators[i].name);
            strcat(headers, header);
            if(i == Stack.ind_count - 1){
                strcat(headers, "\n"); 
            }
            else{
                strcat(headers, ",");
            }
        }
    }
    fprintf(fptr, "%s", headers);
    free(headers);
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
                if(!Stack.ind_count){
                    fprintf(fptr, "%f\n", Stack.points[idx][jdx-1]); 
                }
                else{
                    fprintf(fptr, "%f,", Stack.points[idx][jdx-1]); 
                }
            }
        }   
        jdx = 0;
        if(Stack.ind_count){
            while(jdx < Stack.ind_count){
                jdx++;
                if(jdx < Stack.ind_count){
                    fprintf(fptr, "%f,", Stack.indicators[jdx-1].vals[idx]);  
                } 
                if(jdx == Stack.ind_count){
                    fprintf(fptr, "%f", Stack.indicators[jdx-1].vals[idx]);  
                    fprintf(fptr,"\n");
                }
            }
        }
        
        idx++;
    } 
    fclose(fptr);
    return 1;
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
        if(Stack.ind_count){
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