#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "data.h"


int main() {
    

    //char a[20];
    //strcpy(a,test());
    //printf("%s \n", a);
    
    //printf(a);
    
    char *time_string = "2021-08-21T14:45:40.105";
    printf("%s, before timeval_to_string, %p \n",time_string, &time_string);
    struct timeval tm;
    tm = string_to_timeval(time_string);
     
    char new_time[20];
    strcpy(new_time,timeval_to_string(tm)); 
    printf("%s, after timeval_to_string, %p \n",new_time, &new_time);
    
    /*
	char* path = "../data/test.csv";
    const char* const s = ",";
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    struct Stack_ stack;
    stack = read_csv(path, s);
    end = clock();
    
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    //printf("\n Read csv took %f seconds to execute \n", cpu_time_used);

    */
    return 0;
}