#ifndef _calc_h
#define _calc_h

struct Indicator_ Backtest_RSI(struct Stack_ stack, int length);
double RSI(double* points, int length);

#endif