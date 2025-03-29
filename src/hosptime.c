/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <hosptime.h>

struct timespec getCurrentTime(){
    struct timespec t;
    if(clock_gettime(CLOCK_REALTIME, &t) == -1){
        perror("Erro ao obter o tempo atual\n");
        exit(1);
    }
    return t;
}

struct tm *getLocalTime(struct timespec ts){
    time_t t = ts.tv_sec;
    return localtime(&t);
}

char *getTimeStamp(struct timespec ts){
    char *timestamp = (char *)malloc(50 * sizeof(char));
    if(timestamp == NULL){
        perror("Erro ao alocar memória para o timestamp\n");
        exit(2);
    }
    
    struct tm *time = getLocalTime(ts);

    sprintf(timestamp, "%02d/%02d/%d_%02d:%02d:%02d.%03d", getDay(time),
                                                           getMonth(time),
                                                           getYear(time),
                                                           getHour(time),
                                                           getMinute(time),
                                                           getSecond(time),
                                                           getMilliSecond(ts));
    return timestamp;
}

void mark_creation_time(struct admission* ad)     { ad->create_time       = getCurrentTime(); }
void mark_patient_time(struct admission* ad)      { ad->patient_time      = getCurrentTime(); }
void mark_receptionist_time(struct admission* ad) { ad->receptionist_time = getCurrentTime(); }
void mark_doctor_time(struct admission* ad)       { ad->doctor_time       = getCurrentTime(); }

int getDay(struct tm *time)            { return time->tm_mday;        }
int getMonth(struct tm *time)          { return time->tm_mon + 1;     }
int getYear(struct tm *time)           { return time->tm_year + 1900; }
int getHour(struct tm *time)           { return time->tm_hour;        }
int getMinute(struct tm *time)         { return time->tm_min;         }
int getSecond(struct tm *time)         { return time->tm_sec;         }
int getMilliSecond(struct timespec ts) { return ts.tv_nsec / 1000000; }
