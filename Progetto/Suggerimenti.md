## Statistiche giornaliere e cumulative: utenti serviti in media al giorno

### 1. Obiettivo

Calcolare correttamente il **numero di utenti serviti in media al giorno** nel progetto "Ufficio Postale".

### 2. Formula corretta

```
Media utenti serviti al giorno = Totale utenti serviti / Numero di giorni simulati
```

### 3. Quando aggiornare le statistiche?

Il progetto richiede che **ogni giorno** vengano stampate le statistiche. Quindi:

- **Alla fine di ogni giornata**:

  - Stampare le statistiche giornaliere
  - Aggiornare le statistiche cumulative

- **Alla fine della simulazione**:

  - Stampare tutte le statistiche cumulative e le medie al giorno

### 4. Strutture dati suggerite

```c
// Statistiche della giornata corrente
typedef struct {
    int utenti_serviti;
    // ... altri campi giornalieri
} StatisticheGiornaliere;

// Statistiche cumulative
typedef struct {
    int utenti_serviti_totali;
    int giorni_trascorsi;
    // ... altri campi cumulativi
} StatisticheCumulative;
```

### 5. Gestione durante la simulazione

```c
// All'inizio della giornata
StatisticheGiornaliere statsGiornata = {0};

// Durante la giornata (es. ogni volta che un utente viene servito):
statsGiornata.utenti_serviti++;

// Alla fine della giornata:
printf("[GIORNO %d] Utenti serviti oggi: %d\n", statsCumulative.giorni_trascorsi + 1, statsGiornata.utenti_serviti);

// Aggiornamento cumulative
statsCumulative.utenti_serviti_totali += statsGiornata.utenti_serviti;
statsCumulative.giorni_trascorsi++;

// Calcolo e stampa media
float media_utenti = (float)statsCumulative.utenti_serviti_totali / statsCumulative.giorni_trascorsi;
printf("Media utenti serviti al giorno (fino a oggi): %.2f\n", media_utenti);
```

### 6. Esempio di output

Se i valori giornalieri sono:

- Giorno 1: 20 utenti
- Giorno 2: 25 utenti
- Giorno 3: 15 utenti

Allora l'output al giorno 3 sarà:

```
[GIORNO 3] Utenti serviti oggi: 15
Media utenti serviti al giorno (fino a oggi): 20.00
```

### 7. Note finali

- Non serve calcolare la media rispetto agli utenti esistenti o fare una media delle medie giornaliere.
- Ricorda di resettare `StatisticheGiornaliere` all'inizio di ogni giorno.
- Le statistiche cumulative devono essere mantenute in memoria condivisa se usate da più processi.

---

## Suggerimenti avanzati per il progetto "Ufficio Postale"

### 1. Sincronizzazione e Race Condition
- Usa semafori o mutex per proteggere l’accesso a strutture dati condivise (memoria condivisa, code di messaggi).
- Quando usi le code di messaggi, preferisci la modalità polling con una breve sleep (usleep) se vuoi evitare busy waiting, ma valuta anche l’uso di IPC_NOWAIT per non bloccare i processi.
- Gestisci con attenzione i segnali: assicurati che i flag (`terminate`, `nextDay`) siano sempre dichiarati `volatile sig_atomic_t` e che la logica di uscita dai cicli sia robusta contro race condition.

### 2. Gestione dei segnali
- Ricorda che i segnali possono arrivare in qualsiasi momento: proteggi le sezioni critiche e controlla i flag dopo ogni operazione potenzialmente bloccante (msgrcv, semop).
- Se usi segnali per terminare i processi, assicurati che la terminazione sia "pulita": libera risorse, aggiorna le statistiche e stampa messaggi di log.

### 3. Gestione delle code di messaggi
- Svuota sempre le code di messaggi a fine giornata per evitare che messaggi "vecchi" interferiscano con la giornata successiva.
- Quando invii messaggi, controlla sempre il valore di ritorno di `msgsnd` e gestisci eventuali errori (es. coda piena).

### 4. Statistiche e memoria condivisa
- Aggiorna le statistiche in memoria condivisa in modo atomico (usa semafori se più processi scrivono contemporaneamente).
- Resetta le statistiche giornaliere all’inizio di ogni giornata.
- Alla fine della simulazione, stampa sia le statistiche giornaliere che quelle cumulative.

### 5. Robustezza e debug
- Usa macro per i colori nei messaggi di log per rendere l’output più leggibile.
- Inserisci messaggi di debug nei punti chiave (inizio/fine ciclo, ricezione/invio messaggi, cambio giorno, terminazione).
- Gestisci tutti i possibili errori di sistema (ritorno -1 da funzioni di IPC, semafori, memoria condivisa).

### 6. Strutture dati
- Mantieni le strutture dati semplici e ben documentate. Usa commenti per spiegare il significato di ogni campo.
- Se hai bisogno di statistiche per servizio, per giorno, per operatore, valuta l’uso di array bidimensionali o strutture annidate.

### 7. Modularità
- Separa la logica di ogni componente (direttore, operatore, utente, erogatore ticket) in file e funzioni distinte.
- Usa header file per dichiarare le strutture dati e le funzioni condivise.

### 8. Testing
- Prevedi la possibilità di lanciare la simulazione con parametri diversi (numero utenti, operatori, durata, ecc.) per testare tutti i casi.
- Testa casi limite: nessun utente, nessun operatore, code piene, code vuote, segnali inviati in momenti critici.

---
