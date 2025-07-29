#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include "../include/config.h"
#include "../include/functions.h"

volatile sig_atomic_t terminate = 0;

void signal_handler(int sig) {
    if (sig == SIGUSR1)
        terminate = 1;
}

int main(int argc, char *argv[]) {
    signal(SIGUSR1, signal_handler);

    // Accesso alle IPC tramite ftok
    key_t k_conf = ftok("/tmp", 'C');
    key_t k_statsD = ftok("/tmp", 'D');
    key_t k_statsS = ftok("/tmp", 'S');
    key_t msgkeyEr = ftok("/tmp", 'U');
    key_t msgkeyOp = ftok("/tmp", 'V');

    int shm_conf = shmget(k_conf, sizeof(Config), 0666);
    int shm_day = shmget(k_statsD, sizeof(StatsDay), 0666);
    int shm_sim = shmget(k_statsS, sizeof(StatsSim), 0666);
    if (shm_conf == -1 || shm_day == -1 || shm_sim == -1) {
        perror("[DYN_USER] Errore shmget");
        exit(1);
    }

    Config *shared_memory = shmat(shm_conf, NULL, 0);
    StatsDay *statsDay = shmat(shm_day, NULL, 0);
    StatsSim *statsSim = shmat(shm_sim, NULL, 0);

    printf("[DYN_USER] %d inizializzato dinamicamente\n", getpid());

    srand(time(NULL) ^ getpid());

    while (!terminate && shared_memory->DAYS_LEFT > 0) {
        sleep(1); // Aspetta un po' tra le richieste (semplificato)

        int service = (rand() % 6) + 1;

        bool foundS = false, foundW = false;
        for (int i = 0; i < shared_memory->NOF_WORKER_SEATS; i++)
            if (shared_memory->sportelli[i] == service) foundS = true;
        for (int i = 0; i < shared_memory->NOF_WORKERS; i++)
            if (shared_memory->lavoratori[i] == service) foundW = true;

        if (!(foundS && foundW)) {
            printf("[DYN_USER] %d: Servizio %d non disponibile\n", getpid(), service);
            continue;
        }

        // Richiesta ticket
        int msgidEr = msgget(msgkeyEr, 0666);
        TicketMsg req = {1, service, 0, getpid()};
        msgsnd(msgidEr, &req, sizeof(TicketMsg) - sizeof(long), 0);

        TicketMsg response;
        msgrcv(msgidEr, &response, sizeof(TicketMsg) - sizeof(long), getpid(), 0);
        printf("[DYN_USER] %d ha ricevuto ticket %d\n", getpid(), response.ticket);

        int attesa = (rand() % 180) * shared_memory->N_NANO_SECS;
        nanosleep((const struct timespec[]){{0, attesa}}, NULL);

        // Invia messaggio all'operatore
        int msgidOp = msgget(msgkeyOp, 0666);
        WorkerMsg w = {service, getpid()};
        clock_t start = clock();
        msgsnd(msgidOp, &w, sizeof(WorkerMsg) - sizeof(long), 0);

        WorkerMsg wResp;
        msgrcv(msgidOp, &wResp, sizeof(WorkerMsg) - sizeof(long), getpid(), 0);
        clock_t end = clock();

        float time_ms = ((float)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
        statsDay->tempo_attesa_utenti += time_ms;
        statsSim->tempo_attesa_utenti_tot += time_ms;

        statsDay->servizi_erogati[service - 1]++;
        statsSim->servizi_erogati_tot[service - 1]++;
        statsDay->utenti_serviti++;
        statsSim->utenti_serviti_tot++;

        printf("[DYN_USER] %d ha completato il servizio %d\n", getpid(), service);
    }

    printf("[DYN_USER] %d finito\n", getpid());
    return 0;
}
