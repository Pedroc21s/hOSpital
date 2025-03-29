/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdlib.h>
#include <stdio.h>

#include <doctor.h>
#include <hosptime.h>

int execute_doctor(int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems){

    while (1) {
        // le uma admissão do buffer entre recepcionistas e medicos colocando-a em ad
        struct admission ad = {0, 0, 0, 'x', 0, 0, 0, {0}, {0}, {0}, {0}};
        doctor_receive_admission(&ad, doctor_id, data, comm, sems);

        semaphore_lock(sems->terminate_mutex);
        if (*data->terminate) {
            semaphore_unlock(sems->terminate_mutex);
            break;
        }
        semaphore_unlock(sems->terminate_mutex);

        if (ad.id != -1) {
            doctor_process_admission(&ad, doctor_id, data, sems);
        } else {
            // já que não foi lida nenhuma admissão, 
            // ajustar os semáforos para o estado inicial
            semaphore_lock(sems->receptionist_doctor->empty);
            semaphore_unlock(sems->receptionist_doctor->full);
        }
    }

    return data->doctor_stats[doctor_id];

}

void doctor_receive_admission(struct admission* ad, int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems){
    
    semaphore_lock(sems->terminate_mutex);
    if (*data->terminate) {
        semaphore_unlock(sems->terminate_mutex);
        return;
    }
    semaphore_unlock(sems->terminate_mutex);

    consume_begin(sems->receptionist_doctor);
    read_receptionist_doctor_buffer(comm->receptionist_doctor, doctor_id, data->buffers_size, ad);
    consume_end(sems->receptionist_doctor);

}

void doctor_process_admission(struct admission* ad, int doctor_id, struct data_container* data, struct semaphores* sems){
    semaphore_lock(sems->doctor_stats_mutex);
    semaphore_lock(sems->results_mutex);

    ad->receiving_doctor = doctor_id;
    // caso o número de admissões processadas do hospital não tenha sido atingida o médico vai realizar a consulta
    if (ad->id < data->max_ads) {
        ad->status = 'A';
        data->doctor_stats[doctor_id] = data->doctor_stats[doctor_id] + 1; 
        printf("[Doctor %d] - Atendi o paciente da admissão %d\n", doctor_id, ad->id);
    }
    else { // caso contrário vai adiá-la
        ad->status = 'N';
        printf("[Doctor %d] - Agendei a consulta do paciente da admissão %d\n", doctor_id, ad->id);
    }
    mark_doctor_time(ad);
    data->results[ad->id] = *ad;

    semaphore_unlock(sems->results_mutex);
    semaphore_unlock(sems->doctor_stats_mutex);
}
