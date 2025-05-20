#ifndef CONFIG_H
#define CONFIG_H

#define MAX_SPORTELLI 32

typedef struct Config {
    int NOF_WORKER_SEATS;
    int NOF_WORKERS;
    int NOF_USERS;
    int SIM_DURATION;
    int DAYS_LEFT;
    int DAY;
    int N_NANO_SECS;
    int SERVICE;
    int TIME_SERVICE;
    int P_SERV_MIN;
    int P_SERV_MAX;
    int sportelli[MAX_SPORTELLI];
} Config;

#endif