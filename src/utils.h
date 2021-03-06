#ifndef _utils_h
#define _utils_h

struct Backtest_ def_parser(char* file);

void remove_NL_Char(char *ptr);
void free_char_array(char** arr, int size);
void append(char* s, char c);

int file_size(FILE *fp);

struct timeval string_to_timeval(char *time, char* format, char* dt_order);
char *timeval_to_string(struct timeval tv);
double timeval_to_double(struct timeval tv);
struct timeval double_to_tv(double tvDouble);

bool gt(double a, double b);
bool lt(double a, double b);
bool lte(double a, double b);
bool gte(double a, double b);
#endif