#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#include "data.h"
#include "calc.h"
#include "macro.h"

void remove_NL_Char(char* ptr);
bool isNum(const char* str);
int handle_indicator(struct Stack_* stack, char* line, const char* const delim);
int handle_condition(struct Stack_* stack, char* line, const char* const delim, int side);
struct Portfolio_ handle_portfolio(char* line, const char* const delim);

bool gt(double a, double b);
bool gte(double a, double b);
bool lt(double a, double b);
bool lte(double a, double b);

struct Backtest_ def_parser(const char* const file){
    FILE* fp = NULL;
    char* buffer = NULL;
    const char* const delim = "|"; 
    int id;
    int sdx = 0;
    
    struct Backtest_ backtest;
    struct Portfolio_ portfolio;
    if((fp = fopen(file, "r")) == NULL){
        fprintf(stderr, "Error: File does not exist");
        return backtest;
    }
    if(!(buffer = (char*)calloc(BUFFER_SIZE, sizeof(char)))){
        fprintf(stderr, "Error: Error allocating buffer");
        return backtest;
    }
    while(feof(fp) != true){
        fgets(buffer, BUFFER_SIZE, fp);
        remove_NL_Char(buffer);
        
        char* line = (char*)calloc(BUFFER_SIZE, sizeof(char));
        stpcpy(line, buffer);
        line = line + 2;
      
        char *token = strtok(buffer, delim);
        int line_success = 0;
        id = atoi(token);
        switch(id){
            case 0:
                token = strtok(NULL,delim);
                if(!strcmp(token,"BACKTEST")){
                    token = strtok(NULL,delim);
                    backtest.sources = atoi(token);
                    if(atoi(token) <= 0){
                        fprintf(stderr, "Invalid source count");
                        return backtest;
                    }
                    else{
                        backtest.files = (char**) calloc(atoi(token),sizeof(char*));
                    }
                    backtest.init = 1;
                    break;
                }
                else{
                    fprintf(stderr, "Invalid def specifier");
                    return backtest;
                }
                break;
            case 1:
                token = strtok(NULL,delim);
                if(backtest.init){
                    size_t length = strlen(token);
                    backtest.files[sdx] = (char*)calloc(length, sizeof(char));
                    strcpy(backtest.files[sdx], token);
                    
                    token = strtok(NULL,delim);
                    length = strlen(token);
                    char* instrument = (char*)calloc(length, sizeof(char));
                    strcpy(instrument,token);
                    
                    token = strtok(NULL,delim);
                    const char* const file_delim = token;
                    
                    token = strtok(NULL,delim);
                    char* dt_format = token;
                    
                    token = strtok(NULL,delim);
                    char* dt_order = token;
                    length = strlen(dt_order);
                    dt_order[length-1] = '\0';
                    
                    struct Stack_ stack;
                    stack = read_csv(backtest.files[sdx], file_delim, dt_format, dt_order);
                    stack.instrument = instrument;
                    if(stack.init != 1){
                         fprintf(stderr, "Failed to initalize stack\n");
                         return backtest;
                    }
                    if(sdx == 0){
                        struct Stack_* stacks = malloc(sizeof(struct Stack_));
                        stacks[sdx] = stack;
                        backtest.stacks = stacks;
                    }
                    else{
                        struct Stack_* tmp = realloc(backtest.stacks,(sdx+1)*sizeof(struct Stack_));
                        if(!tmp){
                            fprintf(stderr,"Failed to resize stacks");
                            return backtest;
                        }
                        backtest.stacks = tmp;
                        backtest.stacks[sdx] = stack;
                    }
                    sdx++;
                }            
                else{
                    fprintf(stderr, "def not initalized");
                    return backtest;
                }
                break;
            case 2:
                break;
                line_success = handle_indicator(&backtest.stacks[sdx-1],line,delim);
                break;
            case 3:
                line_success = handle_condition(&backtest.stacks[sdx-1],line,delim,1);
                break;
            case 4:
                line_success = handle_condition(&backtest.stacks[sdx-1],line,delim,-1);
                break;
            case 99:
                portfolio = handle_portfolio(line,delim);
                break;
 
        }
        free(line);
    }
    free(buffer);
    if(fp) fclose(fp);
    
