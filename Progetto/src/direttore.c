#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/msg.h>

#include "../include/config.h"
#include "../include/functions.h"

void errExit(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        perror("[DIRECTOR] Direttore Ucciso");
        exit(EXIT_FAILURE); // Imposta il flag per terminare il processo
    }
}

void sem_op(int semid, int sem_num, int sem_op) {
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1) {
        perror("[DIRECTOR] Errore nell'operazione sul semaforo");
        exit(EXIT_FAILURE);
    }
}

void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message) {

    for(int i = 0; i< shared_memory->NOF_WORKER_SEATS; i++){
        Sportello sp;
        sp.tipoLavoro = (rand() % 6) + 1; //tipo lavoro da 1 a 6
        shared_memory->sportelli[i] = sp.tipoLavoro;
        
        printf("\033[1;32m[DIRECTOR] Sportello %d che offre il servizio: %d \033[0m\n", i, sp.tipoLavoro);

        
        message->mtype =  sp.tipoLavoro;
        message->mtext.tipoLavoro = sp.tipoLavoro;
        
        //invio messaggio sulla coda
        msgsnd(msgId, message, sizeof(Sportello), 0);

    }
}

void setValues(StatsDay *statsDay, int *count_sportelli, int *count_operatori) {
    statsDay->utenti_serviti = 0;
    statsDay->operatori_attivi = 0;
    statsDay->pause_giornata = 0;
    statsDay->tempo_attesa_utenti = 0;
    
    for (int i = 0; i < NUM_SERVIZI; i++) {
        statsDay->servizi_erogati[i] = 0;
        statsDay->servizi_non_erogati[i] = 0;
        statsDay->tempo_erogazione_servizi[i] = 0;
        count_sportelli[i] = 0;
        count_operatori[i] = 0;
    }
}

void printStatsDay(Config *shared_memory, StatsDay *statsDay, FILE *csv, int *count_operatori, int *count_sportelli, int gg) {
    // STAMPA SUL FILE .csv
    fprintf(csv, "%d;", gg);
    fprintf(csv, "%d;", statsDay->utenti_serviti);
    fprintf(csv, "%.2f;", (float)statsDay->utenti_serviti/shared_memory->NOF_USERS);

    for(int s = 0; s < NUM_SERVIZI; s++) 
        fprintf(csv, "%d;", statsDay->servizi_erogati[s]);

    for(int s = 0; s < NUM_SERVIZI; s++) 
        fprintf(csv, "%d;", statsDay->servizi_non_erogati[s]);

    fprintf(csv, "%.2f;", statsDay->tempo_attesa_utenti);

    for(int s = 0; s < NUM_SERVIZI; s++) 
        fprintf(csv, "%.2f;", statsDay->tempo_erogazione_servizi[s]);
    

    for(int s = 0; s < NUM_SERVIZI; s++) {
        if (statsDay->servizi_erogati[s] > 0)
            fprintf(csv, "%.2f;", statsDay->tempo_erogazione_servizi[s]/ statsDay->servizi_erogati[s]);    
        else
            fprintf(csv, "%d;", statsDay->servizi_erogati[s]);
    }

    fprintf(csv, "%d;", statsDay->operatori_attivi);
    fprintf(csv, "%d;", statsDay->pause_giornata);

    for (int i = 0; i < NUM_SERVIZI; i++) {
        int index = i+1;
        if (count_sportelli[i] > 0)
            fprintf(csv, "%.4f;", (float)count_operatori[i] / count_sportelli[i]);
        else
            fprintf(csv, "%.4f;", 0.0f);
    }
    fprintf(csv, "\n");

    // STAMPA A VIDEO
    printf("\n--- STATISTICHE GIORNO %d ---\n", gg);
    printf("Utenti serviti oggi: %d\n", statsDay->utenti_serviti);
    printf("Utenti serviti in media oggi: %.2f\n", (float)statsDay->utenti_serviti/shared_memory->NOF_USERS);
    printf("Operatori attivi oggi: %d\n", statsDay->operatori_attivi);
    printf("Tempo attesa utenti: %f\n", statsDay->tempo_attesa_utenti);
    printf("Pause effettuate dagli operatori in giornata: %d\n", statsDay->pause_giornata);

    for(int s=0; s < NUM_SERVIZI; s++) { 
        printf("Servizio %d: erogati %d, non erogati %d, tempo medio %f\n", s+1, statsDay->servizi_erogati[s], statsDay->servizi_non_erogati[s], statsDay->tempo_erogazione_servizi[s]);

        if (statsDay->servizi_erogati[s] > 0)
            printf("Media tempo erogazione servizio %d: %.2f\n", s, statsDay->tempo_erogazione_servizi[s]/statsDay->servizi_erogati[s]);
        else
            printf("Media tempo erogazione servizio %d: %d\n", s, statsDay->servizi_erogati[s]);
    }

    printf("Tot pause: %d\n", statsDay->pause_giornata);

    if (statsDay->utenti_serviti > 0)
        printf("Tempo di attesa media utenti: %.4f\n", statsDay->tempo_attesa_utenti/statsDay->utenti_serviti);

    if (statsDay->operatori_attivi > 0)
        printf("Media pause: %0.3f", (float)statsDay->pause_giornata/statsDay->operatori_attivi);

    if (statsDay->utenti_serviti > 0)
        printf("Tempo di attesa media utenti: %.4f\n", statsDay->tempo_attesa_utenti/statsDay->utenti_serviti);
    else
        printf("Tempo di attesa media utenti: %.4f\n", 0.0f);

    for (int i = 0; i < NUM_SERVIZI; i++) {
        int index = i+1;
        if (count_sportelli[i] > 0) {
            printf("Rapporto operatori/sportelli %d: %.4f\n", index, (float)count_operatori[i] / count_sportelli[i]);
            printf("TOT OPERATORI PER SERVIZIO %d: %d\n", i+1, count_operatori[i]);
            printf("TOT SPORTELLI PER SERVIZIO %d: %d\n", i+1, count_sportelli[i]);
        }
        else
            printf("Rapporto operatori/sportelli %d: %.4f\n", index, 0.0f);
    }
}

