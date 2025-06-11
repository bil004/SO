#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>

#include "../include/config.h"

typedef struct ticket_msg {
    long mtype;      // Tipo messaggio: 1 per richiesta, PID utente per risposta
    int servizio;    // Tipo di servizio richiesto
    int ticket;      // Numero del ticket (solo nella risposta)
    pid_t pid;       // PID dell'utente (solo nella richiesta)
} TicketMsg;

volatile sig_atomic_t terminate = 0;

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        terminate = 1; // Imposta il flag per terminare il processo
    }
}

int sem_op(int semid, int sem_num, int sem_op) {
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1) {
        if (errno == EINTR) {
            printf("Processo %d: Semaforo interrotto da segnale.\n", getpid());
        }
        else{
            perror("Errore nel semaforo");
            exit(EXIT_FAILURE);
        }
    }
}

// devo prendere i valori dal padre e poi inserirli nel ticket

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "[TICKET] Incorrect number of arg\n");
        exit(1);
    }

    signal(SIGUSR1, signal_handler);

    //FINE INIZIALIZZAZIONE
    int semErogatore = atoi(argv[1]);

    //incrementa semaforo per informare il padre
    sem_op(semErogatore, 0, 1);
    printf("\033[1;33m[TICKET] Inizializzazione Erogatore Terminata\033[0m\n");
    //Attende la risposta del padre
    sem_op(semErogatore, 4, -1);
    
    printf("\033[1;33m[TICKET] Running Erogatore TICKET\033[0m\n");

    key_t msgkey = ftok("/tmp", 'U');
    int msgid = msgget(msgkey, 0666 | IPC_CREAT);
    int next_ticket = 1;
    
    while (!terminate) {
        TicketMsg richiesta;
        puts("\033[1;33m[TICKET] mi preparo a ricevere il messaggio\033[0m");
        msgrcv(msgid, &richiesta, sizeof(TicketMsg) - sizeof(long), 1, 0);
        puts("\033[1;33m[TICKET] messaggio ricevuto\033[0m");

        TicketMsg risposta = {richiesta.pid, richiesta.servizio, next_ticket++, richiesta.pid};
        msgsnd(msgid, &risposta, sizeof(TicketMsg) - sizeof(long), 0);
        puts("\033[1;33m[TICKET] messaggio inviato\033[0m");
    }

    exit(0);
}