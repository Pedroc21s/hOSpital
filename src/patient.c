/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdlib.h>
#include <stdio.h>

#include <patient.h>
#include <hosptime.h>
 
int execute_patient(int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems){

    while (1) {

        // lê uma admissão do buffer da main fazendo com que ad passe a ser essa admissão lida
        struct admission ad = {0, 0, 0, 'x', 0, 0, 0, {0}, {0}, {0}, {0}};
        patient_receive_admission(&ad, patient_id, data, comm, sems);

        semaphore_lock(sems->terminate_mutex);
        if (*data->terminate) {
            semaphore_unlock(sems->terminate_mutex);
            break;
        }
        semaphore_unlock(sems->terminate_mutex);
        
        if (ad.id != -1) {
            patient_process_admission(&ad, patient_id, data, sems);
            patient_send_admission(&ad, data, comm, sems);
        } else {
            // já que não foi lida nenhuma admissão, 
            // ajustar os semáforos para o estado inicial
            semaphore_lock(sems->main_patient->empty);
            semaphore_unlock(sems->main_patient->full);
        }
    } 

    return data->patient_stats[patient_id];
}

void patient_receive_admission(struct admission* ad, int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems){
    semaphore_lock(sems->terminate_mutex);
    if (*data->terminate) {
        semaphore_unlock(sems->terminate_mutex);
        return;
    }
    semaphore_unlock(sems->terminate_mutex);

    consume_begin(sems->main_patient);
    read_main_patient_buffer(comm->main_patient, patient_id, data->buffers_size, ad);
    consume_end(sems->main_patient);
}

void patient_process_admission(struct admission* ad, int patient_id, struct data_container* data, struct semaphores* sems){
    semaphore_lock(sems->patient_stats_mutex);
    semaphore_lock(sems->results_mutex);

    ad->receiving_patient = patient_id;
    ad->status = 'P';
    data->patient_stats[patient_id] = data->patient_stats[patient_id] + 1;
    mark_patient_time(ad);
    data->results[ad->id] = *ad;    

    printf("[Patient %d] - Recebi a admissão %d \n", patient_id, ad->id);
    
    semaphore_unlock(sems->results_mutex);
    semaphore_unlock(sems->patient_stats_mutex);
}

void patient_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems){
    produce_begin(sems->patient_receptionist);
    write_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    produce_end(sems->patient_receptionist);
}
