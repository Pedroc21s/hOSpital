/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#include <stdio.h>
#include <stdlib.h>

#include <configuration.h>


const char* argsFileName;
FILE* fp;

void fillDataFields(char* fileName, struct data_container* data){
    argsFileName = fileName;
    
    openArgsFile();
        fscanf(fp, "%d ", &data->max_ads);
        fscanf(fp, "%d ", &data->buffers_size);
        fscanf(fp, "%d ", &data->n_patients);
        fscanf(fp, "%d ", &data->n_receptionists);
        fscanf(fp, "%d ", &data->n_doctors);
        fscanf(fp, "%s ", data->log_filename);
        fscanf(fp, "%s ", data->statistics_filename);
        fscanf(fp, "%d ", &data->alarm_time);
    closeArgsFile();
}

void openArgsFile(){
    if((fp = fopen(argsFileName, "r")) == NULL){ 
        perror("[Configuration] - Erro ao abrir o ficheiro dos argumentos"); 
        exit(1); 
    }  
}

void closeArgsFile(){
    if(fclose(fp) == 1){
        perror("[Configuration] - Erro ao fechar o ficheiro dos argumentos"); 
        exit(2);
    }
}
