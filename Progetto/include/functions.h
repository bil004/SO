#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void setValues(StatsDay *day/*, StatsSim *sim, int resetDay*/);
void msg_enqueue(Config* shared_memory, int msgId, struct msgbuf *message);
void sem_op(int semid, int sem_num, int sem_op);
void signal_handler(int sig);
void errExit(char* s);
void direttore(int semWaitInit, int shm_id, int shm_id_StatsDay, int shm_id_StatsSim, Config* shared_memory, StatsDay* statsDay, StatsSim* statsSim);
void load_config(const char *filename, Config *config);
void cicloOperativo(int semLavoratore, int i, int tipoLavoro, int msgid, int nanoSec, int nofPause, StatsDay *statsDay, StatsSim *statsSim);
void sem_op(int semid, int sem_num, int sem_op);

#endif