#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define BUFFER_SIZE 200
#define HEADER_SIZE 30

struct Stack_ {
    struct timeval *time_array;
    int rows, columns; 
    char** headers;
    double** points; 
};
static void free_char_array(char** arr, int size){
    for(size_t i = 0; i < size; i++){
        free(arr[i]);
    }
    free(arr);
}
void removeNewLineChar(char *ptr){
    while((ptr != NULL) && (*ptr != '\n')){
        ++ptr;
    }
    *ptr = '\0';
}
int file_size(FILE *fp){
    int start = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp,start, SEEK_SET);
    return size;
}
int read_csv(char* file, const char* const delim){
    FILE* fp = NULL;
    char* buffer = NULL;
    double** points = NULL;
    int columns = 0; 
    int rows = 0;
    struct Stack_ Stack;
    
    char **headers = (char**) calloc(1, sizeof(char*));
    //struct timeval *tr = (struct timeval*)malloc(1*sizeof(struct timeval));
 
    if((fp = fopen(file, "r")) == NULL){
        fprintf(stderr, "Error: File does not exist");
        return 0;
    }
    if(!(buffer = (char*)calloc(BUFFER_SIZE, sizeof(char)))){
        fprintf(stderr, "Error: Error allocating buffer");
        return 0;
    }
    
    fgets(buffer, BUFFER_SIZE, fp);
    removeNewLineChar(buffer);
    
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
    while(feof(fp) != true){
        fgets(buffer, BUFFER_SIZE, fp);
        removeNewLineChar(buffer);
        points[rows] = (double*)malloc((columns-1) * sizeof(double));
        
        char* token = strtok(buffer, delim);
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
            if(!temp){
                fprintf(stderr,"Error: Error reallocting memory");
                return 0;
            }
            points = temp;
            linesInt = linesInt + 100;
            
        }
    }
    
    if(fp) fclose(fp);
    for(int k = 0; k < linesInt; k++){
        free(points[k]);
    }
    free(points);
    free(buffer);
    free_char_array(headers, columns);
    return 1;
}

int main() {
	char* path = "test.csv";
    const char* const s = ",";
    read_csv(path, s);
	return 0;
}