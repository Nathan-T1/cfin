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
struct Condition_ {
    int is_ind_a;
    int is_ind_b;
    int c_pos;
    bool(*comp)(const double, const double);
    char* col_a;
    char* col_b;
    double c;
};
struct Backtest_{
    int init;
    int sources;
    char** files; 
    struct Stack_* stacks;
    
    struct Condition_** entries; 
    struct Condition_** exits; 
};

void print_stack(struct Stack_ Stack);
struct Stack_ resample_stack(struct Stack_ stack, char freq[]);
void free_stack(struct Stack_ Stack);
struct Stack_ read_csv(char* file, const char* const delim, char* dt_format, char* dt_order);
int write_csv(struct Stack_ Stack, char* file);

int get_idx(struct Stack_* stack, const char* col, int indicator);
int add_indicator(struct Stack_* stack, struct Indicator_ indicator);

#endif