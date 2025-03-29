/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <main.h>
#include <patient.h>
#include <receptionist.h>
#include <doctor.h>
#include <process.h>
#include <configuration.h>
#include <hospsignal.h>
#include <hosptime.h>
#include <log.h>
#include <stats.h>

// Variável global para contar o número de admissões criadas.
int adCounter = 0;

// Variáveis globais utilizadas na interação com o utilizador
char command[10], userInput[20];
int argA, argB;

void create_semaphores(struct data_container* data, struct semaphores* sems){

    sems->main_patient->full  = semaphore_create(STR_SEM_MAIN_PATIENT_FULL, 0);
    sems->main_patient->empty = semaphore_create(STR_SEM_MAIN_PATIENT_EMPTY, data->buffers_size);
    sems->main_patient->mutex = semaphore_create(STR_SEM_MAIN_PATIENT_MUTEX, 1);

    sems->patient_receptionist->full  = semaphore_create(STR_SEM_PATIENT_RECEPT_FULL, 0);
    sems->patient_receptionist->empty = semaphore_create(STR_SEM_PATIENT_RECEPT_EMPTY, data->buffers_size);
    sems->patient_receptionist->mutex = semaphore_create(STR_SEM_PATIENT_RECEPT_MUTEX, 1);

    sems->receptionist_doctor->full  = semaphore_create(STR_SEM_RECEPT_DOCTOR_FULL, 0);
    sems->receptionist_doctor->empty = semaphore_create(STR_SEM_RECEPT_DOCTOR_EMPTY, data->buffers_size);
    sems->receptionist_doctor->mutex = semaphore_create(STR_SEM_RECEPT_DOCTOR_MUTEX, 1);

    sems->patient_stats_mutex      = semaphore_create(STR_SEM_PATIENT_STATS_MUTEX, 1);
    sems->receptionist_stats_mutex = semaphore_create(STR_SEM_RECEPT_STATS_MUTEX, 1);
    sems->doctor_stats_mutex       = semaphore_create(STR_SEM_DOCTOR_STATS_MUTEX, 1);

    sems->results_mutex   = semaphore_create(STR_SEM_RESULTS_MUTEX,1);
    sems->terminate_mutex = semaphore_create(STR_SEM_TERMINATE_MUTEX,1);

}

void wakeup_processes(struct data_container* data, struct semaphores* sems){

    for (int i = 0; i < data->n_patients; i++){
        produce_end(sems->main_patient);
    }
    
    for (int i = 0; i < data->n_receptionists; i++){
        produce_end(sems->patient_receptionist);
    }

    for (int i = 0; i < (data->n_doctors + data->n_receptionists); i++){
        produce_end(sems->receptionist_doctor);
    }
}

void destroy_semaphores(struct semaphores* sems){

    semaphore_destroy(STR_SEM_MAIN_PATIENT_FULL,  sems->main_patient->full);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_EMPTY, sems->main_patient->empty);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_MUTEX, sems->main_patient->mutex);

    semaphore_destroy(STR_SEM_PATIENT_RECEPT_FULL,  sems->patient_receptionist->full);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_EMPTY, sems->patient_receptionist->empty);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_MUTEX, sems->patient_receptionist->mutex);

    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_FULL,  sems->receptionist_doctor->full);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_EMPTY, sems->receptionist_doctor->empty);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_MUTEX, sems->receptionist_doctor->mutex);

    semaphore_destroy(STR_SEM_PATIENT_STATS_MUTEX, sems->patient_stats_mutex);
    semaphore_destroy(STR_SEM_RECEPT_STATS_MUTEX,  sems->receptionist_stats_mutex);
    semaphore_destroy(STR_SEM_DOCTOR_STATS_MUTEX,  sems->doctor_stats_mutex);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX,   sems->results_mutex);
    semaphore_destroy(STR_SEM_TERMINATE_MUTEX, sems->terminate_mutex);

}

void main_args(int argc, char* argv[], struct data_container* data){
    char* argsFile = argv[1];
    fillDataFields(argsFile, data);
    openLogFile(data->log_filename);
}

void allocate_dynamic_memory_buffers(struct data_container* data){
    int intSize = sizeof(int);
    data->patient_pids       = allocate_dynamic_memory(data->n_patients * intSize);
    data->receptionist_pids  = allocate_dynamic_memory(data->n_receptionists * intSize);
    data->doctor_pids        = allocate_dynamic_memory(data->n_doctors * intSize);
}

