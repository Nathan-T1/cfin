#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "data.h"

#define END_DT 19
#define USEC_ERROR .000001

struct timeval double_to_tv(double tvDouble){
    int tv_sec = (int) tvDouble;
    double tv_usecD = tvDouble - tv_sec;
    
    struct timeval tv;
    tv.tv_sec = tv_sec;
    if(tv_usecD > USEC_ERROR){
        int tv_usec = (int) tv_usecD * 1000000;
        tv.tv_usec = tv_usec;
    }
    else{
        tv.tv_usec = 0;
    }
    return tv;
}

double timeval_to_double(struct timeval tv){
    double ret; 
    int tv_sec = tv.tv_sec;
    int tv_usec = tv.tv_usec;
    
    float usec = (float) tv_usec / 1000000;
    ret = (double) tv_sec + usec;
    
    return ret;
}
char *timeval_to_string(struct timeval tv){
    char timeString[30] = {};
    char *time;
    if(!(time = malloc(30*sizeof(char)))){
        return NULL;
    }
    time_t tv_sec = tv.tv_sec;
    int tv_usec = tv.tv_usec;
    char tv_usecStr[10];
    
    if(tv_sec == 0){
        fprintf(stderr, "Error: Invalid timeval pased to timeval_to_string\n");
        return NULL;
    }
    
    struct tm ts;
    ts = *localtime(&tv_sec);
    strftime(timeString, sizeof(timeString),"%Y-%m-%dT%H:%M:%S", &ts);
    
    
     if(tv_usec != 0){
        float usec = (float) tv_usec / 1000000;

        int len = snprintf(NULL, 0, "%f", usec);
        char *result = (char *)malloc(len + 1);
        snprintf(result, len + 1, "%f", usec);
        result++;
        strcat(timeString, result);
        free(result);
    }
    
    strcpy(time,timeString);
    return time;
}

struct timeval string_to_timeval(char *time, char* format, char* dt_order){
    struct timeval tv;
    char *tv_sec = malloc(END_DT*sizeof(char));
    char *tv_usec = malloc(sizeof(char));
    tv_sec[END_DT] = '\0';
    strncpy(tv_sec, time, END_DT);
    
    time_t result = 0;
    int year, month, day, hour, min, sec;
    if(strcmp(dt_order, "dmy") == 0){
        sscanf(tv_sec, format, &day,&month,&year,&hour,&min,&sec);
    }
    else if(strcmp(dt_order, "ymd") == 0){
        sscanf(tv_sec, format, &year,&month,&day,&hour,&min,&sec);
    }
   
    struct tm tm_mid = {0};
    tm_mid.tm_year = year - 1900;
    tm_mid.tm_mon = month - 1;
    tm_mid.tm_mday = day;
    tm_mid.tm_hour = hour - 1;
    tm_mid.tm_min = min;
    tm_mid.tm_sec = sec;
    if((result = mktime(&tm_mid)) == (time_t)-1){
        fprintf(stderr, "Failed to convert datetime to time_t \n");
        goto fail;
    }
    
    tv.tv_sec = result;
    int idx = 0;
    while(time[END_DT + idx] != '\0'){
        idx++;
    }
    if(!idx){
        free(tv_sec);
        return tv;
    }

    tv_usec = realloc(tv_usec,(idx)*sizeof(char));
    tv_usec[idx] = '\0';
    strncpy(tv_usec, time+END_DT, idx);
    
    double usec = atof(tv_usec) * 1000000;    
    tv.tv_usec = usec;

    if(tv.tv_sec != 0){
        free(tv_sec);
        free(tv_usec);
        return tv;
    }
    else{goto fail;}
fail: 
    if( tv_sec ) free( tv_sec );
    if( tv_usec ) free( tv_usec );
    fprintf(stderr, "Function failed: string_to_timeval\n");
    return tv;
}
void append(char* s, char c){
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}
void remove_NL_Char(char *ptr){
    while((ptr != NULL) && (*ptr != '\n')){
        ++ptr;
    }
    *ptr = '\0';
}
void free_char_array(char** arr, int size){
    for(size_t i = 0; i < size; i++){
        free(arr[i]);
    }
    free(arr);
}
int file_size(FILE *fp){
    int start = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp,start, SEEK_SET);
    return size;
}