void printStatsSim(StatsSim *statsSim, FILE *csv, Config *shared_memory, int *tot_operatori, int *tot_sportelli, int gg) {    
    if (gg == shared_memory->DAYS_LEFT) {
        // STAMPA A VIDEO
        puts("\n--- STATISTICHE SIMULAZIONE ---\n");
        printf("Utenti serviti totali: %d\n", statsSim->utenti_serviti_tot);
        printf("Utenti serviti in media: %.2f\n", (float)statsSim->utenti_serviti_tot/shared_memory->SIM_DURATION);
        printf("Operatori attivi totali: %d\n", statsSim->operatori_attivi_tot);
        printf("Tempo attesa utenti: %.4f\n", statsSim->tempo_attesa_utenti_tot);
        printf("Operatori in media al giorno: %.2f\n", statsSim->operatori_attivi_tot/(float)shared_memory->SIM_DURATION);
        printf("Pause totali operatori: %d\n", statsSim->pause_tot);
        
        puts("\n\n--- STATISTICHE IN BASE AL TIPO DI SERVIZIO ---\n");

        for (int i = 0; i < NUM_SERVIZI; i++) {
            printf("SERV_TOT_S: lavoro %d, %d\n", i+1, statsSim->servizi_erogati_tot[i]);
            printf("SERV_FAIL_S: lavoro %d, %d\n", i+1, statsSim->servizi_non_erogati_tot[i]);
            printf("SERV_GIORNO_S (SUCCESS): lavoro %d, %.2f\n", i+1, (float)statsSim->servizi_erogati_tot[i]/shared_memory->SIM_DURATION);
            printf("SERV_GIORNO_S (FAILED): lavoro %d, %.2f\n", i+1, (float)statsSim->servizi_non_erogati_tot[i]/shared_memory->SIM_DURATION);
            

            puts("");
        }

        puts("");
        for (int i = 0; i < NUM_SERVIZI; i++) {
            printf("Tempo erogazione servizio %d: %.2f\n", i+1, statsSim->tempo_erogazione_servizi_tot[i]);
        }

        puts("");
        for(int i = 0; i < NUM_SERVIZI; i++) {
            if (tot_sportelli[i] > 0)
                printf("Rapporto operatori/sportelli %d: %.4f\n", i+1, (float)tot_operatori[i] / tot_sportelli[i]);
            else
                printf("Rapporto operatori/sportelli %d: %.4f\n", i+1, 0.0f);
        }

        // STAMPA SUL FILE
        fprintf(csv, "\n\n\nSTATISTICHE FINALI\n");
        fprintf(csv, "Utenti serviti totali;Media Utenti serviti totali;");
        fprintf(csv, "Tot Servizio 1 erogati;Tot Servizio 2 erogati;Tot Servizio 3 erogati;Tot Servizio 4 erogati;Tot Servizio 5 erogati;Tot Servizio 6 erogati;");
        fprintf(csv, "Tot Servizio 1 non erogati;Tot Servizio 2 non erogati;Tot Servizio 3 non erogati;Tot Servizio 4 non erogati;Tot Servizio 5 non erogati;Tot Servizio 6 non erogati;");
        fprintf(csv, "Servizio 1 erogato in media al giorno;Servizio 2 erogato in media al giorno;Servizio 3 erogato in media al giorno;Servizio 4 erogato in media al giorno;Servizio 5 erogato in media al giorno;Servizio 6 erogato in media al giorno;");
        fprintf(csv, "Servizio 1 non erogato in media al giorno;Servizio 2 non erogato in media al giorno;Servizio 3 non erogato in media al giorno;Servizio 4 non erogato in media al giorno;Servizio 5 non erogato in media al giorno;Servizio 6 non erogato in media al giorno;");
        fprintf(csv, "Tempo medio attesa utenti Tot(min);");
        fprintf(csv, "Tempo erogazione servizio 1;Tempo erogazione servizio 2;Tempo erogazione servizio 3;Tempo erogazione servizio 4;Tempo erogazione servizio 5;Tempo erogazione servizio 6;");
        fprintf(csv, "Media tempo erogazione servizio 1;Media tempo erogazione servizio 2;Media tempo erogazione servizio 3;Media tempo erogazione servizio 4;Media tempo erogazione servizio 5;Media tempo erogazione servizio 6;");
        fprintf(csv, "Operatori Totali attivi;Pause Totali effettuate al gg;Operatori al gg;");
        fprintf(csv, "Rapporto Operatori/Sportelli(1);Rapporto Operatori/Sportelli(2);Rapporto Operatori/Sportelli(3);Rapporto Operatori/Sportelli(4);Rapporto Operatori/Sportelli(5);Rapporto Operatori/Sportelli(6)\n");

        fprintf(csv, "%d;", statsSim->utenti_serviti_tot);
        fprintf(csv, "%.5f;", (float)statsSim->utenti_serviti_tot/shared_memory->SIM_DURATION);
        
        for (int i = 0; i < NUM_SERVIZI; i++) 
            fprintf(csv, "%d;", statsSim->servizi_erogati_tot[i]);
        
        for (int i = 0; i < NUM_SERVIZI; i++) 
            fprintf(csv, "%d;", statsSim->servizi_non_erogati_tot[i]);
        
        for (int i = 0; i < NUM_SERVIZI; i++) 
            fprintf(csv, "%.5f;", (float)statsSim->servizi_erogati_tot[i]/shared_memory->SIM_DURATION);

        for (int i = 0; i < NUM_SERVIZI; i++) 
            fprintf(csv, "%.5f;", (float)statsSim->servizi_non_erogati_tot[i]/shared_memory->SIM_DURATION);
        
        fprintf(csv, "%.5f;", statsSim->tempo_attesa_utenti_tot);

        for (int i = 0; i < NUM_SERVIZI; i++) {
            fprintf(csv, "%.5f;", statsSim->tempo_erogazione_servizi_tot[i]);
        }  
        
        for (int i = 0; i < NUM_SERVIZI; i++) {
            fprintf(csv, "%.5f;", (float)statsSim->tempo_erogazione_servizi_tot[i]/shared_memory->SIM_DURATION);
        } 
        
        fprintf(csv, "%d;", statsSim->operatori_attivi_tot);
        fprintf(csv, "%d;", statsSim->pause_tot);
        fprintf(csv, "%.5f;", (float)statsSim->operatori_attivi_tot/shared_memory->SIM_DURATION);
        
        for (int i = 0; i < NUM_SERVIZI; i++) { 
            if (tot_sportelli[i] > 0) {
                fprintf(csv, "%.5f;", (float)tot_operatori[i] / tot_sportelli[i]);
                printf("TOT OPERATORI PER SERVIZIO %d: %d\n", i+1, tot_operatori[i]);
                printf("TOT SPORTELLI PER SERVIZIO %d: %d\n", i+1, tot_sportelli[i]);
            }
            else
                fprintf(csv, "%.5f;", 0.0f);
        }
        fprintf(csv, "\n");

    } else {
        printf("\n--- STATISTICHE SIMULAZIONE (aggiornate) ---\n");
        printf("Utenti serviti totali: %d\n", statsSim->utenti_serviti_tot);
        printf("Operatori attivi totali: %d\n", statsSim->operatori_attivi_tot);
        printf("Tempo attesa utenti: %.5ff\n", statsSim->tempo_attesa_utenti_tot);
        printf("Utenti serviti: %d\n", statsSim->utenti_serviti_tot);

        for (int i = 0; i < NUM_SERVIZI; i++) {
            int index = i+1;
            if (tot_sportelli[i] > 0) {
                printf("Rapporto operatori/sportelli %d: %.5f\n", index, (float)tot_operatori[i] / tot_sportelli[i]);
                printf("TOT OPERATORI PER SERVIZIO %d: %d\n", i+1, tot_operatori[i]);
                printf("TOT SPORTELLI PER SERVIZIO %d: %d\n", i+1, tot_sportelli[i]);
            }
            else
                printf("Rapporto operatori/sportelli %d: %.5f\n", index, 0.0f);
        }
        
    }
}

