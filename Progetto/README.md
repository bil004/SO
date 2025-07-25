# Progetto di Sistemi Operativi - 2024/2025
## Ufficio delle Poste
### Angela, Benslimane, Chirio

---

## Relazione sul funzionamento del programma

Questo progetto simula il funzionamento di un ufficio postale tramite processi concorrenti che interagiscono attraverso memoria condivisa, semafori e code di messaggi. La simulazione si sviluppa su più giornate lavorative e raccoglie statistiche dettagliate sull’attività svolta.

### Struttura generale

Il processo principale (`direttore`) avvia e coordina i seguenti processi:

- **Direttore**: gestisce la simulazione, crea e sincronizza tutti gli altri processi (Erogatore Ticket, Operatori, Utenti), aggiorna la configurazione giornaliera degli sportelli (ogni giorno genera sportelli con tipoLavoro casuale) e raccoglie le statistiche tramite le shared memory `statsDay` e `statsSim`.
- **Erogatore Ticket**: gestisce la coda dei ticket tra utenti e operatori, riceve richieste dagli utenti e assegna un numero progressivo di ticket.
- **Operatori**: rappresentano i lavoratori che gestiscono gli sportelli. Ogni operatore può svolgere un solo tipo di servizio (assegnato alla creazione), serve gli utenti in ordine FIFO e aggiorna le statistiche giornaliere e totali.
- **Utenti**: simulano i clienti dell’ufficio postale. Ogni utente decide casualmente se recarsi in posta e quale servizio richiedere, interagendo con l’erogatore ticket e poi con l’operatore.

### Funzionamento dei processi

- **Direttore**
  - Inizializza le strutture dati condivise e i semafori.
  - Avvia i processi figli (erogatore ticket, operatori, utenti).
  - Ogni giorno assegna casualmente i tipi di servizio agli sportelli.
  - Coordina l’inizio e la fine delle giornate tramite semafori e segnali.
  - Raccoglie e stampa le statistiche giornaliere e finali.
  - Termina la simulazione e cancella le strutture dati e semafori in memoria

- **Erogatore Ticket**
  - Riceve richieste di ticket dagli utenti tramite una coda di messaggi.
  - Assegna un ticket progressivo e risponde all’utente.
  - Funziona come servizio centralizzato per la gestione delle code utenti.

- **Operatori**
  - Ogni operatore attende di essere assegnato a uno sportello del proprio tipo.
  - Riceve richieste dagli utenti tramite una coda di messaggi.
  - Simula l’erogazione del servizio e aggiorna le statistiche.
  - Può effettuare pause secondo una probabilità configurabile.

- **Utenti**
  - Decidono casualmente se andare in posta e quale servizio richiedere.
  - Richiedono un ticket all’erogatore e attendono di essere serviti dall’operatore.
  - Aggiornano le statistiche in base all’esito della richiesta (servito o non servito).

### Sincronizzazione e comunicazione

- **Gestione delle giornate**: il direttore sincronizza l’inizio e la fine delle giornate tramite un semaforo. All’inizio di ogni giornata tutti i processi decrementano il semaforo, in modo che al termine della giornata esso sia a zero e nessun processo possa proseguire senza che il direttore abbia annunciato la giornata successiva. Al termine di ogni giornata il direttore invia segnali SIGUSR1 e SIGUSR2 a tutti i processi, per permettere a quelli bloccati di proseguire correttamente.
- **Memoria condivisa**: utilizzata per la configurazione globale, la lista degli sportelli, dei lavoratori e le statistiche.
- **Semafori**: coordinano l’inizializzazione, l’inizio/fine delle giornate e la sincronizzazione tra processi.
- **Code di messaggi**: gestiscono la comunicazione tra utenti, erogatore ticket e operatori.

### Statistiche

Il programma raccoglie statistiche dettagliate per ogni giornata e per l’intera simulazione, tra cui:
- Numero di utenti serviti e non serviti per ogni servizio.
- Tempi medi di attesa e di erogazione.
- Numero di operatori attivi e pause effettuate.
- Rapporto tra operatori e sportelli per ogni tipo di servizio.

Le statistiche vengono stampate sia a video che su file CSV per