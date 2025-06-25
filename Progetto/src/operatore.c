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
#include "../include/functions.h"

volatile sig_atomic_t terminate = 0;
volatile sig_atomic_t nextDay = 0;

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        terminate = 1; // Imposta il flag per terminare il processo
    }
    if (sig == SIGUSR2){
        nextDay = 1;
    }
}

void sem_op(int semid, int sem_num, int sem_op) {
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1) {
        if (errno == EINTR)
            printf("\033[1;34m\033[1m[WORKER] Processo %d: Semaforo interrotto da segnale.\033[0m\n", getpid());
        else{
            perror("[WORKER] Errore nel semaforo: ");
            exit(EXIT_FAILURE);
        }
    }
}

void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause) {
    printf("\033[0;34m[WORKER] %d sta lavorando\033[0m\n", i);
    while (1) {
        if (terminate) break;
        int fineGG = semctl(semLavoratore, 8, GETVAL);
        if (fineGG == 0) {
            break;
        }
        srand(time(0)^getpid());

        if (nofPause != 0 && (rand()%10) > 6) {
            printf("\033[1;34m\033[1m[WORKER] %d fa pausa!\033[0m\n", getpid());
            nofPause--;
            break;
        }

        // Worker esegue le sue operazioni
        WorkerMsg response;
        int ret = msgrcv(msgid, &response, sizeof(WorkerMsg) - sizeof(long), tipoLavoro, 0);
        if (ret == -1) {
            if (errno == EINTR) {
                if (terminate) {
                    printf("\033[1;34m\033[1m[WORKER] msgrcv interrotto da segnale, termino.\033[0m\n");
                    nextDay = 0;
                    break;
                }
                if (nextDay) {
                    nextDay = 0;
                    break;
                }
            }
            if (errno == ENOMSG) {
                // Nessun lavoro disponibile
                printf("\033[1;34m\033[1m[WORKER] Lavoratore %d non ha trovato un lavoro... \033[0m\n", i); 
                nextDay = 0;
                continue;
            }
            // altri errori
            perror("[WORKER] msgrcv error");
            nextDay = 0;
            exit(1);
        }

        if (terminate) break;

        if (response.pid > 0)
            printf("\033[1;34m\033[1m[WORKER] Lavoratore %d svolge il lavoro di %d...\033[0m\n", i, response.pid);

        int tmpLav = tempario[response.mtype];
        int tmpMin = tmpLav/2;
        srand(time(0)^getpid());
        int time = (rand()%tmpLav) + tmpMin;
        nanosleep((const struct timespec[]){{0, time * nanoSec}}, NULL);

        if (terminate) break;

        WorkerMsg end = {response.pid, 0};
        msgsnd(msgid, &end, sizeof(WorkerMsg) - sizeof(long), 0);
    }
    printf("\033[1;34m\033[1m[WORKER] Lavoratore %d ha finito di lavorare\033[0m\n", i);
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "[WORKER] Incorrect number of arg\n");
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
    if (shared_memory == (Config *)-1) {
        perror("[WORKER] Errore nell'attacco alla memoria condivisa");
        exit(EXIT_FAILURE);
    }

    int nofPause = shared_memory->NOF_PAUSE;

    // incrementa semaforo per informare il padre
    sem_op(semLavoratore, 2, 1);

    printf("\033[0;34m[WORKER] %d inizializzazione terminata\033[0m\n", i);

    // Attende la risposta del padre
    sem_op(semLavoratore, 6, -1);

    // -------------- CONTROLLO SE IL SEMAFORO GIORNATA è ATTIVO (SE LA GG è FINITA O NO) ---------------

    /*  while(1) finché non trova giornata finita
        se operatore serve un user, quando finisce controlla il semaforo e, se indica che la gg è finita,
        conta la gestione del ticket come "non effettuata" */

    while (!terminate) {
        puts("\n");
        puts("\033[0;34m[WORKER] Operatore Sleeping till day starts\033[0m");
        sem_op(semLavoratore, 8, -1);

        puts("\033[0;34m[WORKER] Lavoratore è uscito dal semaforo.\033[0m");

        // Stampa debug ogni giorno, anche se non trova sportello
        printf("\033[0;34m[WORKER] Lavoratore %d, PID: %d, tipoLavoro=%d\033[0m\n", i, getpid(), tipoLavoro);

        if (terminate || shared_memory->DAYS_LEFT <= 0) {
            printf("\033[0;34m[WORKER] Processo %d: Terminazione richiesta.\033[0m\n", getpid());
            break;
        }
        
        struct msgbuf message;
        int ret = msgrcv(msgId, &message, sizeof(Sportello), tipoLavoro, 0);

        if (ret == -1) {
            if (errno == ENOMSG) {
                // Nessuno sportello disponibile per questo operatore oggi
                printf("\033[1;34m\033[1m[WORKER] Lavoratore %d: Nessuno sportello disponibile oggi per tipoLavoro=%d\033[0m\n", getpid(), tipoLavoro);
                // Attende la fine della giornata
                sem_op(semLavoratore, 8, 0);
                continue;
            }
            if (errno == EINTR && terminate) {
                printf("\033[1;34m\033[1m[WORKER] Processo %d: msgrcv interrotto da segnale.\033[0m\n", getpid());
                break;
            }
            if (errno == EINTR && nextDay) {
                printf("\033[1;34m\033[1m[WORKER] Processo %d: msgrcv interrotto. Si passa al gg successivo.\033[0m\n", getpid());
                nextDay = 0;
                // non fare nulla, riprova nel prossimo ciclo
            }
            
            continue;
        }

        printf("\033[1;34m\033[1m[WORKER] Lavoratore %d occupa lo sportello tipo %ld, %d...\033[0m\n", i, message.mtype, message.mtext.tipoLavoro);
        // Messaggio ricevuto, lavora
        key_t msgkeyOp = ftok("/tmp", 'V');
        int msgidW = msgget(msgkeyOp, 0666 | IPC_CREAT);

        // ricezione utente
        cicloOperativo(semLavoratore, i, tipoLavoro, msgidW, shared_memory->N_NANO_SECS, nofPause);

        msgsnd(msgId, &message, sizeof(Sportello), 0);

        // Attende la fine della giornata prima di ripartire
        sem_op(semLavoratore, 8, 0);
        // Aggiorna statistiche
    }

    // Detach shared memory
    if (shmdt(shared_memory) == -1) {
        perror("[WORKER] shmdt error!");
        exit(EXIT_FAILURE);
    }

    printf("\033[1;34m\033[1m[WORKER] operatore %d finito\033[0m\n", getpid());

    exit(0);
}
