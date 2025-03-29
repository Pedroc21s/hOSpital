/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <log.h>
#include <hosptime.h>

FILE* fpLog;

void openLogFile(char* fileName){
    // abrir ficheiro para escrita
    if((fpLog = fopen(fileName, "w")) == NULL){ 
        perror("Erro ao abrir o ficheiro de Log"); 
        exit(1); 
    }
}

void closeLogFile(){
    if(fclose(fpLog) == 1){
        perror("Erro ao fechar o ficheiro de Log"); 
        exit(2);
    }
}

void writeInLog(char* operation, int argA, int argB){
    char bufferA[5];
    if(argA >= 0) sprintf(bufferA, "%d", argA); 
    else          sprintf(bufferA, " "); 
    
    char bufferB[5];
    if(argB >= 0) sprintf(bufferB, "%d", argB); 
    else          sprintf(bufferB, " ");
    
    char *timestamp = getTimeStamp(getCurrentTime());

    fprintf(fpLog, "%s %s %s %s\n", timestamp, 
                                    operation, 
                                    bufferA,
                                    bufferB);

    free(timestamp);
}