void direttore(int semWaitInit, int shm_id, int shm_id_StatsDay, int shm_id_StatsSim, Config* shared_memory, StatsDay* statsDay, StatsSim* statsSim, bool *explode) {
    pid_t* pid_array = malloc(sizeof(pid_t) * (shared_memory->NOF_USERS + shared_memory->NOF_WORKERS + 1));
    int j = 0;

    srand(time(NULL));

    char shm_id_str[10], semWaitInit_str[10], shm_id_StatsDay_str[10], shm_id_StatsSim_str[10];
    snprintf(shm_id_str, 10, "%d", shm_id);
    snprintf(shm_id_StatsDay_str, 10, "%d", shm_id_StatsDay);
    snprintf(shm_id_StatsSim_str, 10, "%d", shm_id_StatsSim);
    snprintf(semWaitInit_str, 10, "%d", semWaitInit);

    //Creazione processi figli
    //-----------------Erogatore Ticket------------------
    pid_t erogatoreTicket = fork();
    switch(erogatoreTicket) {
        case -1:
            perror("[DIRECTOR] erogatore_ticket not created!");
            exit(EXIT_FAILURE);
            break;

        case 0:
            printf("\033[1;32m[DIRECTOR] Erogatore ticket con pid:%d\033[0m\n", getpid());
            execl("./erogatoreTicket", "./erogatoreTicket", semWaitInit_str, shm_id_str, shm_id_StatsDay_str, shm_id_StatsSim_str, NULL);
            errExit("execl failure!");
            break;
            
        default:
            pid_array[j] = erogatoreTicket;
            j++;
            break;
    }

    //-------------------Inizializzazione Sportelli---------------------

    //Creare la Coda di messaggi per i lavoratori
    int msgId = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgId == -1) {
        perror("[DIRECTOR] Errore coda di messaggi!");
        exit(1);
    }
    struct msgbuf message;
    struct msqid_ds buf;
    msg_enqueue(shared_memory, msgId, &message);
    

    //-------------------Operatori-----------------
    pid_t* opUsrPid = malloc(sizeof(pid_t) * (shared_memory->NOF_WORKERS + shared_memory->NOF_USERS));
    char tipiWorkers[64];

    int k = 0;
    for(int i = 0; i < shared_memory->NOF_WORKERS; i++) {
        pid_t Lavoratore = fork();
        switch(Lavoratore) {
            case -1:
                errExit("[DIRECTOR] operatore process error!");
                break;

            case 0:
                srand(time(NULL)^getpid());
                char I[64];
                char msgId_str[64];
                sprintf(I, "%d", i);
                sprintf(msgId_str, "%d", msgId);
                int tipolavoro = (rand() % 6) + 1;
                shared_memory->lavoratori[k] = tipolavoro;
                char tipoLavoro_str[64];
                sprintf(tipoLavoro_str, "%d", tipolavoro);
                execl("./operatore", "./operatore", semWaitInit_str, shm_id_str, I, msgId_str, tipoLavoro_str, shm_id_StatsDay_str, shm_id_StatsSim_str, NULL);
                errExit("[DIRECTOR] execl failure!");
                break;

            default:
                pid_array[j] = Lavoratore;
                opUsrPid[k] = Lavoratore;
                k++;
                j++;
                break;

        }
    }

    //-------------------Utenti------------------
    for(int i = 0; i < shared_memory->NOF_USERS; i++) {
        pid_t Utente = fork();
        switch(Utente) {
            case -1:
                errExit("[DIRECTOR] utente process error!");
                break;
            
            case 0:
                char I[64];
                sprintf(I, "%d", i);
                execl("./utente", "./utente", semWaitInit_str, shm_id_str, I, shm_id_StatsDay_str, shm_id_StatsSim_str, NULL);
                errExit("[DIRECTOR] execl failure!");
                break;

            default:
                opUsrPid[k] = Utente;
                pid_array[j] = Utente;
                j++;
                k++;
                break;
        }
    }

    //aspetta inizializzazione FIGLI
    for(int i=0; i<shared_memory->NOF_USERS; i++){
        sem_op(semWaitInit, 3, -1);
    }
    puts("[DIRECTOR] Inizializzazione USER Terminata");

    //aspetta inizializzazione EROGATORE
    sem_op(semWaitInit, 0, -1);
    puts("[DIRECTOR] Inizializzazione EROGATORE Terminata");

    //aspetta inizializzazione LAVORATORI
    for(int i=0; i<shared_memory->NOF_WORKERS; i++){
        sem_op(semWaitInit, 2, -1);
    }
    printf("\033[1;32m[DIRECTOR] Inizializzazione LAVORATORI Terminata\033[0m\n");

    statsSim->utenti_serviti_tot = 0;
    statsSim->tempo_attesa_utenti_tot = 0;
    statsSim->operatori_attivi_tot = 0;
    statsSim->pause_tot = 0;

    for (int i = 0; i < NUM_SERVIZI; i++) {
        statsSim->servizi_erogati_tot[i] = 0;
        statsSim->servizi_non_erogati_tot[i] = 0;
        statsSim->servizi_non_erogati_media_giorno[i] = 0;
        statsSim->tempo_erogazione_servizi_tot[i] = 0;
    }

    for (int i = 0; i < MAX_SPORTELLI; i++) 
        statsSim->rapporto_op_sportelli_media[i] = 0;


    int count_sportelli[NUM_SERVIZI] = {0};
    int count_operatori[NUM_SERVIZI] = {0};

    int tot_sportelli[NUM_SERVIZI] = {0};
    int tot_operatori[NUM_SERVIZI] = {0};

    float userAvgTot = 0;
    
    puts("\nSESSIONE INIZIATA");

    FILE *csv = fopen("../data/statsSim.csv", "w");
    if (csv == NULL) {
        perror("[DIRECTOR] file error!");
        exit(1);
    }

    fprintf(csv, "Giorno;Utenti serviti;Media Utenti serviti;");
    fprintf(csv, "Tot Servizio 1 erogati;Tot Servizio 2 erogati;Tot Servizio 3 erogati;Tot Servizio 4 erogati;Tot Servizio 5 erogati;Tot Servizio 6 erogati;");
    fprintf(csv, "Tot Servizio 1 non erogati;Tot Servizio 2 non erogati;Tot Servizio 3 non erogati;Tot Servizio 4 non erogati;Tot Servizio 5 non erogati;Tot Servizio 6 non erogati;");
    fprintf(csv, "Tempo medio attesa utenti (min);");
    fprintf(csv, "Tempo erogazione servizio 1;Tempo erogazione servizio 2;Tempo erogazione servizio 3;Tempo erogazione servizio 4;Tempo erogazione servizio 5;Tempo erogazione servizio 6;");
    fprintf(csv, "Media tempo erogazione servizio 1;Media tempo erogazione servizio 2;Media tempo erogazione servizio 3;Media tempo erogazione servizio 4;Media tempo erogazione servizio 5;Media tempo erogazione servizio 6;");
    fprintf(csv, "Operatori attivi;Pause effettuate al gg;");
    fprintf(csv, "Rapporto Operatori/Sportelli(1);Rapporto Operatori/Sportelli(2);Rapporto Operatori/Sportelli(3);Rapporto Operatori/Sportelli(4);Rapporto Operatori/Sportelli(5);Rapporto Operatori/Sportelli(6)\n");
    
    int giorno;
    // creare un semaforo mutex giornata attiva
    for (giorno = 0; giorno < shared_memory->SIM_DURATION; giorno++) {
        // resetta i valori giornalieri di smStats
        setValues(statsDay, count_sportelli, count_operatori);

        printf("\033[1;32m\n[DIRECTOR] Giorno %d iniziato.\033[0m\n", giorno + 1);

        // aggiorna il semaforo per indicare che la giornata è iniziata e waita
        semctl(semWaitInit, 8, SETVAL, 1+ shared_memory->NOF_WORKERS + shared_memory->NOF_USERS); //reimposta il semaforo fine giornata a 1 (giornata non finita)

        // reimposta i semafori
        for(int i=0; i<shared_memory->NOF_USERS; i++)
            sem_op(semWaitInit, 7, 1);
    
        sem_op(semWaitInit, 4, 1);
    
        for(int i=0; i<shared_memory->NOF_WORKERS; i++)
            sem_op(semWaitInit, 6, 1);

        nanosleep((const struct timespec[]){{0, 60 * 8 * shared_memory->N_NANO_SECS}}, NULL);

        // aggiorna variabile per dire fine giornata

        semctl(semWaitInit, 8, SETVAL, 0); //reimposta il semaforo fine giornata a 0 (giornata finita)

        //Svuoto coda degli sportelli
        int result;
        while ((result = msgrcv(msgId, &message, sizeof(message.mtext), 0, IPC_NOWAIT)) != -1) {
            printf("SPORTELLO TIPO %d RIMOSSO\n", message.mtext.tipoLavoro);
        }

        shared_memory->DAYS_LEFT--;
        printf("\033[1;32m[DIRECTOR] Giorni rimanenti: %d\033[0m\n", shared_memory->DAYS_LEFT);
        
        puts("[DIRECTOR] Fine giornata.");

        // Clear user-Erogatore message queue
        key_t msgkeyErog = ftok("/tmp", 'U');
        int msgidErog = msgget(msgkeyErog, 0666 | IPC_CREAT);
        
        WorkerMsg wsmg;
        while ((result = msgrcv(msgidErog, &wsmg, sizeof(WorkerMsg) - sizeof(long), 0, IPC_NOWAIT)) != -1) {}

        // Clear user-worker message queue
        key_t msgkeyOp = ftok("/tmp", 'V');
        int msgidOp = msgget(msgkeyOp, 0666 | IPC_CREAT);
        
        
        while ((result = msgrcv(msgidOp, &wsmg, sizeof(WorkerMsg) - sizeof(long), 0, IPC_NOWAIT)) != -1) {}

        //Operatore esce da msgRcv per sportelli
        if(shared_memory->DAYS_LEFT != 0) {
            for (int i = 0; i < shared_memory->NOF_WORKERS + shared_memory->NOF_USERS; i++) {
                if (kill(opUsrPid[i], 0) == 0) {
                    if (kill(opUsrPid[i], SIGUSR2) == -1) {
                        perror("[DIRECTOR] errore invio segnale");
                    }
                    else printf("\033[1;32m[DIRECTOR] Figlio segnale uscita PID: %d\033[0m\n", opUsrPid[i]);
                }
            }
            msg_enqueue(shared_memory, msgId, &message);
        }

        nanosleep((const struct timespec[]){{0, 60 * 16 * shared_memory->N_NANO_SECS}}, NULL);
        
        // Fai esplodere la sim se user falliti > explode_threshold
        int notService = 0;
        for (int i = 0; i < NUM_SERVIZI; i++)
            notService += statsDay->servizi_non_erogati[i];

        if (notService > shared_memory->EXPLODE_THRESHOLD) 
            *explode = true;

        if(*explode || shared_memory->DAYS_LEFT == 0) {
            puts("\n[DIRECTOR] Giorni finiti, chiudo tutto");
            // signal ia figli per interrompere attesa su nuovo giorno
            for (int i = 0; i < shared_memory->NOF_USERS + shared_memory->NOF_WORKERS + 1; i++) {
                if (kill(pid_array[i], 0) == 0) {
                    if (kill(pid_array[i], SIGUSR1) == -1) {
                        perror("[DIRECTOR] Errore nell'invio del segnale");
                    }
                    else printf("\033[1;32m[DIRECTOR] Segnale inviato a PID: %d\033[0m\n", pid_array[i]);
                } 
            }
            break;
        }

        // Stampa statistiche giornaliere
        for (int i = 0; i < shared_memory->NOF_WORKER_SEATS; i++) {
            count_sportelli[shared_memory->sportelli[i] - 1]++;
            tot_sportelli[shared_memory->sportelli[i] - 1]++;
            count_operatori[shared_memory->lavoratori[i] - 1]++;
            tot_operatori[shared_memory->lavoratori[i] - 1]++;
        }
        
        for(int i = 0; i < shared_memory->NOF_WORKER_SEATS; i++) {
            printf("tot_sportelli: %d\n", tot_sportelli[i]);
            printf("tot_operatori: %d\n", tot_operatori[i]);
            puts("");
        }


        int gg = giorno+1;

        printStatsDay(shared_memory, statsDay, csv, count_sportelli, count_operatori, gg);
        puts("");
        
        printStatsSim(statsSim, csv, shared_memory, tot_operatori, tot_sportelli, gg);
        puts("");
    }


    while (1) {
        pid_t pid = waitpid(-1, NULL, 0); // Aspetta qualunque figlio
        if (pid == -1) {
            if (errno == ECHILD) {
                // Non ci sono più figli
                printf("\033[1;32m[DIRECTOR] Tutti i figli sono terminati.\033[0m\n");
                break;
            } else {
                perror("[DIRECTOR] Errore durante waitpid");
                break;
            }
        } else {
            //printf("Figlio terminato, PID: %d\n", pid);
        }
    }
    
    // Deallocazione della coda di messaggi
    if (msgctl(msgId, IPC_RMID, NULL) == -1) {
        perror("[DIRECTOR] Errore nella deallocazione della coda di messaggi");
        exit(EXIT_FAILURE);
    }

    // Stampa statistiche giornaliere
    for (int i = 0; i < shared_memory->NOF_WORKER_SEATS; i++) {
        count_sportelli[shared_memory->sportelli[i] - 1]++;
        tot_sportelli[shared_memory->sportelli[i] - 1]++;
        count_operatori[shared_memory->lavoratori[i] - 1]++;
        tot_operatori[shared_memory->lavoratori[i] - 1]++;
    }
    
    for(int i = 0; i < shared_memory->NOF_WORKER_SEATS; i++) {
        printf("tot_sportelli: %d\n", tot_sportelli[i]);
        printf("tot_operatori: %d\n", tot_operatori[i]);
        puts("");
    }

    puts("");
    printStatsDay(shared_memory, statsDay, csv, count_sportelli, count_operatori, giorno+1);
    puts("\n");
    
    printStatsSim(statsSim, csv, shared_memory, count_operatori, count_sportelli, shared_memory->DAYS_LEFT);
    fclose(csv);
    
    puts("");
    printf("\033[1;32m[DIRECTOR] direttore finito\033[0m\n");
}

