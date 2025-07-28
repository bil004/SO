#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Definizioni di costanti, strutture dati e configurazioni globali della simulazione.
 */

/// Numero massimo di sportelli disponibili nel sistema.
#define MAX_SPORTELLI 128

/// Numero massimo di lavoratori nel sistema.
#define MAX_LAVORATORI 128

/// Numero totale di servizi differenti offerti.
#define NUM_SERVIZI 6

/**
 * @brief Struttura che rappresenta uno sportello.
 */
typedef struct sportello {
    int tipoLavoro;  ///< Tipo di servizio che lo sportello può gestire.
} Sportello;

/**
 * @brief Struttura per i messaggi nella coda tra utenti e sportelli.
 */
struct msgbuf {
    long mtype;       ///< Tipo di messaggio (per System V IPC).
    Sportello mtext;  ///< Dati del messaggio (tipo di sportello).
};

/**
 * @brief Messaggio usato dai lavoratori per comunicare con il direttore.
 */
typedef struct workerMsg {
    long mtype;       ///< Tipo di messaggio.
    pid_t pid;        ///< PID del lavoratore.
} WorkerMsg;

/**
 * @brief Messaggio di richiesta/risposta di ticket da parte degli utenti.
 */
typedef struct ticket_msg {
    long mtype;       ///< Tipo del messaggio: 1 per richiesta, PID per risposta.
    int servizio;     ///< Servizio richiesto.
    int ticket;       ///< Numero di ticket (solo in risposta).
    pid_t pid;        ///< PID dell'utente (solo in richiesta).
} TicketMsg;

/**
 * @brief Rappresentazione interna di un ticket.
 */
typedef struct ticket {
    int type;         ///< Tipo di servizio del ticket.
    int time;         ///< Tempo necessario per erogare il servizio.
} Ticket, *TicketPtr;

/// Tempi di erogazione dei servizi, in unità arbitrarie.
int tempario[NUM_SERVIZI] = {10, 8, 6, 8, 20, 20};

/**
 * @brief Configurazione globale condivisa per tutti i processi.
 */
typedef struct Config {
    int NOF_WORKER_SEATS;   ///< Numero di postazioni disponibili per i lavoratori.
    int NOF_WORKERS;        ///< Numero totale di lavoratori.
    int NOF_USERS;          ///< Numero di utenti nella simulazione.
    int SIM_DURATION;       ///< Durata complessiva della simulazione.
    int DAYS_LEFT;          ///< Giorni rimanenti nella simulazione.
    int N_NANO_SECS;        ///< Fattore di tempo per la simulazione in nanosecondi.
    int P_SERV_MIN;         ///< Percentuale minima assegnabile a un servizio.
    int P_SERV_MAX;         ///< Percentuale massima assegnabile a un servizio.
    int NOF_PAUSE;          ///< Numero di pause concesse a ciascun lavoratore.
    int P_PAUSE;            ///< Percentuale di probabilità che un lavoratore faccia una pausa.
    int EXPLODE_THRESHOLD;  ///< Soglia per attivare la simulazione di un'esplosione.
    int sportelli[MAX_SPORTELLI];   ///< Configurazione degli sportelli per tipo di servizio.
    int lavoratori[MAX_LAVORATORI]; ///< Mappatura dei lavoratori e dei loro servizi.
} Config;

/**
 * @brief Statistiche giornaliere raccolte durante la simulazione.
 */
typedef struct StatsDay {
    int utenti_serviti;                             ///< Numero totale di utenti serviti nel giorno.
    int servizi_erogati[NUM_SERVIZI];               ///< Servizi erogati per ciascun tipo.
    int servizi_non_erogati[NUM_SERVIZI];           ///< Servizi richiesti ma non erogati.
    float tempo_attesa_utenti;                      ///< Tempo medio di attesa degli utenti.
    float tempo_erogazione_servizi[NUM_SERVIZI];    ///< Tempo medio di erogazione per tipo di servizio.
    int operatori_attivi;                           ///< Numero di operatori attivi.
    int pause_giornata;                             ///< Numero totale di pause effettuate nel giorno.
} StatsDay;

/**
 * @brief Statistiche aggregate sull'intera simulazione.
 */
typedef struct StatsSim {
    int utenti_serviti_tot;                         ///< Totale utenti serviti.
    int servizi_erogati_tot[NUM_SERVIZI];           ///< Totale servizi erogati per tipo.
    int servizi_non_erogati_tot[NUM_SERVIZI];       ///< Totale servizi non erogati per tipo.
    float servizi_non_erogati_media_giorno[NUM_SERVIZI]; ///< Media giornaliera dei servizi non erogati.
    float tempo_attesa_utenti_tot;                  ///< Tempo totale di attesa degli utenti.
    float tempo_erogazione_servizi_tot[NUM_SERVIZI];///< Tempo totale di erogazione per tipo.
    int operatori_attivi_tot;                       ///< Totale operatori attivi durante tutta la simulazione.
    int pause_tot;                                  ///< Totale pause effettuate.
    float rapporto_op_sportelli_media[MAX_SPORTELLI]; ///< Media del rapporto operatori/sportelli.
} StatsSim;

#endif