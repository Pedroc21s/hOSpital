/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#ifndef LOG_H_GUARD
#define LOG_H_GUARD

/**
 * Função que abre o ficheiro (caso exista) ou cria
 * um novo (para escrita) onde serão escritos todos
 * os comandos inseridos pelo o utilizador durante 
 * a execução do programa
*/
void openLogFile(char* fileName);

/**
 * Função que fecha esse ficheiro
*/
void closeLogFile();

/**
 * Função que escreve nesse ficheiro a operação inserida
 * pelo utilizador, bem como os argumentos passados para
 * essa operaçao (caso exista algum)
*/
void writeInLog(char* operation, int argA, int argB);

#endif
