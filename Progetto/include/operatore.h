#ifndef OPERATORE_H
#define OPERATORE_H

#include "config.h"

#define SERVICE 6

typedef struct ticket {
    int type;
    int time;
} Ticket,*TicketPtr;

int tempiario[SERVICE] = {10, 8, 6, 8, 20, 20};

#endif