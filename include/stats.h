/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include <main.h>
#include <memory.h>

/*
* Este método permite abrir o ficheiro, ou seja inicializa o fpStats através de fopen(), 
* isto assumindo que a variavel statsFileName já foi inicializada antes. 
* Retorna 0 se foi bem sucedido e 1 caso contrário.
*/
int openStatsFile();

/*
* Este método permite fechar o ficheiro usando o fclose().
* Retorna 0 se foi bem sucedido e 1 caso contrário.
*/
int closeStatsFile();

/*
* Este método imprime as estatísticas no ficheiro statsFileName.
*/
void writeStats(struct data_container* data, int totalAdmissions);

/*
* Este é um método para imprimir no statsFileName a informação relativa aos processos
* da estrutura data.
*/
void writeProcessStats(struct data_container* data);

/*
* Este é um método para imprimir o estado da admissão ad enviada como parâmetro no
* statsFileName.
*/
void writeAdmissionStats(struct admission ad);

/*
* Este é o método da classe stats.c que executa todo o código e toda a lógica relativa a imprimir as
* estatísticas no ficheiro.
*/
void execute_stats(struct data_container* data, int totalAdmissions);

#endif
