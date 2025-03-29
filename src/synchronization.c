/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <synchronization.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

sem_t* semaphore_create(char* name, int value){

    char semaphoreName[50]; 
    sprintf(semaphoreName, "/%s%d", name, getpid());

    sem_t* sem = sem_open(semaphoreName, O_CREAT, 0xFFFFFFFF, value);

    if (sem == SEM_FAILED) {
        perror("\n[Synchronization] - Error creating a semaphore");
        exit(0);
    }

    return sem;
}

void semaphore_destroy(char* name, sem_t* semaphore){
    char semaphoreName[50]; 
    sprintf(semaphoreName, "/%s%d", name, getpid());

    if(sem_close(semaphore) == -1)
        perror("sem");

    if(sem_unlink(semaphoreName) == -1)
        perror("sem");
}

void produce_begin(struct prodcons* pc){
    semaphore_lock(pc->empty);
    semaphore_lock(pc->mutex);
}

void produce_end(struct prodcons* pc){
    semaphore_unlock(pc->mutex);
    semaphore_unlock(pc->full);
}

void consume_begin(struct prodcons* pc){
    semaphore_lock(pc->full);
    semaphore_lock(pc->mutex);
}

void consume_end(struct prodcons* pc){
    semaphore_unlock(pc->mutex);
    semaphore_unlock(pc->empty);
}

void semaphore_lock(sem_t* sem){
    if(sem_wait(sem) == -1){
        perror("sem wait");
    }
}

void semaphore_unlock(sem_t* sem){
    if(sem_post(sem) == -1){
        perror("sem post");
    }
}
