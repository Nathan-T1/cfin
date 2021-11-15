#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "data.h"
#include "utils.h"
#include "calc.h"
#include "macro.h"

void test(){
    
    char* path = "../data/EURUSD_Candlestick_1_M_BID_24.10.2021-29.10.2021.csv";
    char* out_path = "../data/EURUSD_Candlestick_1_M_BID_24.10.2021-29.10.2021_out.csv";

    char* dt_format = "%d.%d.%d %d:%d:%d";
    char* dt_order = "dmy";
    char freq[3] = "S5";
    const char* const s = ",";
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    struct Stack_ stack;
    stack = read_csv(path, s, dt_format, dt_order);
    print_stack(stack);
    
    struct Indicator_ ind = Backtest_RSI(&stack, 14, 3);
    add_indicator(&stack,ind);
    printf("ind_count: %i\n",stack.ind_count);
    print_stack(stack);
    //const char* col = "Close";
    //int idx = get_idx(stack, col);
    //printf("%i\n",idx);
    
    free_stack(stack);
}
void testP(){
    int a = def_parser("DEF.txt");
}
void test2(){
    const double a = 1.2;
    const double c = 1.2;
    
    bool(*comp)(double,double) = &gte;

    bool ret = (*comp)(a,c);
    if(ret){
        printf("Greater than");
    }
    else{
        printf("Less Than");
    }
    
}

int main() {
    clock_t start, end;
    start = clock();
    testP();
    end = clock();
    double cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("\n Read csv took %f seconds to execute \n", cpu_time_used);

    return 0;
}