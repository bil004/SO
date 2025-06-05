# Relazione del Progetto

## Introduzione

Il presente progetto ha come obiettivo la simulazione del funzionamento di un ufficio postale. La simulazione è stata progettata per rappresentare in modo realistico le dinamiche di gestione di un ufficio, includendo la gestione di processi, risorse e servizi offerti agli utenti.

## Descrizione del Sistema

Il sistema si compone di diversi processi e risorse che interagiscono tra loro:

- **Processo Direttore Ufficio Posta**: responsabile dell’avvio e della gestione della simulazione, nonché della raccolta e della stampa delle statistiche relative alle richieste e ai servizi erogati. Il direttore crea gli sportelli e i processi operatore e utente.
- **Processo Erogatore Ticket**: si occupa di erogare ticket generici e specifici per i servizi offerti dall’ufficio postale, in base alle richieste degli utenti.
- **Processi Utente**: simulano i clienti che richiedono servizi specifici, attendono il proprio turno, ricevono la prestazione e terminano la loro attività.
- **Risorse Sportello**: ogni sportello è specializzato in un tipo di servizio, che può variare giornalmente. Gli sportelli sono occupati dagli operatori durante l’orario di lavoro.
- **Processi Operatore**: eseguono i servizi agli sportelli, effettuano pause casuali e gestiscono la disponibilità degli sportelli.

## Servizi Offerti

L’ufficio postale offre i seguenti servizi, con i relativi tempi medi di erogazione (in minuti):

| Servizio                      | Tempo medio (minuti) |
|------------------------------|---------------------|
| Invio e ritiro pacchi         | 10                  |
| Invio lettere e raccomandate  | 8                   |
| Prelievi e versamenti Bancoposta | 6                |
| Pagamento bollettini postali  | 8                   |
| Acquisto prodotti finanziari  | 20                  |
| Acquisto orologi e braccialetti | 20                |

Il tempo di erogazione di ogni servizio è generato casualmente nell’intervallo ±50% del valore medio indicato, per simulare la variabilità reale.

## Dettagli della Simulazione

La simulazione si svolge su un numero di giorni definito da `SIM_DURATION`. Ogni minuto simulato corrisponde a un intervallo di nanosecondi (`N_NANO_SECS`). La simulazione inizia solo quando tutti i processi (erogatore, operatori, utenti) sono stati inizializzati correttamente.

Alla fine di ogni giornata, il processo direttore stampa statistiche dettagliate, tra cui:

- Numero totale e medio di utenti serviti.
- Numero totale e medio di servizi erogati e non erogati.
- Tempi medi di attesa e di erogazione dei servizi.
- Statistiche suddivise per tipologia di servizio.
- Numero di operatori attivi e pause effettuate.
- Rapporto tra operatori disponibili e sportelli esistenti.

## Funzionamento dei Processi

- **Direttore**: crea i processi e le risorse, avvia e termina la simulazione, legge e stampa le statistiche.
- **Erogatore Ticket**: fornisce i ticket richiesti dagli utenti.
- **Operatori**: competono per occupare sportelli, eseguono i servizi, possono effettuare pause fino a un massimo definito (`NOF_PAUSE`).
- **Utenti**: decidono se recarsi all’ufficio postale, scelgono il servizio, attendono il turno e ricevono il servizio.

## Terminazione della Simulazione

La simulazione termina al raggiungimento della durata impostata (`SIM_DURATION`) o se il numero di utenti in attesa supera una soglia definita (`EXPLODE_THRESHOLD`).

## Requisiti Implementativi

Il progetto è stato realizzato rispettando i seguenti requisiti:

- Evitare l’attesa attiva.
- Utilizzare memoria condivisa, semafori e un meccanismo di comunicazione tra processi (code di messaggi o pipe).
- Suddividere il codice in moduli, con eseguibili separati per i vari processi.
- Compilare con `make` e con le opzioni `gcc -Wvla -Wextra -Werror`.
- Massimizzare la concorrenza tra processi.
- Deallocare le risorse IPC al termine.
- Supportare l’esecuzione su macchine con parallelismo (più processori).
- Definire la macro `GNU_SOURCE` o compilare con il flag `-D_GNU_SOURCE`.
