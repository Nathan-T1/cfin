#ifndef _calc_h
#define _calc_h

double RSI(double* points, int length);
struct Indicator_ Backtest_RSI(struct Stack_* stack, int length, int col);

#endif