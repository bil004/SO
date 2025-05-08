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

#include "../include/config.h"
//#include "../include/erogatore_ticket.h"

void sem_op(int semid, int sem_num, int sem_op) {
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1) {
        perror("Errore nell'operazione sul semaforo");
        exit(EXIT_FAILURE);
    }
}

// devo prendere i valori dal padre e poi inserirli nel ticket

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arg\n");
        exit(1);
    }

    //FINE INIZIALIZZAZIONE
    int semErogatore = atoi(argv[1]);

    //incrementa semaforo per informare il padre
    sem_op(semErogatore, 0, 1);
    printf("Inizializzazione Erogatore Terminata\n");
    //Attende la risposta del padre
    sem_op(semErogatore, 4, -1);


    //TicketPtr t = (TicketPtr)malloc(sizeof(Ticket));
    //if (t == NULL) errExit("ticket creation error!");
    printf("Running Erogatore TICKET\n");
    /*
    switch(atoi(argv[2])) {
        case 1:
            t->time = tempario[0];
            t->type = servizio[0];
            break;

        case 2:
            t->time = tempario[1];
            t->type = servizio[1];
            break;

        case 3:
            t->time = tempario[2];
            t->type = servizio[2];
            break;

        case 4:
            t->time = tempario[3];
            t->type = servizio[3];
            break;

        case 5:
            t->time = tempario[4];
            t->type = servizio[4];
            break;

        case 6:
            t->time = tempario[5];
            t->type = servizio[5];
    }
    */

    exit(0);
}