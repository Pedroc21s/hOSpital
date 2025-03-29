/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include <main.h>

/**
 * Função que abre o ficheiro (para leitura) que possui 
 * os argumentos para a execução do programa
*/
void openArgsFile();

/**
 * Função que fecha esse ficheiro
*/
void closeArgsFile();

/**
 * Função que vai ler o ficheiro e preencher 
 * os campos correspondentes no data_container
*/
void fillDataFields(char* fileName, struct data_container* data);

#endif
