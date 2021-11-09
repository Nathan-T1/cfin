#ifndef _data_h
#define _data_h

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
};

void print_stack(struct Stack_ Stack);
struct Stack_ resample_stack(struct Stack_ stack, char freq[]);
void free_stack(struct Stack_ Stack);
struct Stack_ read_csv(char* file, const char* const delim, char* dt_format, char* dt_order);
int write_csv(struct Stack_ Stack, char* file);

#endif