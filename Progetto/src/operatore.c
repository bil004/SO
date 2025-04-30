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

#include "../include/config.h"

#define NOF_PAUSE 4

typedef struct sportello{
    int tipoLavoro;
} Sportello, *SportelloPtr;

struct msgbuf {
    long mtype;
    SportelloPtr mtext;
};

void sem_op(int semid, int sem_num, int sem_op)
{
    struct sembuf operazione;
    operazione.sem_num = sem_num; // Indice del semaforo nel set
    operazione.sem_op = sem_op;   // Operazione (incremento/decremento/attesa)
    operazione.sem_flg = 0;       // Nessuna flag

    if (semop(semid, &operazione, 1) == -1)
    {
        perror("Errore nell'operazione sul semaforo");
        exit(EXIT_FAILURE);
    }
}

void cicloOperativo(int semLavoratore, int i){
    
    printf("Lavoratore %d sta lavorando\n", i);
    while(1){
        int valoreSemaforo = semctl(semLavoratore, 8, GETVAL);
        if (valoreSemaforo > 0) {
            // Semaforo impostato dal processo padre, termina il ciclo
            break;
        }

        //printf("ciao %d\n", i);
    }
    printf("Lavoratore %d ha finito di lavorare\n", i);
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        fprintf(stderr, "Incorrect number of arg\n");
        exit(1);
    }

    srand((unsigned)time(0));

    int semLavoratore = atoi(argv[1]);
    int shmId = atoi(argv[2]); // ID della memoria condivisa passato come argomento
    int i = atoi(argv[3]); // num lavoratore
    int msgId = atoi(argv[4]); // id coda messaggi degli sportelli
    int tipoLavoro = atoi(argv[5]); //tipo di lavoro che sa svolgere l'operatore

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

    while(semctl(semLavoratore, 8, GETVAL) == 0 && shared_memory->DAYS_LEFT > 0) {

        printf("Simulazione iniziata, lavoratore: %d tipoLavoro: %d\n", i, tipoLavoro);

        struct msgbuf message;
        bool msgFound = false;
        do{
            if(msgrcv(msgId, &message, sizeof(message.mtext), tipoLavoro, IPC_NOWAIT) == -1){
                msgFound = false;
            }else{
                msgFound = true;
            }
        }while(!msgFound && semctl(semLavoratore, 8, GETVAL) == 0);
        //printf("Lavoratore %d preso Sportello per lavoro %d\n", i, tipoLavoro);
        if(msgFound){
            cicloOperativo(semLavoratore, i);
        }

        //Da qui la giornata è finita
        
        msgsnd(msgId, &message, sizeof(message.mtext), IPC_NOWAIT);

        //ascolta sem 8 se giorni mancanti > 0;
        if(shared_memory->DAYS_LEFT > 0){
            sem_op(semLavoratore, 8, 0);
        }
    }



/*
    int pCount = 0;    
    // COLLEGAMENTO CON SPORTELLO

    // Simula il lavoro delle varie giornate
    for (int j = 0; j < 8; j++)
    {
        sleep(1);

        int boh = pCount;
        if (pCount < NOF_PAUSE && rand() % 4 == 0)
        {
            printf("Lavoratore %d sta facendo una pausa\n", i);

            // Termina il servizio del cliente corrente
            sleep(1);

            // Libera lo sportello
            sem_op(semLavoratore, 3, 1);
            printf("Lavoratore %d ha lasciato lo sportello per una pausa\n", i);

            // Attende prima di tornare a lavoro
            sleep(2);
            sem_op(semLavoratore, 3, -1);
            printf("Lavoratore %d è tornato al lavoro\n", i);
        }
    }*/

    // Detach shared memory
    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt error!");
        exit(EXIT_FAILURE);
    }

    // printf("Running operatore %d\n", i);

    exit(0);
}



/*
#define NOF_PAUSE 4

int main(int argc, char *argv[]) {
    while (1) {
        // Compete per uno sportello libero
        sem_op(semLavoratore, 3, -1); // Decrementa il semaforo degli sportelli liberi
        printf("Lavoratore %d ha occupato uno sportello\n", i);

        // Simula il lavoro fino alla fine della giornata
        for (int j = 0; j < 8; j++) { // Simula 8 ore lavorative
            sleep(1); // Simula un'ora di lavoro

            // Decide casualmente se fare una pausa
            if (pause_count < NOF_PAUSE && rand() % 4 == 0) {
                printf("Lavoratore %d sta facendo una pausa\n", i);
                pause_count++;

                // Termina il servizio del cliente corrente
                sleep(1); // Simula il completamento del servizio

                // Libera lo sportello
                sem_op(semLavoratore, 3, 1); // Incrementa il semaforo degli sportelli liberi
                printf("Lavoratore %d ha lasciato lo sportello per una pausa\n", i);

                // Aggiorna le statistiche (simulato)
                shared_data[i]++; // Aggiorna una statistica fittizia

                // Attende prima di tornare al lavoro
                sleep(2);
                sem_op(semLavoratore, 3, -1); // Occupa di nuovo uno sportello
                printf("Lavoratore %d è tornato al lavoro\n", i);
            }
        }

        // Fine giornata lavorativa
        printf("Lavoratore %d termina la giornata\n", i);

        // Libera lo sportello
        sem_op(semLavoratore, 3, 1); // Incrementa il semaforo degli sportelli liberi
        break;
    }

    // Distacca la memoria condivisa
    if (shmdt(shared_data) == -1) {
        perror("shmdt error!");
        exit(EXIT_FAILURE);
    }

    exit(0);
}
*/