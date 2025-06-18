#ifndef CONFIG_H
#define CONFIG_H

#define MAX_SPORTELLI 128
#define MAX_LAVORATORI 128
#define NUM_SERVIZI 6

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

typedef struct Stats {
    /*
    // Utenti
    int UTENTI_TOT;
    float UTENTI_GIORNO;

    // Servizi globali
    int SERV_TOT;
    int SERV_FAIL;
    float SERV_GIORNO;
    float FAIL_GIORNO;

    // Tempi globali
    float WAIT_TOT;//attesa degli user nella simulazione
    float WAIT_GIORNO;//attesa degli user nel giorno
    float DUR_TOT;//tempo medio erogazione servizi in sim
    float DUR_GIORNO;//tempo medio erogazione servizi in gg
    */

    // Per servizio
    int SERV_TOT_S[NUM_SERVIZI];
    int SERV_FAIL_S[NUM_SERVIZI];
    float SERV_GIORNO_S[NUM_SERVIZI];
    float FAIL_GIORNO_S[NUM_SERVIZI];
    float WAIT_TOT_S[NUM_SERVIZI];
    float WAIT_GIORNO_S[NUM_SERVIZI];
    float DUR_TOT_S[NUM_SERVIZI];
    float DUR_GIORNO_S[NUM_SERVIZI];

    /*
        - Per WAIT_GIORNO_S, ogni user aggiunge in una var (in memoria condivisa) il suo tempo impiegato
        - UTENTI_TOT la calcoliamo tramite SERV_TOT_S
    */

    // Operatori
    int OP_GIORNO;
    int OP_TOT;

    // Pause
    float PAUSE_GIORNO;
    int PAUSE_TOT;

    // Rapporto operatori/sportelli [giorno][sportello]
    //float OP_SPORT_RATIO[NUM_GIORNI][NUM_SPORTELLI];

} Stats;

#endif