void create_shared_memory_buffers(struct data_container* data, struct communication* comm){
    int fullBufferSize =     data->buffers_size * sizeof(struct admission);
    int fullBufferSizeInts = data->buffers_size * sizeof(int);
    int pointersSize = sizeof(struct pointers);
    int patStatsSize = sizeof(int) * data->n_patients;
    int recStatsSize = sizeof(int) * data->n_receptionists;
    int docStatsSize = sizeof(int) * data->n_doctors;
    int resultsMaxSize = MAX_RESULTS * sizeof(struct admission);

    comm->main_patient->ptrs   = create_shared_memory(STR_SHM_MAIN_PATIENT_PTR,    pointersSize);
    comm->main_patient->buffer = create_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, fullBufferSize);
    
    comm->patient_receptionist->ptrs   = create_shared_memory(STR_SHM_PATIENT_RECEPT_PTR,    fullBufferSizeInts);
    comm->patient_receptionist->buffer = create_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, fullBufferSize);
    
    comm->receptionist_doctor->ptrs   = create_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR,    pointersSize);
    comm->receptionist_doctor->buffer = create_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, fullBufferSize);

    data->patient_stats      = create_shared_memory(STR_SHM_PATIENT_STATS, patStatsSize);
    data->receptionist_stats = create_shared_memory(STR_SHM_RECEPT_STATS,  recStatsSize);
    data->doctor_stats       = create_shared_memory(STR_SHM_DOCTOR_STATS,  docStatsSize);

    data->results            = create_shared_memory(STR_SHM_RESULTS,       resultsMaxSize);
    data->terminate          = create_shared_memory(STR_SHM_TERMINATE,     sizeof(int));
}

void launch_processes(struct data_container* data, struct communication* comm, struct semaphores* sems){
    for(int i = 0; i < data->n_patients; i++)
        data->patient_pids[i] = launch_patient(i, data, comm, sems);

    for(int i = 0; i < data->n_receptionists; i++)
        data->receptionist_pids[i] = launch_receptionist(i, data, comm, sems);

    for(int i = 0; i < data->n_doctors; i++)
        data->doctor_pids[i] = launch_doctor(i, data, comm, sems);
}

/*
* Função Auxiliar para imprimir na consola informação relativa às ações disponíveis que o
* utilizador pode requisitar em contexto de execução.
*/

void printOptions(){
    printf("\n[Main] - Ações Disponíveis:\n");
    printf("[Main] - ad paciente medico: se deseja fazer uma admissao no hospital;\n");
    printf("[Main] - info id : se deseja consultar o estado de uma admissao;\n");
    printf("[Main] - status : se deseja consultar o estado atual de data_container;\n");
    printf("[Main] - help : se deseja mais informação sobre as ações disponíveis;\n");
    printf("[Main] - end : se deseja terminar a execucao do sistema;\n");
}

void user_interaction(struct data_container* data, struct communication* comm, struct semaphores* sems){

    printOptions();
    printf("\n[Main] - Introduzir ação: ");
    set_alarm(data->alarm_time);

    do{
        fgets(userInput, sizeof(userInput), stdin);
        // formatar o input do utilizador consoante os campos em baixo
        int numArgs = sscanf(userInput,"%s %d %d", command, &argA, &argB); 

        if (numArgs == 3 && !strcmp(command, "ad")) {
            writeInLog("ad", argA, argB);
            create_request(&adCounter, data, comm, sems);
        }
        else if (numArgs == 2 && !strcmp(command,"info")) {
            writeInLog("info", argA, -1);
            
            if (argA >= 0 && argA < adCounter) {
                read_info(data, sems); 
            }
            else {
                printf("[Main] - A admissão indicada não existe!\n");
            }
        }
        else if (numArgs == 1 && !strcmp(command, "help")) { 
            writeInLog("help", -1, -1);
            printOptions();
        } 
        else if (numArgs == 1 && !strcmp(command, "end")) { 
            writeInLog("end", -1, -1);
            break; 
        } 
        else if (numArgs == 1 && !strcmp(command, "status")) { 
            writeInLog("status", -1, -1);
            print_status(data, sems); 
        }
        else { 
            printf("\n[Main] - Comando Inválido, digite 'help' para visualizar as ações disponíveis!");
        }
        usleep(100000);
        printf("\n[Main] - Introduzir ação: ");
    } while(1);
    end_execution(data, comm, sems);
}

void create_request(int* ad_counter, struct data_container* data, struct communication* comm, struct semaphores* sems){

    int admissionId = *ad_counter;
    *ad_counter = *ad_counter + 1;
    
    struct admission ad = {admissionId, argA, argB, 'M', -1, -1, -1, {0}, {0}, {0}, {0}};
    mark_creation_time(&ad);

    produce_begin(sems->main_patient);
    semaphore_lock(sems->results_mutex);
    write_main_patient_buffer(comm->main_patient, data->buffers_size, &ad);
    data->results[ad.id] = ad;
    semaphore_unlock(sems->results_mutex);
    produce_end(sems->main_patient);

    printf("\n[Main] - A admissão %d foi criada!\n", admissionId);
}

