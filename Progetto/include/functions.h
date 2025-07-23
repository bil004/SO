#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>

/**
 * @file functions.h
 * @brief Dichiarazioni delle funzioni utilizzate nel progetto
 */

/**
 * @brief Funzione per aggiungere un messaggio alla coda di messaggi
 *
 * @param shared_memory Puntatore alla memoria condivisa
 * @param msgId Identificatore del messaggio
 * @param message Puntatore alla struttura del messaggio
 */
void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message);

/**
 * @brief Funzione per eseguire un'operazione su un semaforo
 *
 * @param semid Identificatore del set di semafori
 * @param sem_num Indice del semaforo nel set
 * @param sem_op Operazione da eseguire (incremento/decremento/attesa)
 */
void sem_op(int semid, int sem_num, int sem_op);

/**
 * @brief Funzione per gestire i segnali
 *
 * @param sig Segnale ricevuto
 */
void signal_handler(int sig);

/**
 * @brief Funzione per stampare un messaggio di errore e terminare il programma
 *
 * @param s Messaggio di errore da stampare
 */
void errExit(char* s);

/**
 * @brief Funzione principale del direttore
 *
 * @param semWaitInit Semaforo per la sincronizzazione dell'inizializzazione
 * @param shm_id ID memoria condivisa Config
 * @param shm_id_StatsDay ID memoria condivisa StatsDay
 * @param shm_id_StatsSim ID memoria condivisa StatsSim
 * @param shared_memory Puntatore a Config condivisa
 * @param statsDay Puntatore a StatsDay condivisa
 * @param statsSim Puntatore a StatsSim condivisa
 * @param explode Puntatore a ESPLOSIONE!!!
 */
void direttore(int semWaitInit, int shm_id, int shm_id_StatsDay, int shm_id_StatsSim, Config* shared_memory, StatsDay* statsDay, StatsSim* statsSim, bool *explode);

/**
 * @brief Funzione per caricare la configurazione dal file
 *
 * @param filename Nome del file di configurazione
 * @param config Puntatore alla struttura Config da popolare
 */
void load_config(const char *filename, Config *config);

/**
 * @brief Funzione per il ciclo operativo di un lavoratore
 *
 * @param semLavoratore Semaforo del lavoratore
 * @param i Indice del lavoratore
 * @param tipoLavoro Tipo di lavoro che il lavoratore può svolgere
 * @param msgid ID della coda di messaggi
 * @param nanoSec Fattore di tempo per la simulazione
 * @param nofPause Numero di pause disponibili
 * @param statsDay Puntatore a StatsDay condivisa
 * @param statsSim Puntatore a StatsSim condivisa
 * @param pPause Puntatore a pause
 * @param sharedMemory Puntatore a Config condivisa
 */
void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause, StatsDay *statsDay, StatsSim *statsSim, int pPause, Config *sharedMemory);

/**
 * @brief Funzione per stampare le statistiche della giornata
 *
 * @param statsDay Puntatore alla struttura StatsDay
 * @param csv File di output per le statistiche
 * @param count_operatori Array con il conteggio degli operatori per servizio
 * @param count_sportelli Array con il conteggio degli sportelli per servizio
 * @param gg Giorno
 */
void printStatsDay(Config *shared_memory, StatsDay *statsDay, FILE *csv, int *count_sportelli, int *count_operatori, int gg);

/**
 * @brief Funzione per stampare le statistiche della simulazione
 *
 * @param statsSim Puntatore alla struttura StatsSim
 * @param csv File di output per le statistiche
 * @param shared_memory Puntatore alla struttura shared_memory
 * @param count_operatori Array con il conteggio degli operatori per servizio
 * @param count_sportelli Array con il conteggio degli sportelli per servizio
 * @param gg Giorno
 */
void printStatsSim(StatsSim *statsSim, FILE *csv, Config *shared_memory, int *tot_operatori, int *tot_sportelli, int gg);

#endif

