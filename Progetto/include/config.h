#ifndef MACRO_H
#define MACRO_H

typedef struct Config {
    int NOF_WORKER_SEATS;
    int NOF_WORKERS;
    int NOF_USERS;
    int SIM_DURATION;
    int DAYS_LEFT;
    int N_NANO_SECS;
    int SERVICE;
    int TIME_SERVICE;
    int P_SERV_MIN;
    int P_SERV_MAX;
} Config;

#endif