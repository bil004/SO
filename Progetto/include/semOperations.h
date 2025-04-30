#ifndef MACRO_H
#define MACRO_H


struct sembuf operazione;

void sem_op(int semid, int sem_num, int sem_op);

#endif