#ifndef _data_h
#define _data_h

struct Indicator_ {
    char* name; 
    int init;
    int lookback;
    double* vals;
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
struct Stack_ {
    int init;
    int ind_count;
    int entry_count;
    int exit_count;
    
    struct timeval *timeArray;
    int rows, columns;
    char* instrument;    
    char** headers;
    double** points;   
    struct Indicator_* indicators;
    
    struct Condition_* entries; 
    struct Condition_* exits; 
};
struct Position_{
    int id;
    int size;
    int side;
    bool isOpen;
    
    char* instrument;
    struct timeval entry;
    struct timeval exit; 
    
    double open_price;
    double close_price;
};
struct Portfolio_{
    int init;
    int position_count;
    bool use_margin;
    
    double leverage;
    double nav; 
    double cash;
    struct Position_* positions;
    
};
struct Backtest_{
    int init;
    int sources;
    char** files; 
    struct Stack_* stacks;
    struct Portfolio_ portfolio;
   
};

void print_stack(struct Stack_ Stack);
struct Stack_ resample_stack(struct Stack_ stack, char freq[]);
void free_stack(struct Stack_ Stack);
struct Stack_ read_csv(char* file, const char* const delim, char* dt_format, char* dt_order);
int write_csv(struct Stack_ Stack, char* file);

int get_idx(struct Stack_* stack, const char* col, int indicator);
int add_indicator(struct Stack_* stack, struct Indicator_ indicator);
int add_condition(struct Stack_* stack, struct Condition_ condition, int side);

#endif