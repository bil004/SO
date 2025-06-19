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
