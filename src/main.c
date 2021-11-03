#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "data.h"



int main() {

    
	char* path = "../data/EURUSD_Candlestick_1_M_BID_24.10.2021-29.10.2021.csv";
    //char* dt_format = "%d-%d-%dT%d:%d:%d";
    //char* dt_order = "ymd";
    char* dt_format = "%d.%d.%d %d:%d:%d";
    char* dt_order = "dmy";
    char freq[3] = "S5";
    const char* const s = ",";
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    struct Stack_ stack;
    stack = read_csv(path, s, dt_format, dt_order);
    if(stack.init != 1){
         fprintf(stderr, "Failed to initalize stack\n");
         return 0;
    }
    end = clock();
    
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);
    
    print_stack(stack);
    Backtest_RSI(stack, 14);

    //test();
    
    //stack = resample_stack(stack, freq);
    //if(stack.init == 1){
    //    print_stack(stack);
    //}
    //char* test = "../data/test_write.csv";
    //write_csv(stack,test);

    //free_stack(stack);
    return 0;
}