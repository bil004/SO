# Sistemi Operativi 2024/25

## Simulazione di un Ufficio Postale

Questo progetto, sviluppato per il corso di **Sistemi Operativi** (a.a. 2024/2025), simula il funzionamento di un ufficio postale tramite l’utilizzo di processi concorrenti, memoria condivisa, semafori e code di messaggi.

---

### 🧩 Obiettivi principali
- Simulare la gestione quotidiana di un ufficio postale.
- Modellare i comportamenti di utenti, sportelli e operatori.
- Gestire ticket e code in modo realistico.
- Raccogliere statistiche giornaliere e complessive.
- Implementare meccanismi di terminazione della simulazione (durata massima o sovraccarico di richieste).

---

### 🏗️ Componenti principali
- **Processo Direttore**: gestisce la simulazione, crea i processi e raccoglie le statistiche.
- **Processo Erogatore Ticket**: assegna i ticket agli utenti.
- **Processi Operatore**: gestiscono sportelli specializzati e servono gli utenti.
- **Processi Utente**: si recano all’ufficio postale e richiedono servizi.
- **Risorse condivise**: memoria condivisa, semafori e code di messaggi.

---

### ⚙️ Tecnologie e strumenti
- Linguaggio: **C**
- Meccanismi IPC: **memoria condivisa**, **semafori**, **code di messaggi**
- Compilazione: **Makefile**
- Documentazione tecnica: **Doxygen**
- Output delle statistiche in formato **CSV**

---

### ▶️ Esecuzione del progetto

#### Compilazione
Per compilare tutti i componenti del progetto:
```bash
make all
```

#### Pulizia
Per rimuovere i file exe + .csv:
```bash
make clean
```

#### Compilazione
Per avviare la simulazione (in ´bin´):
```bash
./direttore
```

> **Nota:** Questo progetto è parte della valutazione del corso e **non deve essere condiviso** o riutilizzato senza autorizzazione fino a novembre 2025, come indicato nel regolamento didattico.