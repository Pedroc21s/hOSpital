/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdlib.h>
#include <stdio.h>

#include <receptionist.h>
#include <hosptime.h>

int execute_receptionist(int receptionist_id, struct data_container* data, struct communication* comm, struct semaphores* sems){
    
    while(1){
        // le uma admissão do buffer entre pacientes e recepcionistas colocando-a em ad
        struct admission ad = {0, 0, 0, 'x', 0, 0, 0, {0}, {0}, {0}, {0}};
        receptionist_receive_admission(&ad, data, comm, sems);

        semaphore_lock(sems->terminate_mutex);
        if (*data->terminate) {
            semaphore_unlock(sems->terminate_mutex);
            break;
        }
        semaphore_unlock(sems->terminate_mutex);

        if (ad.id != -1) {
            receptionist_process_admission(&ad, receptionist_id, data, sems);
            receptionist_send_admission(&ad, data, comm, sems);
        } else {
            // já que não foi lida nenhuma admissão, 
            // ajustar os semáforos para o estado inicial
            semaphore_lock(sems->patient_receptionist->empty);
            semaphore_unlock(sems->patient_receptionist->full);
        }
    }

    return data->receptionist_stats[receptionist_id];
}

void receptionist_receive_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems){
    semaphore_lock(sems->terminate_mutex);
    if (*data->terminate) {
        semaphore_unlock(sems->terminate_mutex);
        return;
    }
    semaphore_unlock(sems->terminate_mutex);

    consume_begin(sems->patient_receptionist);
    read_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    consume_end(sems->patient_receptionist);
}

void receptionist_process_admission(struct admission* ad, int receptionist_id, struct data_container* data, struct semaphores* sems){
    semaphore_lock(sems->receptionist_stats_mutex);
    semaphore_lock(sems->results_mutex);

    ad->receiving_receptionist = receptionist_id;
    ad->status = 'R';
    data->receptionist_stats[receptionist_id] = data->receptionist_stats[receptionist_id] + 1;
    mark_receptionist_time(ad);
    data->results[ad->id] = *ad;

    printf("[Recepcionist %d] - Recebi a admissão %d \n", receptionist_id, ad->id);

    semaphore_unlock(sems->results_mutex);
    semaphore_unlock(sems->receptionist_stats_mutex);
}

void receptionist_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems){
    produce_begin(sems->receptionist_doctor);
    write_receptionist_doctor_buffer(comm->receptionist_doctor, data->buffers_size, ad);
    produce_end(sems->receptionist_doctor);
}