    if(portfolio.init != 1){
        fprintf(stderr, "Error: Portolio failed to initalize");
        return backtest;
    }
    else{
        backtest.portfolio = portfolio;
    }
    backtest.init = 1;
    return backtest;
};
struct Portfolio_ handle_portfolio(char* line, const char* const delim){
    struct Portfolio_ portfolio;
    memset(&portfolio, 0, sizeof(struct Portfolio_));
    char* token = strtok(line,delim);
    
    bool isNumber = isNum(token);
    if(!isNumber){
        fprintf(stderr,"Error: Invalid starting cash");
        return portfolio;
    }
    else{
        portfolio.cash = atof(token);
    }
    token = strtok(NULL,delim);
    isNumber = isNum(token);
    if(!isNumber){
        fprintf(stderr,"Error: Invalid margin amount");
        return portfolio;
    }
    else{
        portfolio.leverage = atof(token);
        portfolio.use_margin = true;
    }
    portfolio.init = 1;
    return portfolio;
}
int handle_condition(struct Stack_* stack, char* line, const char* const delim, int side){
    char* token = strtok(line,delim);
    struct Condition_ condition; 

    bool isNumber = isNum(token);
    if(!isNumber){
        condition.col_a = token;
    }
    else{
        condition.c = atof(token);
        condition.c_pos = 1;
    }
    
    token = strtok(NULL,delim);
    bool(*comp)(const double, const double);
    if(!strcmp(token,"<")){
        comp = &lt;
    }
    else if(!strcmp(token,">")){
        comp = &gt;
    }
    else if(!strcmp(token,"<=")){
        comp = &lte;
    }
    else if(!strcmp(token,">=")){
        comp = &gte;
    }
    condition.comp = comp;
    
    token = strtok(NULL,delim);
    isNumber = isNum(token);
    if(!isNumber){
        condition.col_b = token;
    }
    else{
        condition.c = atof(token);
        condition.c_pos = 2;
    }
    int success = add_condition(stack,condition, side);
    if(!success){
        fprintf(stderr, "Error: add_condition failed");
        return 0;
    }
    return 1;
}

int handle_indicator(struct Stack_* stack, char* line, const char* const delim){
    char* token = strtok(line, delim);
    struct Indicator_ indicator;
    int ret = 0;
    while(token){
        if(!strcmp(token,"RSI")){
            token = strtok(NULL,delim);
            int is_calc = atoi(token);
            if(!is_calc){
                token = strtok(NULL,delim);
                int col = get_idx(stack, token , 0);
                int lookback = atoi(strtok(NULL,delim));
                indicator = Backtest_RSI(stack, lookback,col);
                
                ret = add_indicator(stack,indicator);
                if(ret){
                    return ret;
                }
            }
        }
        token = strtok(NULL,delim); 
    }
    return 0;
}

struct timeval double_to_tv(double tvDouble){
    int tv_sec = (int) tvDouble;
    double tv_usecD = tvDouble - tv_sec;
    
    struct timeval tv;
    tv.tv_sec = tv_sec;
    if(tv_usecD > USEC_ERROR){
        int tv_usec = (int) tv_usecD * MICRO_SECONDS;
        tv.tv_usec = tv_usec;
    }
    else{
        tv.tv_usec = 0;
    }
    return tv;
}

