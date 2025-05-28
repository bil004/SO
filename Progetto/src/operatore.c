#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>

#include "../include/config.h"

#define NOF_PAUSE 4
volatile sig_atomic_t terminate = 0;
volatile sig_atomic_t nextDay = 0;

typedef struct sportello {
    int tipoLavoro;
} Sportello, *SportelloPtr;

struct msgbuf {
    long mtype;
    SportelloPtr mtext;
};

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        terminate = 1; // Imposta il flag per terminare il processo
    }
    if (sig == SIGUSR2){
        nextDay = 1;
    }
}

int sem_op(int semid, int sem_num, int sem_op) {
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1) {
        if (errno == EINTR)
            printf("Processo %d: Semaforo interrotto da segnale.\n", getpid());
        else{
            perror("Errore nel semaforo: ");
            exit(EXIT_FAILURE);
        }
    }
}

void cicloOperativo(int semLavoratore, int i) {

    printf("Lavoratore %d sta lavorando\n", i);
    while (1) {
        int valoreSemaforo = semctl(semLavoratore, 8, GETVAL);
        if (valoreSemaforo == 0){
            // Semaforo impostato dal processo padre, termina il ciclo
            break;
        }
        // fa un lavoro
        // printf("ciao %d\n", i);
    }
    printf("Lavoratore %d ha finito di lavorare\n", i);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Incorrect number of arg\n");
        exit(1);
    }

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    srand((unsigned)time(0));

    int semLavoratore = atoi(argv[1]);
    int shmId = atoi(argv[2]);      // ID della memoria condivisa passato come argomento
    int i = atoi(argv[3]);          // num lavoratore
    int msgId = atoi(argv[4]);      // id coda messaggi degli sportelli
    int tipoLavoro = atoi(argv[5]); // tipo di lavoro che sa svolgere l'operatore

    // Attacca la memoria condivisa
    Config *shared_memory = (Config *)shmat(shmId, NULL, 0);
    if (shared_memory == (Config *)-1)
    {
        perror("Errore nell'attacco alla memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // incrementa semaforo per informare il padre
    sem_op(semLavoratore, 2, 1);

    printf("Inizializzazione Terminata Lavoratore %d\n", i);

    // Attende la risposta del padre
    sem_op(semLavoratore, 6, -1);

    // -------------- CONTROLLO SE IL SEMAFORO GIORNATA è ATTIVO (SE LA GG è FINITA O NO) ---------------

    /*  while(1) finché non trova giornata finita
        se operatore serve un user, quando finisce controlla il semaforo e, se indica che la gg è finita,
        conta la gestione del ticket come "non effettuata" */

    while (!terminate) {
        puts("Operatore Sleeping till day starts");
        sem_op(semLavoratore, 8, -1);

        // Stampa debug ogni giorno, anche se non trova sportello
        printf("DEBUG: Operatore %d, DAYS_LEFT=%d, terminate=%d, tipoLavoro=%d\n", getpid(), shared_memory->DAYS_LEFT, terminate, tipoLavoro);

        if (terminate || shared_memory->DAYS_LEFT <= 0) {
            printf("Processo %d: Terminazione richiesta.\n", getpid());
            break;
        }
        struct msgbuf message;
        int ret = msgrcv(msgId, &message, sizeof(message.mtext), tipoLavoro, 0);
        if (ret == -1) {
            if (errno == ENOMSG) {
                // Nessuno sportello disponibile per questo operatore oggi
                printf("Operatore %d: Nessuno sportello disponibile oggi per tipoLavoro=%d\n", getpid(), tipoLavoro);
                continue;
            }
            if (errno == EINTR && terminate) {
                printf("Processo %d: msgrcv interrotto da segnale.\n", getpid());
                break;
            }
            if (errno == EINTR && nextDay) {
                printf("Processo %d: msgrcv interrotto. Si passa al gg successivo.\n", getpid());
                nextDay = 0;
            }
            
            continue;
        }
        // Messaggio ricevuto, lavora
        cicloOperativo(semLavoratore, i);
        msgsnd(msgId, &message, sizeof(message.mtext), 0);
        // Aggiorna statistiche
    }

    // Detach shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt error!");
        exit(EXIT_FAILURE);
    }

    printf("operatore %d finito\n", getpid());

    exit(0);
}