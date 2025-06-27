#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/**
 * @brief Inizializza i valori della struttura StatsDay.
 * @param day Puntatore alla struttura StatsDay da inizializzare.
 */
void setValues(StatsDay *day/*, StatsSim *sim, int resetDay*/);

/**
 * @brief Inizializza la coda di messaggi per i lavoratori.
 * @param shared_memory Puntatore alla struttura Config condivisa.
 * @param msgId Identificatore della coda di messaggi.
 * @param message Puntatore alla struttura msgbuf per il messaggio.
 */
void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message);

/**
 * @brief Esegue un'operazione su un semaforo (P/V).
 * @param semid Identificatore del set di semafori.
 * @param sem_num Indice del semaforo nel set.
 * @param sem_op Operazione da eseguire (incremento/decremento/attesa).
 */
void sem_op(int semid, int sem_num, int sem_op);

/**
 * @brief Gestore dei segnali per i processi.
 * @param sig Segnale ricevuto.
 */
void signal_handler(int sig);

/**
 * @brief Stampa un messaggio di errore e termina il programma.
 * @param s Messaggio di errore da stampare.
 */
void errExit(char* s);

/**
 * @brief Funzione principale del direttore: gestisce la simulazione e la creazione dei processi.
 * @param semWaitInit Semaforo per la sincronizzazione dell'inizializzazione.
 * @param shm_id ID memoria condivisa Config.
 * @param shm_id_StatsDay ID memoria condivisa StatsDay.
 * @param shm_id_StatsSim ID memoria condivisa StatsSim.
 * @param shared_memory Puntatore a Config condivisa.
 * @param statsDay Puntatore a StatsDay condivisa.
 * @param statsSim Puntatore a StatsSim condivisa.
 */
void direttore(int semWaitInit, int shm_id, int shm_id_StatsDay, int shm_id_StatsSim, Config* shared_memory, StatsDay* statsDay, StatsSim* statsSim);

/**
 * @brief Carica la configurazione dal file e popola la struttura Config.
 * @param filename Nome del file di configurazione.
 * @param config Puntatore alla struttura Config da popolare.
 */
void load_config(const char *filename, Config *config);

/**
 * @brief Ciclo operativo di un lavoratore: gestisce la ricezione e l'esecuzione dei lavori.
 * @param semLavoratore Semaforo del lavoratore.
 * @param i Indice del lavoratore.
 * @param tipoLavoro Tipo di lavoro che il lavoratore può svolgere.
 * @param msgid ID della coda di messaggi.
 * @param nanoSec Fattore di tempo per la simulazione.
 * @param nofPause Numero di pause disponibili.
 * @param statsDay Puntatore a StatsDay condivisa.
 * @param statsSim Puntatore a StatsSim condivisa.
 */
void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause, StatsDay *statsDay, StatsSim *statsSim);

/**
 * @brief Esegue un'operazione su un semaforo (P/V). (Duplicato, da rimuovere se non necessario)
 * @param semid Identificatore del set di semafori.
 * @param sem_num Indice del semaforo nel set.
 * @param sem_op Operazione da eseguire (incremento/decremento/attesa).
 */
void sem_op(int semid, int sem_num, int sem_op);

/**
 * @brief Stampa le statistiche della giornata.
 * @param statsDay Puntatore alla struttura StatsDay.
 * @param count_operatori Array con il conteggio degli operatori per servizio.
 * @param count_sportelli Array con il conteggio degli sportelli per servizio.
 * @param gg giorno 
 */
void printStatsDay(StatsDay *statsDay, int *count_operatori, int *count_sportelli, int gg);

/**
 * @brief Stampa le statistiche della simulazione.
 * @param statsSim Puntatore alla struttura StatsSim.
 * @param shared_memory Puntatore alla struttura shared_memory
 * @param count_operatori Array con il conteggio degli operatori per servizio.
 * @param count_sportelli Array con il conteggio degli sportelli per servizio.
 * @param gg giorno
 */
void printStatsSim(StatsSim *statsSim, Config *shared_memory, int *count_operatori, int *count_sportelli, int gg);

#endif