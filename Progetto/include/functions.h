#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>

/**
 * @file functions.h
 * @brief Dichiarazioni delle funzioni utilizzate nel progetto di simulazione.
 */

/**
 * @brief Aggiunge un messaggio alla coda dei messaggi.
 *
 * @param shared_memory Puntatore alla struttura Config nella memoria condivisa.
 * @param msgId Identificatore della coda dei messaggi.
 * @param message Puntatore alla struttura del messaggio da inviare.
 */
void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message);

/**
 * @brief Esegue un'operazione (P/V) su un semaforo del set.
 *
 * @param semid Identificatore del set di semafori.
 * @param sem_num Indice del semaforo all'interno del set.
 * @param sem_op Operazione da eseguire:
 *               -1 per wait (P), +1 per signal (V), 0 per wait finché zero.
 */
void sem_op(int semid, int sem_num, int sem_op);

/**
 * @brief Gestisce i segnali ricevuti dal processo.
 *
 * @param sig Codice del segnale ricevuto (es. SIGINT, SIGTERM).
 */
void signal_handler(int sig);

/**
 * @brief Stampa un messaggio di errore e termina il programma.
 *
 * @param s Stringa contenente il messaggio di errore.
 */
void errExit(char* s);

/**
 * @brief Funzione principale eseguita dal processo "direttore".
 *
 * @param semWaitInit Semaforo per sincronizzare l'inizializzazione dei processi.
 * @param shm_id ID della memoria condivisa per la struttura Config.
 * @param shm_id_StatsDay ID della memoria condivisa per StatsDay.
 * @param shm_id_StatsSim ID della memoria condivisa per StatsSim.
 * @param shared_memory Puntatore alla struttura Config condivisa.
 * @param statsDay Puntatore alla struttura StatsDay condivisa.
 * @param statsSim Puntatore alla struttura StatsSim condivisa.
 * @param explode Puntatore a variabile booleana per la gestione di un'esplosione simulata.
 */
void direttore(int semWaitInit, int shm_id, int shm_id_StatsDay, int shm_id_StatsSim, Config* shared_memory, StatsDay* statsDay, StatsSim* statsSim, bool *explode);

/**
 * @brief Carica i parametri di configurazione da file.
 *
 * @param filename Nome del file di configurazione (formato previsto).
 * @param config Puntatore alla struttura Config da inizializzare.
 */
void load_config(const char *filename, Config *config);

/**
 * @brief Ciclo operativo di un lavoratore nel sistema simulato.
 *
 * @param semLavoratore Semaforo per la sincronizzazione del lavoratore.
 * @param i Indice del lavoratore (identificativo univoco).
 * @param tipoLavoro Tipo di lavoro che il lavoratore può svolgere (servizio).
 * @param msgid ID della coda di messaggi per la comunicazione.
 * @param nanoSec Fattore di moltiplicazione per la simulazione temporale.
 * @param nofPause Numero di pause a disposizione del lavoratore.
 * @param statsDay Puntatore alla struttura StatsDay condivisa.
 * @param statsSim Puntatore alla struttura StatsSim condivisa.
 * @param pPause Indice nella struttura delle pause condivise.
 * @param sharedMemory Puntatore alla struttura Config condivisa.
 */
void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause, StatsDay *statsDay, StatsSim *statsSim, int pPause, Config *sharedMemory);

/**
 * @brief Stampa le statistiche giornaliere su file CSV.
 *
 * @param shared_memory Puntatore alla struttura Config condivisa.
 * @param statsDay Puntatore alla struttura StatsDay contenente le statistiche.
 * @param csv Puntatore al file CSV di output.
 * @param count_sportelli Array contenente il conteggio degli sportelli per servizio.
 * @param count_operatori Array contenente il conteggio degli operatori per servizio.
 * @param gg Giorno corrente della simulazione.
 */
void printStatsDay(Config *shared_memory, StatsDay *statsDay, FILE *csv, int *count_sportelli, int *count_operatori, int gg);

/**
 * @brief Stampa le statistiche complessive della simulazione su file CSV.
 *
 * @param statsSim Puntatore alla struttura StatsSim con i dati aggregati.
 * @param csv Puntatore al file CSV di output.
 * @param shared_memory Puntatore alla struttura Config condivisa.
 * @param tot_operatori Array con il conteggio totale degli operatori per servizio.
 * @param tot_sportelli Array con il conteggio totale degli sportelli per servizio.
 * @param gg Numero di giorni simulati.
 */
void printStatsSim(StatsSim *statsSim, FILE *csv, Config *shared_memory, int *tot_operatori, int *tot_sportelli, int gg);

#endif