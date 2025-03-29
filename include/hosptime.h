#ifndef HOSPTIME_H_GUARD
#define HOSPTIME_H_GUARD

#include <time.h>
#include <sys/time.h>
#include <memory.h>

/**
 * Função que retorna uma struct timespec do tempo atual
*/
struct timespec getCurrentTime();

/**
 * Função que recebe uma struct timespec e devolve a struct tm associada a ela
*/
struct tm *getLocalTime(struct timespec ts);

/**
 * Função que recebe uma struct timespec e devolve um ponteiro para
 * a string que representa o timestamp numa formatação específica
*/
char *getTimeStamp(struct timespec ts);

/**
 * As funções seguintes servem para preencher os campos das admissões
 * com as structs timespec que correspondem ao momento em que a função
 * foi chamada 
*/
void mark_creation_time(struct admission* ad);
void mark_patient_time(struct admission* ad);
void mark_receptionist_time(struct admission* ad);
void mark_doctor_time(struct admission* ad);

/**
 * As funções seguintes servem para a obtenção de certos dados da 
 * struct tm e auxiliar a implementaçao da função getTimeStamp
*/
int getDay(struct tm *time);
int getMonth(struct tm *time);
int getYear(struct tm *time);
int getHour(struct tm *time);
int getMinute(struct tm *time);
int getSecond(struct tm *time);
int getMilliSecond(struct timespec ts);

#endif
