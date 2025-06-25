#ifndef CONFIG_H
#define CONFIG_H

#define MAX_SPORTELLI 128
#define MAX_LAVORATORI 128
#define NUM_SERVIZI 6

typedef struct sportello{
    int tipoLavoro;
} Sportello;

struct msgbuf {
    long mtype;
    Sportello mtext;
};

typedef struct workerMsg {
    long mtype;
    pid_t pid;
} WorkerMsg;

typedef struct ticket_msg {
    long mtype;      // Tipo messaggio: 1 per richiesta, PID utente per risposta
    int servizio;    // Tipo di servizio richiesto
    int ticket;      // Numero del ticket (solo nella risposta)
    pid_t pid;       // PID dell'utente (solo nella richiesta)
} TicketMsg;

typedef struct ticket {
    int type;
    int time;
} Ticket,*TicketPtr;

int tempario[NUM_SERVIZI] = {10, 8, 6, 8, 20, 20};

typedef struct Config {
    int NOF_WORKER_SEATS;
    int NOF_WORKERS;
    int NOF_USERS;
    int SIM_DURATION;
    int DAYS_LEFT;
    int DAY;
    int N_NANO_SECS;
    int SERVICE;
    int TIME_SERVICE;
    int P_SERV_MIN;
    int P_SERV_MAX;
    int NOF_PAUSE;
    int sportelli[MAX_SPORTELLI];
    int lavoratori[MAX_LAVORATORI];
} Config;

typedef struct StatsDay {
    int utenti_serviti; // utenti serviti nella giornata
    int servizi_erogati[NUM_SERVIZI]; // per tipo servizio
    int servizi_non_erogati[NUM_SERVIZI];
    float tempo_attesa_utenti; // somma tempi attesa utenti nella giornata
    float tempo_erogazione_servizi[NUM_SERVIZI]; // per tipo servizio
    int operatori_attivi; // operatori attivi nella giornata
    float pause_giornata; // pause effettuate nella giornata
    float rapporto_op_sportelli[MAX_SPORTELLI]; // rapporto per ogni sportello
} StatsDay;

typedef struct StatsSim {
    int utenti_serviti_tot; // utenti serviti totali
    float utenti_serviti_media_giorno; // media utenti serviti al giorno
    int servizi_erogati_tot[NUM_SERVIZI];
    int servizi_non_erogati_tot[NUM_SERVIZI];
    float servizi_erogati_media_giorno[NUM_SERVIZI];
    float servizi_non_erogati_media_giorno[NUM_SERVIZI];
    float tempo_attesa_utenti_tot; // somma tempi attesa utenti in sim
    float tempo_erogazione_servizi_tot[NUM_SERVIZI];
    float tempo_erogazione_servizi_media_giorno[NUM_SERVIZI];
    int operatori_attivi_tot;
    float operatori_attivi_media_giorno;
    float pause_tot;
    float pause_media_giorno;
    float rapporto_op_sportelli_media[MAX_SPORTELLI];
} StatsSim;

#endif