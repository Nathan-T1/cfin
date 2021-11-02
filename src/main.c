#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "data.h"


int main() {

    
	char* path = "../data/test.csv";
    char freq[3] = "S5";
    const char* const s = ",";
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    struct Stack_ stack;
    stack = read_csv(path, s);
    if(stack.init != 1){
         fprintf(stderr, "Failed to initalize stack\n");
         return 0;
    }
    end = clock();
    
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);
    
    //print_stack(stack);
    //test();
    
    stack = resample_stack(stack, freq);
    if(stack.init == 1){
        print_stack(stack);
    }
    
    /*
    char new_time[20];
    struct timeval tv;
    tv.tv_sec = 1629571806;
    tv.tv_usec = 0;
    strcpy(new_time,timeval_to_string(tv));
    printf("%s \n", new_time);
    */
    
    //free_stack(stack);
    return 0;
}