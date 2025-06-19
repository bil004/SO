#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void setValues(Stats *smStats);
void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message);
void sem_op(int semid, int sem_num, int sem_op);
void signal_handler(int sig);
void errExit(char* s);
void direttore(char* semWaitInit_str, char* shmid_str, char* shmid_Stats_str, Config* shared_memory, Stats* smStats);
void load_config(const char *filename, Config *config);
void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause);
void sem_op(int semid, int sem_num, int sem_op);

#endif