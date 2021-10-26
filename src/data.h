#ifndef _data_h
#define _data_h

struct Stack_ {
    int init;
    struct timeval *time_array;
    int rows, columns; 
    char** headers;
    double** points; 
};

struct Stack_ read_csv(char* file, const char* const delim);
#endif