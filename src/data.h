#ifndef _data_h
#define _data_h

struct Stack_ {
    int init;
    struct timeval *timeArray;
    int rows, columns; 
    char** headers;
    double** points; 
};

void print_stack(struct Stack_ Stack);
struct Stack_ resample_stack(struct Stack_ stack, char freq[]);
void free_stack(struct Stack_ Stack);
struct Stack_ read_csv(char* file, const char* const delim, char* dt_format, char* dt_order);
int write_csv(struct Stack_ Stack, char* file);

double* Backtest_RSI(struct Stack_ stack, int length);
#endif