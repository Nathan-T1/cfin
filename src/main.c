#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>

#include "data.h"
#include "utils.h"
#include "calc.h"
#include "macro.h"

int src_count;
struct Portfolio_* portfolio;

void handle_beat(double* slice, struct Indicator_* indicators, struct Stack_* stack){
    if(portfolio->position_count > 0){
        int b = 0;
    }
    else if(portfolio->position_count == 0){
        int a = 0;
    }
}

int init_backtest(struct Backtest_* backtest){
    if(backtest->init != 1){
        fprintf(stderr, "Error: Backtest not initalized properly");
        return 0;
    }
    src_count = backtest->sources;
    int row_count = backtest->stacks[0].rows;
    int col_count = backtest->stacks[0].columns;
    portfolio = &backtest->portfolio;
    
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < src_count; j++){
            struct Stack_* stack = &backtest->stacks[j];
            double* slice = stack->points[i]; 
            struct Indicator_* indicators = stack->indicators;
            
            handle_beat(slice, indicators, stack);
        }
    }
  
    printf("%i\n",col_count);
    printf("Init trade completed\n");
    return 1;
}

void testC(){
    struct Backtest_ backtest = def_parser("DEF.txt");
    int success = init_backtest(&backtest);
    print_stack(backtest.stacks[0]);
}

int main() {
    clock_t start, end;
    start = clock();
    testC();
    end = clock();
    double cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);

    return 0;
}