void read_info(struct data_container* data, struct semaphores* sems){
    char adStatus     = data->results[argA].status;
    int requestingPat = data->results[argA].requesting_patient;
    int resquestedDoc = data->results[argA].requested_doctor;
    int receivingPat  = data->results[argA].receiving_patient;
    int receivingRec  = data->results[argA].receiving_receptionist;
    int receivingDoc  = data->results[argA].receiving_doctor;
    
    printf("\n[Main] - ");

    switch (adStatus) {
        case 'M': printf("A admissão %d com estado %c criada pelo paciente %d com o médico %d requisitado, ainda não foi validada pelo paciente.\n",                                                       
                            argA, adStatus, requestingPat, resquestedDoc);                                           break;
        case 'P': printf("A admissão %d com estado %c criada pelo paciente %d com o médico %d requisitado, foi validada pelo paciente %d.\n",                                                              
                            argA, adStatus, requestingPat, resquestedDoc, receivingPat);                             break;
        case 'R': printf("A admissão %d com estado %c criada pelo paciente %d com o médico %d requisitado, foi validada pelo paciente %d e admitida pelo rececionista %d. \n",                           
                            argA, adStatus, requestingPat, resquestedDoc, receivingPat, receivingRec);               break;
        case 'A': printf("A admissão %d com estado %c criada pelo paciente %d com o médico %d requisitado, foi validada pelo paciente %d, admitida pelo rececionista %d e foi concluida pelo médico %d. \n", 
                            argA, adStatus, requestingPat, resquestedDoc, receivingPat, receivingRec, receivingDoc); break;
        case 'N': printf("A admissão %d com estado %c criada pelo paciente %d com o médico %d requisitado, foi validada pelo paciente %d, admitida pelo rececionista %d e foi adiada pelo médico %d. \n",    
                            argA, adStatus, requestingPat, resquestedDoc, receivingPat, receivingRec, receivingDoc); break;
        default:  printf("Ocorreu um erro");
    }  
}

/*
* Esta função auxiliar imprime os PIDs e Stats dos ponteiros passados como pârametros.
*/
void printStatsOrPids(int size, int *array){
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", array[i]);
        if (i+1 < size) printf(", "); 
        else            printf("]\n");
    }   
}

/*
* Esta função auxiliar imprime uma representação textual do vetor results utilizando
* exclusivamente os IDs das admissões
*/
void printAdmissionIds(int size, struct admission *results){
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", results[i].id);
        if (i+1 < size) printf(", ");
        else            printf("]\n");
    } 
}

void print_status(struct data_container* data, struct semaphores* sems){
    int nPat = data->n_patients;
    int nRec = data->n_receptionists;
    int nDoc = data->n_doctors;

    printf("[Main] max_ads: %d\n", data->max_ads);
    printf("[Main] buffers_size: %d\n", data->buffers_size);
    printf("[Main] n_patients: %d\n", nPat);
    printf("[Main] n_receptionists: %d\n", nRec);
    printf("[Main] n_doctors: %d\n", nDoc);
    printf("[Main] patient_pids: ");       printStatsOrPids(nPat, data->patient_pids);
    printf("[Main] receptionist_pids: ");  printStatsOrPids(nRec, data->receptionist_pids);
    printf("[Main] doctor_pids: ");        printStatsOrPids(nDoc, data->doctor_pids);
    printf("[Main] patient_stats: ");      printStatsOrPids(nPat, data->patient_stats);
    printf("[Main] receptionist_stats: "); printStatsOrPids(nRec, data->receptionist_stats);
    printf("[Main] doctor_stats: ");       printStatsOrPids(nDoc, data->doctor_stats);
    printf("[Main] results: ");            printAdmissionIds(data->max_ads, data->results);
    printf("[Main] terminate: %d", *data->terminate);
}

