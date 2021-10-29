#ifndef _utils_h
#define _utils_h

void remove_NL_Char(char *ptr);
void free_char_array(char** arr, int size);

int file_size(FILE *fp);

struct timeval string_to_timeval(char *time);
char *timeval_to_string(struct timeval tv);
double timeval_to_double(struct timeval tv);
struct timeval double_to_tv(double tvDouble);


#endif