void load_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[50];
        int value;
        if (sscanf(line, "%[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "NOF_WORKER_SEATS") == 0) {
                config->NOF_WORKER_SEATS = value;
            } else if (strcmp(key, "NOF_WORKERS") == 0) {
                config->NOF_WORKERS = value;
            } else if (strcmp(key, "NOF_USERS") == 0) {
                config->NOF_USERS = value;
            } else if (strcmp(key, "SIM_DURATION") == 0) {
                config->SIM_DURATION = value;
            } else if (strcmp(key, "N_NANO_SECS") == 0) {
                config->N_NANO_SECS = value;
            } else if (strcmp(key, "NOF_PAUSE") == 0) {
                config->NOF_PAUSE = value;
            } else if (strcmp(key, "EXPLODE_THRESHOLD") == 0) {
                config->EXPLODE_THRESHOLD = value;
            } else if (strcmp(key, "P_PAUSE") == 0) {
                config->P_PAUSE = value;
            } 
        }
    }

    fclose(file);
    config->DAYS_LEFT = config->SIM_DURATION;
}

int main(int argc, char *argv[]) {
    // Caricamento configurazione
    Config config;
    load_config("../src/post_office.conf", &config);

    // Crea memoria condivisa per passare le config ai figli
    int shm_id = shmget(IPC_PRIVATE, sizeof(Config), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // Crea memoria condivisa per tener traccia delle statistiche
    int shm_id_StatsDay = shmget(IPC_PRIVATE, sizeof(StatsDay), IPC_CREAT | 0666);
    if (shm_id_StatsDay == -1) {
        perror("Errore nell'attacco alla memoria condivisa (StatsDay)");
        exit(EXIT_FAILURE);
    }
    int shm_id_StatsSim = shmget(IPC_PRIVATE, sizeof(StatsSim), IPC_CREAT | 0666);
    if (shm_id_StatsSim == -1) {
        perror("Errore nell'attacco alla memoria condivisa (StatsSim)");
        exit(EXIT_FAILURE);
    }

    // Attacca la memoria condivisa (Config)
    Config *shared_memory = (Config *)shmat(shm_id, NULL, 0);
    if (shared_memory == (Config *)-1) {
        perror("Errore nell'attacco alla memoria condivisa (Config)");
        exit(EXIT_FAILURE);
    }
    *shared_memory = config;

    // Attacca la memoria condivisa (Stats)
    StatsDay *statsDay = (StatsDay *)shmat(shm_id_StatsDay, NULL, 0);
    if (statsDay == (void *)-1) {
        perror("Errore nell'attacco alla memoria condivisa (StatsDay)");
        exit(EXIT_FAILURE);
    }
    StatsSim *statsSim = (StatsSim *)shmat(shm_id_StatsSim, NULL, 0);
    if (statsSim == (void *)-1) {
        perror("Errore nell'attacco alla memoria condivisa (StatsSim)");
        exit(EXIT_FAILURE);
    }

    // Crea semaforo per l'attesa dell'inizializzazione dei figli
    int semWaitInit = semget(IPC_PRIVATE, 9, IPC_CREAT | 0666);
    if (semWaitInit == -1) {
        perror("semget failed");
        exit(1);
    }
    for(int i=0; i<9; i++){
        if(semctl(semWaitInit, i, SETVAL, 0) == -1) {
            perror("semctl failed");
            exit(1);
        }
    }

    signal(SIGUSR1, signal_handler);

    bool explode = false;
    // Avvia simulazione
    direttore(semWaitInit, shm_id, shm_id_StatsDay, shm_id_StatsSim, shared_memory, statsDay, statsSim, &explode);

    // Detach e cleanup
    if (shmdt(shared_memory) == -1) {
        perror("[DIRECTOR] shmdt error!");
        exit(EXIT_FAILURE);
    }
    if (shmdt(statsDay) == -1) {
        perror("[DIRECTOR] shmdt error! (StatsDay)");
        exit(EXIT_FAILURE);
    }
    if (shmdt(statsSim) == -1) {
        perror("[DIRECTOR] shmdt error! (StatsSim)");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }
    if (shmctl(shm_id_StatsDay, IPC_RMID, NULL) == -1) {
        perror("shmctl failed (StatsDay)");
        exit(1);
    }
    if (shmctl(shm_id_StatsSim, IPC_RMID, NULL) == -1) {
        perror("shmctl failed (StatsSim)");
        exit(1);
    }
    if(semctl(semWaitInit, 0, IPC_RMID) == -1) {
        perror("semctl failed");
        exit(1);
    }
    puts("");
    printf("\033[1;32m\033[1mSIMULAZIONE TERMINATA!\033[0m\n");

    // Controllare se viene raggiunto l'explode TrashHold
    if (!explode) 
        puts("Simulazione terminata correttamente!(TIMEOUT)");
    else
        puts("Simulazione terminata prematuramente (EXPLODE)");
        

    exit(0);
}