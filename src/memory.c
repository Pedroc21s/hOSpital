/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdlib.h>
#include <stdio.h>

#include <memory.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void* create_shared_memory(char* name, int size){
    char memoryName[50];
    sprintf(memoryName, "/%s%d", name, getpid());

    int fd = shm_open(memoryName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("[Memory] - Erro a criar o espaço de memória partilhada!");
        exit(1);
    }

    int ret = ftruncate(fd, size);
    if (ret == -1) {
        perror("[Memory] - Erro a definir o tamanho do segmento na memoria partilhada!");
        exit(2);
    }

    void* pointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pointer == MAP_FAILED) {
        perror("[Memory] - Erro a projetar na zona de memoria partilhada!");
        exit(3);
    }

    return pointer;
}


void* allocate_dynamic_memory(int size){
    return calloc(1,size);
}

void destroy_shared_memory(char* name, void* ptr, int size){
    char memoryName[50];
    sprintf(memoryName, "/%s%d", name, getpid());

    int ret = munmap(ptr, size);
    if (ret == -1) {
        perror("[Memory] - Erro a terminar a projeção de memoria partilhada!");
        exit(1);
    }

    ret = shm_unlink(memoryName);
    if (ret == -1) {
        perror("[Memory] - Erro a eliminar a zona de memoria partilhada!");
        exit(2);
    }
}

void deallocate_dynamic_memory(void* ptr){
    free(ptr);
}

/*
* Função auxiliar para escrever no buffer circular visto o código ser comum para os 
* buffers entre main_patient e receptionist_doctor
*/
void writeInCircularBuffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad){
    
    // Se o buffer estiver cheio não vai ser possível escrever neste
    if ((buffer->ptrs->in + 1) % buffer_size == buffer->ptrs->out) {
        return;
    }

    int in = buffer->ptrs->in;
    buffer->buffer[in] = *ad;
    buffer->ptrs->in = (in + 1) % buffer_size;
}

void write_main_patient_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad){
    writeInCircularBuffer(buffer, buffer_size, ad);
}

void write_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad){

    for (int i = 0; i < buffer_size; i++) {
        if (!buffer->ptrs[i]) { // se a posição se encontrar livre vai escrever nesta ad
            buffer->buffer[i] = *ad;
            buffer->ptrs[i] = 1; 
            return;
        }
    }
}

void write_receptionist_doctor_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad){
    writeInCircularBuffer(buffer, buffer_size, ad);
}

void read_main_patient_buffer(struct circular_buffer* buffer, int patient_id, int buffer_size, struct admission* ad){

    int in = buffer->ptrs->in;
    int out = buffer->ptrs->out;

    // se o buffer não estiver cheio e a entrada indicada pelo pointer for direcionada ao paciente vai se ler a entrada
    if(in != out && buffer->buffer[out].requesting_patient == patient_id){
        *ad = buffer->buffer[out];
        buffer->ptrs->out = (out+1) % buffer_size;
    } else {
        ad->id = -1;
    }

}

void read_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad){

    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1) { // se a posição estiver ocupada lê a admissão
            *ad = buffer->buffer[i];
            buffer->ptrs[i] = 0; // depois de a ler vai deixar a posição como desocupada
            return;
        }
    }
    
    ad->id = -1;
}

void read_receptionist_doctor_buffer(struct circular_buffer* buffer, int doctor_id, int buffer_size, struct admission* ad){

    int in = buffer->ptrs->in;
    int out = buffer->ptrs->out;

    // se o buffer não estiver cheio e a entrada indicada pelo pointer for direcionada ao medico vai se ler a entrada
    if (in != out && buffer->buffer[out].requested_doctor == doctor_id) {
        *ad = buffer->buffer[out];
        buffer->ptrs->out = (out+1) % buffer_size;
    }
    else {
        ad->id = -1;
    }

}
