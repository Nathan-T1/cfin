#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "data.h"

#define END_DT 19

char *timeval_to_string(struct timeval tv){
    char timeString[30] = {};
    char *time = timeString;
    
    time_t tv_sec = tv.tv_sec;
    int tv_usec = tv.tv_usec;
    char tv_usecStr[10];
    
    if(tv_sec == 0){
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
      
        strcat(timeString, result);
        free(result);
    }
    
    printf("%s, inside timeval_to_string, %p \n",time, &time);
    
    return time;
}

struct timeval string_to_timeval(char *time){
    struct timeval tv;
    char *tv_sec = malloc(END_DT*sizeof(char));
    char *tv_usec = malloc(sizeof(char));
    tv_sec[END_DT] = '\0';
    strncpy(tv_sec, time, END_DT);
    
    time_t result = 0;
    int year, month, day, hour, min, sec;

    if(sscanf(tv_sec, "%d-%d-%dT%d:%d:%d", &year,&month,&day,&hour,&min,&sec) == 6){
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
    }
    else{
        fprintf(stderr, "Failed to parse datetime to time_t \n");
        goto fail;
    }
    tv.tv_sec = result;
    int idx = 0;
    while(time[END_DT + idx] != '\0'){
        idx ++;
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
