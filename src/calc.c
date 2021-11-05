#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "data.h"

double RSI(double* points, int length);
struct Indicator_ Backtest_RSI(struct Stack_ stack, int length);

double RSI(double* points, int length){
    double sumUp, sumDown, diff;    
    
    for(int i = 1; i < length; i++){
        diff = points[i] - points[i-1];
        if(diff > 0){
            sumUp = sumUp + diff;
        }
        else{
            sumDown = sumUp - diff;
        }
    }
    if(sumUp < 10e-7){return 0;};
    if(sumDown < 10e-7){return 100;};
    
    double rs = (sumUp/length)/(sumDown/length);
    return 100-(100/(1+rs));
}
struct Indicator_ Backtest_RSI(struct Stack_ stack, int lookback){
    int n = stack.rows;
    int ind_size = (n - 1);
    double* vals = calloc(ind_size,sizeof(double));
    struct Indicator_ indicator;
    int offset = 0;
    
    while(lookback+offset < n-2){
        double* slice = malloc(lookback*sizeof(double));
        for(int i = 0; i <= lookback; i++){
            slice[i] = stack.points[i + offset][3];
            //printf("%f\n",stack.points[i + offset][3]);
        }
        double RSI_val = RSI(slice, lookback);
        vals[offset + lookback+1] = RSI_val;
        offset++; 
        free(slice);
    }
    indicator.name = "RSI";
    indicator.init = 1;
    indicator.lookback = lookback;
    indicator.vals = vals;
    
    return indicator; 
}