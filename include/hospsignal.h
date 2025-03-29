/*
*   Grupo SO-018 integrado por:
*      Pedro Correia fc59791
*      Renan Silva fc59802
*      Sofia Reis fc59880
*/

#ifndef HOSPSIGNAL_H_GUARD
#define HOSPSIGNAL_H_GUARD

#include <synchronization.h>
#include <memory.h>
#include <main.h>

/*
* Este método dá inicio à captura de sinais de termino do programa (ou seja CTRL+C), que caso
* capture algum invoca o método terminate_program().
*/
void start_capture(struct data_container* data, struct communication* comm, struct semaphores* sems);

/*
* Este método é invocado quando é dada a ordem de encerramento do programa através do sinal.
*/
void terminate_program();

/*
* Este método permite definir um alarme que a cada X segundos (X sendo o valor de interval enviado
* como parâmetro) imprime na consola o estado das admissões atuais.
*/
void set_alarm(int interval);

/*
* Este método imprime o estado da admissão enviada como parâmetro na consola.
*/
void print_admission_status(struct admission ad);

/*
* Este método permite imprimir na consola o estado do programa, nomeadamente o estado das admissões.
*/
void print_info();

#endif
