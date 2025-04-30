#ifndef EROGATORE_TICKET_H
#define EROGATORE_TICKET_H

#include "config.h"

typedef struct ticket {
    int type;
    int time;
} Ticket,*TicketPtr;

char *servizio[SERVICE] = {
    "Invio e ritiro pacchi",
    "Invio lettere e raccomandate",
    "Prelievi e versamenti Bancoposta",
    "Pagamento bollettini postali",
    "Acquisto prodotti finanziari",
    "Acquisto orologi e braccialetti",
};

int tempario[TIME_SERVICE] = {10, 8, 6, 8, 20, 20};

#endif