double timeval_to_double(struct timeval tv){
    double ret; 
    int tv_sec = tv.tv_sec;
    int tv_usec = tv.tv_usec;
    
    float usec = (float) tv_usec / MICRO_SECONDS;
    ret = (double) tv_sec + usec;
    
    return ret;
}
char *timeval_to_string(struct timeval tv){
    char timeString[30] = {};
    char *time;
    if(!(time = malloc(30*sizeof(char)))){
        return NULL;
    }
    time_t tv_sec = tv.tv_sec;
    int tv_usec = tv.tv_usec;
    char tv_usecStr[10];
    
    if(tv_sec == 0){
        fprintf(stderr, "Error: Invalid timeval pased to timeval_to_string\n");
        return NULL;
    }
    
    struct tm ts;
    ts = *localtime(&tv_sec);
    strftime(timeString, sizeof(timeString),"%Y-%m-%dT%H:%M:%S", &ts);
    
    
     if(tv_usec != 0){
        float usec = (float) tv_usec / MICRO_SECONDS;

        int len = snprintf(NULL, 0, "%f", usec);
        char *result = (char *)malloc(len + 1);
        snprintf(result, len + 1, "%f", usec);
        result++;
        strcat(timeString, result);
        free(result);
    }
    
    strcpy(time,timeString);
    return time;
}

struct timeval string_to_timeval(char *time, char* format, char* dt_order){
    struct timeval tv;
    char *tv_sec = malloc(END_DT*sizeof(char));
    char *tv_usec = malloc(sizeof(char));
    tv_sec[END_DT] = '\0';
    strncpy(tv_sec, time, END_DT);
    
    time_t result = 0;
    int year, month, day, hour, min, sec;
    if(strcmp(dt_order, "dmy") == 0){
        sscanf(tv_sec, format, &day,&month,&year,&hour,&min,&sec);
    }
    else if(strcmp(dt_order, "ymd") == 0){
        sscanf(tv_sec, format, &year,&month,&day,&hour,&min,&sec);
    }

    struct tm tm_mid = {0};
    tm_mid.tm_year = year - 1900;
    tm_mid.tm_mon = month - 1;
    tm_mid.tm_mday = day;
    tm_mid.tm_hour = hour - 1;
    tm_mid.tm_min = min;
    tm_mid.tm_sec = sec;
    if((result = mktime(&tm_mid)) == (time_t)-1){
        fprintf(stderr, "Failed to convert datetime to time_t \n");
        goto fail;
    }
    
    tv.tv_sec = result;
    int idx = 0;
    while(time[END_DT + idx] != '\0'){
        idx++;
    }
    if(!idx){
        free(tv_sec);
        return tv;
    }

    tv_usec = realloc(tv_usec,(idx)*sizeof(char));
    tv_usec[idx] = '\0';
    strncpy(tv_usec, time+END_DT, idx);
    
    double usec = atof(tv_usec) * MICRO_SECONDS;    
    tv.tv_usec = usec;

    if(tv.tv_sec != 0){
        free(tv_sec);
        free(tv_usec);
        return tv;
    }
    else{goto fail;}
fail: 
    if( tv_sec ) free( tv_sec );
    if( tv_usec ) free( tv_usec );
    fprintf(stderr, "Function failed: string_to_timeval\n");
    return tv;
}
bool isNum(const char* str){
    const int str_length = strlen(str);
    for(int i = 0; i < str_length-1; i++){
        if(!isdigit(*str) && *str){
            return false;
        }
        str++;
    }
    return true;
}
void append(char* s, char c){
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}
void remove_NL_Char(char *ptr){
    size_t ln = strlen(ptr) - 1;
    if (*ptr && ptr[ln] == '\n') 
        ptr[ln] = '\0';
}
void free_char_array(char** arr, int size){
    for(size_t i = 0; i < size; i++){
        free(arr[i]);
    }
    free(arr);
}
int file_size(FILE *fp){
    int start = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp,start, SEEK_SET);
    return size;
}
bool gt(double a, double b){
    bool ret = false; 
    if(a > b){
        ret = true;
    }
    return ret;
}
bool lt(double a, double b){
    bool ret = false; 
    if(a < b){
        ret = true;
    }
    return ret;
}
bool lte(double a, double b){
    bool ret = false; 
    if(a <= b){
        ret = true;
    }
    return ret;
}
bool gte(double a, double b){
    bool ret = false; 
    if(a >= b){
        ret = true;
    }
    return ret;
}


