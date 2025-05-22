# Suggerimenti su IPC e gestione ticket tra processi

## Comunicazione tra processi utente ed erogatoreTicket

Per mettere in comunicazione due processi in C (ad esempio utente ed erogatoreTicket) e gestire la richiesta di ticket, puoi usare una coda di messaggi System V. Questo metodo permette di:
- Sincronizzare più utenti che richiedono ticket.
- Gestire le richieste in ordine FIFO.
- Inviare la risposta direttamente all'utente che l'ha richiesta.

### Struttura del messaggio
```c
struct ticket_msg {
    long mtype;      // Tipo messaggio: 1 per richiesta, PID utente per risposta
    int servizio;    // Tipo di servizio richiesto
    int ticket;      // Numero del ticket (solo nella risposta)
    pid_t pid;       // PID dell'utente (solo nella richiesta)
};
```

### Lato utente
1. Genera la chiave con `ftok("/tmp", 'T')` (il path deve esistere, la lettera può essere cambiata per code diverse).
2. Ottieni la coda con `msgget`.
3. Invia la richiesta con `msgsnd`.
4. Attendi la risposta con `msgrcv` usando come tipo di messaggio il proprio PID.

Esempio:
```c
key_t msgkey = ftok("/tmp", 'T');
int msgid = msgget(msgkey, 0666 | IPC_CREAT);
struct ticket_msg richiesta = {1, servizio, 0, getpid()};
msgsnd(msgid, &richiesta, sizeof(struct ticket_msg) - sizeof(long), 0);
struct ticket_msg risposta;
msgrcv(msgid, &risposta, sizeof(struct ticket_msg) - sizeof(long), getpid(), 0);
printf("Utente %d ha ricevuto il ticket %d\n", getpid(), risposta.ticket);
```

### Lato erogatoreTicket
1. Usa la stessa chiave per accedere alla coda.
2. Ricevi le richieste con `msgrcv` (tipo 1).
3. Genera un ticket (ad esempio incrementando una variabile).
4. Invia la risposta con `msgsnd` usando come tipo di messaggio il PID dell'utente.

Esempio:
```c
key_t msgkey = ftok("/tmp", 'T');
int msgid = msgget(msgkey, 0666 | IPC_CREAT);
int next_ticket = 1;
while (1) {
    struct ticket_msg richiesta;
    msgrcv(msgid, &richiesta, sizeof(struct ticket_msg) - sizeof(long), 1, 0);
    struct ticket_msg risposta = {richiesta.pid, richiesta.servizio, next_ticket++, richiesta.pid};
    msgsnd(msgid, &risposta, sizeof(struct ticket_msg) - sizeof(long), 0);
}
```

### Note su ftok
- Il path passato a `ftok` deve esistere (es. `/tmp` va sempre bene su Linux).
- La lettera (es. `'T'`) può essere cambiata per generare chiavi diverse.

### Richieste multiple
Il codice proposto gestisce una richiesta di ticket per volta. Se vuoi che un utente possa richiedere più servizi contemporaneamente, modifica la struttura del messaggio per includere un array di servizi e gestisci la risposta di conseguenza.

---

Per domande o estensioni (es. richieste multiple), chiedi pure!