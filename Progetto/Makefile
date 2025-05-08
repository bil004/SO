# Imposta la directory dei sorgenti e dei binari
SRC_DIR = src
BIN_DIR = bin

# Specifichiamo il compilatore e le opzioni di compilazione
CC = gcc
CFLAGS = -Wvla -Wextra -Werror -I$(SRC_DIR)

# Lista dei file sorgenti e degli eseguibili
SOURCES = main.c direttore.c erogatoreTicket.c operatore.c utente.c
EXECUTABLES = $(SOURCES:%.c=$(BIN_DIR)/%)

# Regola principale
all: makedir $(EXECUTABLES)

# Regola generica per compilare ogni file sorgente
$(BIN_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Regola per creare la directory bin
makedir:
	mkdir -p $(BIN_DIR)

# Regola per pulire i file compilati
clean:
	rm -rf $(BIN_DIR)