void end_execution(struct data_container* data, struct communication* comm, struct semaphores* sems){
    printf("\n[Main] - A encerrar a execução do hOSpital! \n");
    semaphore_lock(sems->terminate_mutex);
    *data->terminate = 1;
    semaphore_unlock(sems->terminate_mutex);
    wakeup_processes(data, sems);
    wait_processes(data);
    execute_stats(data, adCounter);
    destroy_semaphores(sems);
    destroy_memory_buffers(data, comm);

    //release memory before terminating
    deallocate_dynamic_memory(data);
    deallocate_dynamic_memory(comm->main_patient);
    deallocate_dynamic_memory(comm->patient_receptionist);
    deallocate_dynamic_memory(comm->receptionist_doctor);
    deallocate_dynamic_memory(comm);
    deallocate_dynamic_memory(sems->main_patient);
    deallocate_dynamic_memory(sems->patient_receptionist);
    deallocate_dynamic_memory(sems->receptionist_doctor);
    deallocate_dynamic_memory(sems);

    closeLogFile();
    exit(0);
}

void wait_processes(struct data_container* data) {
    for (int i = 0; i < data->n_patients; i++)
        data->patient_stats[i] = wait_process(data->patient_pids[i]);

    for (int i = 0; i < data->n_receptionists; i++)
        data->receptionist_stats[i] = wait_process(data->receptionist_pids[i]);

    for (int i = 0; i < data->n_doctors; i++)
        data->doctor_stats[i] = wait_process(data->doctor_pids[i]);
}

void write_statistics(struct data_container* data){
    printf("As estatísticas de execução são: \n");

    for (int i = 0; i < data->n_patients; i++)
        printf("[Main] - O paciente %d requeriu %d admissões!\n", i, data->patient_stats[i]);

    for (int i = 0; i < data->n_receptionists; i++)
        printf("[Main] - O rececionista %d realizou %d admissões!\n", i, data->receptionist_stats[i]);

    for (int i = 0; i < data->n_doctors; i++)
        printf("[Main] - O médico %d atendeu %d admissões!\n", i, data->doctor_stats[i]);
}

void destroy_memory_buffers(struct data_container* data, struct communication* comm){
    int fullBufferSize =     data->buffers_size * sizeof(struct admission);
    int fullBufferSizeInts = data->buffers_size * sizeof(int);
    int pointersSize = sizeof(struct pointers);
    int patStatsSize = sizeof(int) * data->n_patients;
    int recStatsSize = sizeof(int) * data->n_receptionists;
    int docStatsSize = sizeof(int) * data->n_doctors;
    int resultsMaxSize = MAX_RESULTS * sizeof(struct admission);

    deallocate_dynamic_memory(data->patient_pids);
    deallocate_dynamic_memory(data->receptionist_pids);
    deallocate_dynamic_memory(data->doctor_pids);

    destroy_shared_memory(STR_SHM_MAIN_PATIENT_PTR,      comm->main_patient->ptrs,           pointersSize);
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER,   comm->main_patient->buffer,         fullBufferSize);
    
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_PTR,    comm->patient_receptionist->ptrs,   fullBufferSizeInts);
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, comm->patient_receptionist->buffer, fullBufferSize);

    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR,     comm->receptionist_doctor->ptrs,    pointersSize);
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER,  comm->receptionist_doctor->buffer,  fullBufferSize);

    destroy_shared_memory(STR_SHM_PATIENT_STATS,         data->patient_stats,                patStatsSize);
    destroy_shared_memory(STR_SHM_RECEPT_STATS,          data->receptionist_stats,           recStatsSize);
    destroy_shared_memory(STR_SHM_DOCTOR_STATS,          data->doctor_stats,                 docStatsSize);

    destroy_shared_memory(STR_SHM_RESULTS,               data->results,                      resultsMaxSize);
    destroy_shared_memory(STR_SHM_TERMINATE,             data->terminate,                    sizeof(int));

}

int main(int argc, char *argv[]){
    //init data structures
    struct data_container* data = allocate_dynamic_memory(sizeof(struct data_container));
    struct communication* comm  = allocate_dynamic_memory(sizeof(struct communication));
    comm->main_patient          = allocate_dynamic_memory(sizeof(struct circular_buffer));
    comm->patient_receptionist  = allocate_dynamic_memory(sizeof(struct rnd_access_buffer));
    comm->receptionist_doctor   = allocate_dynamic_memory(sizeof(struct circular_buffer));

    // init semaphore data structure
    struct semaphores* sems     = allocate_dynamic_memory(sizeof(struct semaphores));
    sems->main_patient          = allocate_dynamic_memory(sizeof(struct prodcons));
    sems->patient_receptionist  = allocate_dynamic_memory(sizeof(struct prodcons));
    sems->receptionist_doctor   = allocate_dynamic_memory(sizeof(struct prodcons));

    //execute main code
    main_args(argc, argv, data);
    allocate_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, comm);
    create_semaphores(data, sems);
    launch_processes(data, comm, sems);
    user_interaction(data, comm, sems);
    
}