#include <stdio.h>
#include <stdlib.h>
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

void errExit(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void signal_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        perror("Direttore Ucciso");
        exit(EXIT_FAILURE); // Imposta il flag per terminare il processo
    }
}

typedef struct sportello{
    int tipoLavoro;
} Sportello, *SportelloPtr;

struct msgbuf {
    long mtype;
    SportelloPtr mtext;
};

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

void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message){
    
    srand(time(NULL));
    
    for(int i = 0; i< shared_memory->NOF_WORKER_SEATS; i++){
        SportelloPtr sp = (SportelloPtr)malloc(sizeof(Sportello));
        if(sp == NULL) {
            perror("sportello non creato");
            exit(0);
        }
        sp->tipoLavoro = (rand() % 6) + 1; //tipo lavoro da 1 a 6
        shared_memory->sportelli[i] = sp->tipoLavoro;
        
        printf("Sportello %d che offre il servizio: %d \n", i, sp->tipoLavoro);

        
        message->mtype =  sp->tipoLavoro;
        message->mtext = sp;
        
        //invio messaggio sulla coda
        msgsnd(msgId, &message, sizeof(message->mtext), 0);

    }
}

void direttore(char* semWaitInit_str, char* shmid_str, Config* shared_memory){
    pid_t* pid_array = malloc(sizeof(pid_t) * (shared_memory->NOF_USERS + shared_memory->NOF_WORKERS + 1));
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!! Main in altro file, direttore chiamato con execl.., parametri passati nell'argv[] !!!!!!!!!!!!!!!!
    int j = 0;
    //Puntatore a memoria condivisa

    //Creazione processi figli
    //-----------------Erogatore Ticket------------------
    pid_t erogatoreTicket = fork();
    switch(erogatoreTicket) {
        case -1:
            perror("erogatore_ticket not created!");
            exit(EXIT_FAILURE);
            break;

        case 0:
            printf("Erogatore ticket con pid:%d\n", getpid());
            execl("./erogatoreTicket", "./erogatoreTicket", semWaitInit_str, shmid_str, NULL);
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
        perror("Errore coda di messaggi!");
        exit(1);
    }
    struct msgbuf message;
    msg_enqueue(shared_memory, msgId, &message);
    

    //-------------------Lavoratori-----------------
    for(int i = 0; i < shared_memory->NOF_WORKERS; i++) {
        pid_t Lavoratore = fork();
        switch(Lavoratore) {
            case -1:
                errExit("operatore process error!");
                break;

            case 0:
                //printf("sono Worker %d con PID: %d!\n", i, getpid());
                srand(time(NULL)^getpid());
                char I[64];
                char msgId_str[64];
                sprintf(I, "%d", i);
                sprintf(msgId_str, "%d", msgId);
                int tipolavoro = (rand() % 6) + 1;
                char tipoLavoro_str[64];
                sprintf(tipoLavoro_str, "%d", tipolavoro);
                execl("./operatore", "./operatore", semWaitInit_str, shmid_str, I, msgId_str, tipoLavoro_str, NULL);
                errExit("execl failure!");
                break;

            default:
                pid_array[j] = Lavoratore;
                j++;
                break;

        }
    }

    //-------------------Utenti------------------
    for(int i = 0; i < shared_memory->NOF_USERS; i++) {
        pid_t Utente = fork();
        switch(Utente) {
            case -1:
                errExit("utente process error!");
                break;
            
            case 0:
                char I[64];
                sprintf(I, "%d", i);
                execl("./utente", "./utente", semWaitInit_str, shmid_str, I, NULL);
                errExit("execl failure!");
                break;

            default:
                pid_array[j] = Utente;
                j++;
                break;
        }
    }

    //aspetta inizializzazione FIGLI
    for(int i=0; i<shared_memory->NOF_USERS; i++){
        int semWaitInit = atoi(semWaitInit_str);
        sem_op(semWaitInit, 3, -1);
    }
    printf("Inizializzazione FIGLI Terminata\n");

    //aspetta inizializzazione EROGATORE
    int semWaitInit = atoi(semWaitInit_str);
    sem_op(semWaitInit, 0, -1);
    printf("Inizializzazione EROGATORE Terminata\n");

    //aspetta inizializzazione LAVORATORI
    for(int i=0; i<shared_memory->NOF_WORKERS; i++){
        int semWaitInit = atoi(semWaitInit_str);
        sem_op(semWaitInit, 2, -1);
    }
    printf("Inizializzazione LAVORATORI Terminata\n");

    puts("\nsessione iniziata");
    
    // creare un semaforo mutex giornata attiva
    for (int giorno = 0; giorno < shared_memory->SIM_DURATION; giorno++) {
        // aggiorna il semaforo per indicare che la giornata è iniziata e waita
        printf("\n[Direttore] Giorno %d iniziato.\n", giorno + 1);
        
        semctl(semWaitInit, 8, SETVAL, 1+ shared_memory->NOF_WORKERS + shared_memory->NOF_USERS); //reimposta il semaforo fine giornata a 1 (giornata non finita)

        // reimposta i semafori
        for(int i=0; i<shared_memory->NOF_USERS; i++){
            int semWaitInit = atoi(semWaitInit_str);
            sem_op(semWaitInit, 7, 1);
        }
    
        sem_op(semWaitInit, 4, 1);
    
        for(int i=0; i<shared_memory->NOF_WORKERS; i++){
            int semWaitInit = atoi(semWaitInit_str);
            sem_op(semWaitInit, 6, 1);
        }

        nanosleep((const struct timespec[]){{0, 60 * 8 * shared_memory->N_NANO_SECS}}, NULL);

        // aggiorna variabile per dire fine giornata

        semctl(semWaitInit, 8, SETVAL, 0); //reimposta il semaforo fine giornata a 0 (giornata finita)

        //cambio dei lavori sportelli
        shared_memory->DAYS_LEFT--;
        printf("Giorni rimanenti: %d\n", shared_memory->DAYS_LEFT);
        
        puts("Fine giornata.");

        //Svuoto coda degli sportelli
        int result;
        while ((result = msgrcv(msgId, &message, sizeof(message.mtext), 0, IPC_NOWAIT)) != -1) {
        }
        //riempio coda nuovamente
        if(shared_memory->DAYS_LEFT!=0)
            msg_enqueue(shared_memory, msgId, &message);

        if(shared_memory->DAYS_LEFT == 0){
            puts("\nGiorni finiti, chiudo tutto");
            // signal ia figli per interrompere attesa su nuovo giorno
            for (int i = 0; i < shared_memory->NOF_USERS + shared_memory->NOF_WORKERS + 1; i++) {
                if (kill(pid_array[i], 0) == 0) {
                    if (kill(pid_array[i], SIGUSR1) == -1) {
                        perror("Errore nell'invio del segnale");
                    }
                    printf("Segnale inviato a PID: %d\n", pid_array[i]);
                } 
            }
            break;
        }

        nanosleep((const struct timespec[]){{0, 60 * 16 * shared_memory->N_NANO_SECS}}, NULL);

        // direttore aspetta segnale semaforo da operatore di concluso servizio ultimo utente
        // Quando riceve segnale, dice all'operatore di smettere di servire altri utenti
        
        // Aggiorna statistiche giornaliere

        /*sem_wait(&semid);
        printf("[Direttore] Statistiche al termine del giorno %d:\n", giorno + 1);
        for (int i = 0; i < 6; i++) {
            //Scrittura di tutte le statistiche aggiornatae
        }
        sem_signal(semid, 6);
    */
    }


    while (1) {
        pid_t pid = waitpid(-1, NULL, 0); // Aspetta qualunque figlio
        if (pid == -1) {
            if (errno == ECHILD) {
                // Non ci sono più figli
                printf("Tutti i figli sono terminati.\n");
                break;
            } else {
                perror("Errore durante waitpid");
                break;
            }
        } else {
            //printf("Figlio terminato, PID: %d\n", pid);
        }
    }
    
    // Deallocazione della coda di messaggi
    if (msgctl(msgId, IPC_RMID, NULL) == -1) {
        perror("Errore nella deallocazione della coda di messaggi");
        exit(EXIT_FAILURE);
    }

    puts("direttore finito\n");


}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of arg\n");
        exit(1);
    }

    signal(SIGUSR1, signal_handler);

    /*
    argv1 semafori Inizializzazione
    argv2 shared memory
    argv3 file config
    argv4 NULL
    */

    // Converti la stringa semid_str in intero
    int semWaitInit = atoi(argv[1]);
    
    // Verifica che il semaforo esista
    if (semWaitInit < 0) {
        perror("Errore nel recupero del semaforo");
        exit(1);
    }

    int shm_id = atoi(argv[2]);

    Config *shared_memory = (Config *)shmat(shm_id, NULL, 0);
    if (shared_memory == (Config *)-1) {
        perror("Errore nell'attacco alla memoria condivisa");
        exit(EXIT_FAILURE);
    }

    direttore(argv[1], argv[2], shared_memory);
    // Simula la durata della giornata
    
    if (shmdt(shared_memory) == -1) {
        perror("shmdt error!");
        exit(EXIT_FAILURE);
    }

    //fine giorno n

    exit(0);
}