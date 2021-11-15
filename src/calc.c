#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "data.h"
#include "macro.h"



double MS(double* points, int length){
    double MA;
    double sum = 0;
    if(length == 0){
        fprintf(stderr, "Error: Invalid Moving Average Length");
        return MA;
    }    
    for(int i = 0; i < length; i++){
        sum = sum + points[i];
    }
    MA = sum / length;
    return MA;
}

static double RSI(double* points, int length){
    double sumUp = 0, sumDown = 0, diff = 0;  
    
    for(int i = 1; i < length+1; i++){
        diff = points[i] - points[i-1];
        if(diff > 0){
            sumUp = sumUp + diff;
        }
        else{
            sumDown = sumDown - diff;
        }
    }
    if(sumUp < 10e-8){return 0;};
    if(sumDown < 10e-8){return 100;};

    double rs = (sumUp/length)/(sumDown/length);
    return 100-(100/(1+rs));
}
struct Indicator_ Backtest_RSI(struct Stack_* stack, int lookback, int col){
    int n = stack->rows;
    int ind_size = (n - 1);
    double* vals = calloc(ind_size,sizeof(double));
    struct Indicator_ indicator;
    int offset = 0;
    
    while(lookback+offset < n-2){
        double* slice = malloc((lookback+1)*sizeof(double));
        for(int i = 0; i <= lookback; i++){
            slice[i] = stack->points[i + offset][col];
        }
       
        double RSI_val = RSI(slice, lookback);
        vals[offset + lookback] = RSI_val;
        offset++; 
        free(slice);
    }
    indicator.name = "RSI";
    indicator.init = 1;
    indicator.lookback = lookback;
    indicator.vals = vals;
    
    return indicator; 
}