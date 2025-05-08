#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#include "../include/config.h"

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

// uso la pipe per ricevere i dati da erogatore_ticket

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of arg\n");
        exit(1);
    }

    // Attacca la memoria condivisa
    
    int shmId = atoi(argv[2]);
    Config *shared_memory = (Config *)shmat(shmId, NULL, 0);
    if (shared_memory == (Config *)-1) {
        perror("Errore nell'attacco alla memoria condivisa");
        exit(EXIT_FAILURE);
    }
    
    //FINE INIZIALIZZAZIONE
    int semUtente = atoi(argv[1]);

    //incrementa semaforo per informare il padre
    sem_op(semUtente, 3, 1);
    //printf("Inizializzazione Terminata utente %d\n", i);
    //Attende la risposta del padre
    sem_op(semUtente, 7, -1);

    
    //printf("Running utente %d\n", i);

    // -----------------------------------------------------------------------------
    while(shared_memory->DAYS_LEFT > 0){
        puts("utente Sleeping till day starts");
        sem_op(semUtente, 8, -1);
    
        // Probabilità della decisione dell'utente
        srand(time(NULL)^getpid());
        int intervallo = shared_memory->P_SERV_MAX - shared_memory->P_SERV_MIN + 1;
        int P_SERV = (rand() % intervallo) + shared_memory->P_SERV_MIN;

        int fail = (rand() % intervallo) + shared_memory->P_SERV_MIN;

        if(P_SERV <= fail) {
            puts("Vai alla posta");
            
            switch (rand() % 6) {
                case 0:
                    printf("USER %d: Invio e ritiro pacchi\n", getpid());
                    break;
                
                case 1:
                    printf("USER %d: Invio e lettere e raccomandate\n", getpid());
                    break;
                
                case 2:
                    printf("USER %d: Prelievi e versamenti Bancoposta\n", getpid());
                    break;
                
                case 3:
                    printf("USER %d: Pagamento bollettini postali\n", getpid());
                    break;
                
                case 4:
                    printf("USER %d: Acquisto prodotti finanziari\n", getpid());
                    break;
                
                case 5:
                    printf("USER %d: Acquisto orologi e braccialetti\n", getpid());
                    break;
                
                default:
                    puts("Error");
                    break;
            }

            int orario = (rand()%1320) * shared_memory->N_NANO_SECS; //Sceglie un'orario tra 1 minuto e 22 ore dal momento della decisione
            nanosleep((const struct timespec[]){{0, orario}}, NULL);
            //Controllo posta aperta
            //Si reca dall'erogatore ticket per controllare se gli sportelli per il suo tipo di op. sono aperti
            
        }
        else puts("Non vai alla posta");

        sem_op(semUtente, 8, 0);
        //Aggiorna statistiche

    }

    // -----------------------------------------------------------------------------


    exit(0);
}