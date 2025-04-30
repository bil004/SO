# Imposta la directory dei sorgenti (adatta il percorso se necessario)
SRC_DIR = src
BIN_DIR = bin

# Specifichiamo il compilatore e le opzioni di compilazione
CC = gcc
CFLAGS = -Wall -I$(SRC_DIR)

all: makedir $(BIN_DIR)/main $(BIN_DIR)/direttore $(BIN_DIR)/erogatoreTicket $(BIN_DIR)/operatore $(BIN_DIR)/utente

# Regola per il programma padre (main)
$(BIN_DIR)/main: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main $(SRC_DIR)/main.c

# Regola per il programma figlio direttore
$(BIN_DIR)/direttore: $(SRC_DIR)/direttore.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/direttore $(SRC_DIR)/direttore.c

# Regola per il programma figlio erogatoreTicket
$(BIN_DIR)/erogatoreTicket: $(SRC_DIR)/erogatoreTicket.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/erogatoreTicket $(SRC_DIR)/erogatoreTicket.c

# Regola per il programma figlio operatore
$(BIN_DIR)/operatore: $(SRC_DIR)/operatore.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/operatore $(SRC_DIR)/operatore.c

# Regola per il programma figlio utente
$(BIN_DIR)/utente: $(SRC_DIR)/utente.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/utente $(SRC_DIR)/utente.c

makedir: 
	mkdir -p bin

# Regola per pulire i file compilati
clean:
	rm -rf $(BIN_DIR)/*