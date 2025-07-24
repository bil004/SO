SRC_DIR = src
BIN_DIR = bin

CC = gcc
CFLAGS = -Wvla -Wextra -Werror -I -D_GNU_SOURCE$(SRC_DIR)

SOURCES = direttore.c erogatoreTicket.c operatore.c utente.c
EXECUTABLES = $(SOURCES:%.c=$(BIN_DIR)/%)

all: clean makedir $(EXECUTABLES)

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

makedir:
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)