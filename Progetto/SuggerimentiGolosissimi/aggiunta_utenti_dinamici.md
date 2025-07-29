# Aggiunta Dinamica di Utenti - Progetto SO (Versione da 30/30)

**Data:** 2025-07-29

---

## ✅ Obiettivo
Supportare l'aggiunta dinamica di utenti durante la simulazione dell'ufficio postale, come richiesto dal punto 6 della versione completa del progetto (valida per il 30).

---

## 🧩 Cambiamenti Necessari

### 1. Cambiare IPC da `IPC_PRIVATE` a `ftok()`
Per permettere a processi esterni (come `aggiungi_utenti`) di collegarsi alle risorse già esistenti.

```c
// Esempio con ftok()
key_t key_config = ftok("/tmp", 'C');
int shm_id = shmget(key_config, sizeof(Config), IPC_CREAT | 0666);
```

---

### 2. Accesso indipendente degli utenti alle risorse condivise
`utente.c` deve essere in grado di inizializzarsi sia dal direttore (via argv[]) che autonomamente (via `ftok()`).

---

### 3. Rimozione della dipendenza dai semafori di sincronizzazione iniziale
Gli utenti dinamici non possono usare i semafori `sem[3]`, `sem[7]`, `sem[8]` perché sono pensati per i processi originali.

---

### 4. Creazione di un nuovo binario: `utente_dinamico.c`
Processo utente che si autoconfigura tramite `ftok()`, senza dipendere dal direttore.

---

### 5. Creazione del comando esterno: `aggiungi_utenti.c`
Permette l'aggiunta di nuovi utenti durante l'esecuzione:

```bash
./aggiungi_utenti 5
```

---

## 📂 File Creati

### ✅ `utente_dinamico.c`
Contiene la logica di un utente indipendente che si collega alla simulazione tramite IPC esistenti.

### ✅ `aggiungi_utenti.c`
Fork-a e exec-a `utente_dinamico` il numero di volte richiesto da linea di comando.

---

## 🧠 Considerazioni

| Tema | Soluzione |
|------|-----------|
| Come sincronizzare nuovi utenti col tempo simulato? | Usa `shared_memory->DAYS_LEFT` oppure una clock-shared-memory a parte |
| Gli utenti dinamici possono usare i semafori? | Solo se i semafori sono progettati per supportare utenti futuri (meglio evitarli) |
| Serve modificare il direttore? | Solo per passare da `IPC_PRIVATE` a `ftok()` |
| Gli utenti devono aspettare un giorno valido? | Possono usare `sleep(1)` e verificare `DAYS_LEFT > 0` |

---

## 🏁 Esecuzione consigliata

1. **Avvia la simulazione:**

```bash
./direttore
```

2. **In un altro terminale, aggiungi utenti:**

```bash
./aggiungi_utenti 3
```

I nuovi utenti si collegheranno, parteciperanno alla simulazione, chiederanno ticket e riceveranno servizi come quelli originali.

---

## ✍️ Note finali

Per completare al 100% la versione da 30:
- Scrivi le statistiche dei nuovi utenti nel CSV finale
- Fai logging dei PID anche dei dinamici se richiesto
- Assicurati che il tempo sia leggibile globalmente via shared memory

