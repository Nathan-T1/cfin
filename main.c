#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER_SIZE 200
#define HEADER_SIZE 30
struct Point_ {
    char* t;
    float open;
    float high;
    float low;
    float close;
    unsigned long volume;
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
static void handle_row(char* buffer, const char* const delim, struct timeval* tv, float* step, int cols){
    char* token;
    
    token = strtok(buffer, delim);
    while(token){
        printf("%s \n", token);
        token = strtok(NULL,",");
    }
}
int read_csv(char* file, const char* const delim){
    FILE* fp;
    char* buffer;
    int columns = 0; 
    int rows = 0;
    
    char **headers = (char**) calloc(1, sizeof(char*));
    float **arr = (float**) calloc(1, sizeof(float*));
    struct timeval *tr = (struct timeval*)malloc(1*sizeof(struct timeval));
    
    if(headers == NULL || arr == NULL || tr == NULL){
        fprintf(stderr, "Error: Error allocating memory");
    }
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
    while(feof(fp) != true){
        arr = (float**) realloc(arr, (rows + 1)*sizeof(float*));
        tr = (struct timeval*) realloc(tr, (rows + 1)*sizeof(struct timeval));
        arr[rows] = (float*)calloc(columns, sizeof(float));
        
        fgets(buffer, BUFFER_SIZE, fp);
        handle_row(buffer, delim, &tr[rows], arr[rows], columns);
        rows++;
        
    }

    free_char_array(headers, columns);
    return 1;
}

int main() {
	char* path = "test.csv";
    const char* const s = ",";
    read_csv(path, s);
	return 0;
}