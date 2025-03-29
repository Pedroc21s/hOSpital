/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <hospsignal.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

struct data_container* main_data;
struct communication* communication_buffers;
struct semaphores* semaphores;

void start_capture(struct data_container* data, struct communication* comm, struct semaphores* sems) {

    main_data = data;
    communication_buffers = comm;
    semaphores = sems;

    signal(SIGINT, terminate_program);
}

void terminate_program() {
    end_execution(main_data, communication_buffers, semaphores);
}

void set_alarm(int time) {
    struct itimerval interval;
    signal(SIGALRM, print_info);
    interval.it_interval.tv_sec = time;
    interval.it_interval.tv_usec = 0;
    interval.it_value.tv_sec = time;
    interval.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &interval, 0); 
}

void print_default(struct admission ad){
    printf("ad:%d status:%c start_time:%ld ", ad.id, ad.status, ad.create_time.tv_sec);
}
void print_for_patient(struct admission ad){
    printf("patient:%d patient_time:%ld ", ad.receiving_patient, ad.patient_time.tv_sec);
}
void print_for_receptionist(struct admission ad){
    printf("receptionist:%d\nreceptionist_time:%ld ", ad.receiving_receptionist, ad.receptionist_time.tv_sec);
}
void print_for_doctor(struct admission ad){
    printf("doctor:%d doctor_time:%ld\n", ad.receiving_doctor, ad.doctor_time.tv_sec);
}

void print_admission_status(struct admission ad) {
    print_default(ad);

    switch (ad.status){
        case 'M': break;

        case 'P': print_for_patient(ad); 
                  break;

        case 'R': print_for_patient(ad); 
                  print_for_receptionist(ad); 
                  break;
                      
        default:  print_for_patient(ad); 
                  print_for_receptionist(ad); 
                  print_for_doctor(ad); 
                  break;
    }
}

void print_info() {
    printf("\n");
    for (int i = 0; i < main_data->max_ads; i++) {
        if ((main_data->results[i].status != '\0') && (main_data->results[i].status != 'x')) { 
            print_admission_status(main_data->results[i]);
        }
    }
    signal(SIGALRM, print_info);
}