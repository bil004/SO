#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include"../include/config.h"

#define CONFFILE "../src/post_office.conf"

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
            }
        }
    }

    fclose(file);
    config->DAYS_LEFT = config->SIM_DURATION;
    config->DAY = 0; // 0-inizio giornata    1-fine giornata
}

int main() {
    // Crea memoria condivisa per passare le config ai figli
    int shm_id = shmget(IPC_PRIVATE, sizeof(Config), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // Attacca la memoria condivisa
    Config *shared_memory = (Config *)shmat(shm_id, NULL, 0);
    if (shared_memory == (Config *)-1) {
        perror("Errore nell'attacco alla memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // Caricamento dei valori nella struct condivisa
    load_config(CONFFILE, shared_memory);

    // Crea semaforo per l'attesa dell'inizializzazione dei figli
    int semWaitInit = semget(IPC_PRIVATE, 9, IPC_CREAT | 0666);
    if (semWaitInit == -1) {
        perror("semget failed");
        exit(1);
    }
    /*semWaitInit:
        0 = Erog
        1 = 
        2 = Lavoratori
        3 = Utenti
        4 = ErogPadre
        5 = 
        6 = LavoratoriPadre
        7 = UtentiPadre
        8 = giornata
    */
    // Inizializza il semaforo per l'attesa dell'inizializzazione dei figli
    for(int i=0; i<9; i++){
        if(semctl(semWaitInit, i, SETVAL, 0) == -1) {
            perror("semctl failed");
            exit(1);
        }
    }

    // Avvia il direttore usando exec
    pid_t pid = fork();
    if (pid == 0) {
        pid = getpid();
        printf("\033[1;32m\033[1mDirettore con pid:%d\033[0m\n", pid);
        char shm_id_str[10], semWaitInit_str[10];
        snprintf(shm_id_str, 10, "%d", shm_id);
        snprintf(semWaitInit_str, 10, "%d", semWaitInit);
        execl("./direttore", "./direttore", semWaitInit_str, shm_id_str, CONFFILE, NULL);
        perror("execl");
        exit(1);
    }


    // Attendi che il direttore termini
    if (waitpid(pid, NULL, 0) == -1) {
        perror("Errore durante waitpid");
        exit(1);
    }

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    // Rimuovi memoria condivisa e semafori
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    if(semctl(semWaitInit, 0, IPC_RMID) == -1) {
        perror("semctl failed");
        exit(1);
    }
    
    puts("");
    printf("\033[1;32m\033[1mSIMULAZIONE TERMINATA!\033[0m\n");
    return 0;
}