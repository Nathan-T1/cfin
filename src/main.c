#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "data.h"
#include "utils.h"
#include "calc.h"
#include "macro.h"

int main() {
    //int a = def_parser("DEF.txt");

	char* path = "../data/EURUSD_Candlestick_1_M_BID_24.10.2021-29.10.2021.csv";
    char* out_path = "../data/EURUSD_Candlestick_1_M_BID_24.10.2021-29.10.2021_out.csv";
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
    end = clock();
    if(stack.init != 1){
         fprintf(stderr, "Failed to initalize stack\n");
         return 0;
    }
    
    
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);
    
    
    struct Indicator_* ind = malloc(sizeof(struct Indicator_));
    ind[0] = Backtest_RSI(stack, 14);
    stack.indicators = ind;
    stack.ind_count = 1;

    print_stack(stack);
    write_csv(stack, out_path);
    free_stack(stack);
    return 0;
}