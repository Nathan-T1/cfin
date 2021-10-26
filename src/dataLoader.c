#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"

struct Stack_ read_csv(char* file, const char* const delim){
    FILE* fp = NULL;
    char* buffer = NULL;
    double** points = NULL;
    struct timeval *time_array = NULL;
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
    if(!(time_array = calloc(linesInt,sizeof(struct timeval)))){
        fprintf(stderr, "Error allocating timeval array");
        return Stack;
    };
    while(feof(fp) != true){
        fgets(buffer, BUFFER_SIZE, fp);
        remove_NL_Char(buffer);
        points[rows] = (double*)malloc((columns-1) * sizeof(double));
        
        char* token = strtok(buffer, delim);
        struct timeval tm;
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
            struct timeval *temp_tr = realloc(time_array, (linesInt + 100)*sizeof(struct timeval));
            if(!temp){
                fprintf(stderr,"Error: Error reallocting memory");
                return Stack;
            }
            points = temp;
            time_array = temp_tr;
            linesInt = linesInt + 100;
            
        }
    }
    Stack.points = points;
    Stack.time_array = time_array;
    Stack.init = 1;
    
    if(fp) fclose(fp);
    for(int k = 0; k < linesInt; k++){
        free(points[k]);
    }
    free(points);
    free(time_array);
    free(buffer);
    free_char_array(headers, columns);
    return Stack;
}