/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdlib.h>
#include <stdio.h>

#include <process.h>
#include <wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <hospsignal.h>
#include <signal.h>
#include <patient.h>
#include <receptionist.h>
#include <doctor.h>


int launch_patient(int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems){

    start_capture(data, comm, sems);
    pid_t pid = fork();

    if(pid == -1){
        perror("[Process] - Um erro ocorreu a executar o launch_patient!");
        exit(1);
    }

    if(pid == 0){ // processo filho
        signal(SIGINT, SIG_IGN);
        int result = execute_patient(patient_id, data, comm, sems);
        exit(result);
    } else {      // processo pai
        return pid;
    }
}

int launch_receptionist(int receptionist_id, struct data_container* data, struct communication* comm, struct semaphores* sems){

    start_capture(data, comm, sems);
    pid_t pid = fork();

    if(pid == -1){
        perror("[Process] - Um erro ocorreu a executar o launch_receptionist!");
        exit(1);
    }

    if(pid == 0){ // processo filho
        signal(SIGINT, SIG_IGN);
        int result = execute_receptionist(receptionist_id, data, comm, sems);
        exit(result);
    } else {      // processo pai
        return pid;
    }
}

int launch_doctor(int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems){

    start_capture(data, comm, sems);
    pid_t pid = fork();

    if(pid == -1){
        perror("[Process] - Um erro ocorreu a executar o launch_doctor!");
        exit(1);
    }

    if(pid == 0){ // processo filho
        signal(SIGINT, SIG_IGN);
        int result = execute_doctor(doctor_id, data, comm, sems);
        exit(result);
    } else {      // processo pai
        return pid;
    }
}

int wait_process(int process_id){
    int result;
    waitpid(process_id, &result, 0);

    if(WIFEXITED(result)) {
        return WEXITSTATUS(result);
    } 
    else{
        perror("[Proccess] - Um erro ocorreu a terminar o programa!");
        exit(1);
    }
}
