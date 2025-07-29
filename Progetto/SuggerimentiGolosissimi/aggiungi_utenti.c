#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_utenti>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Numero di utenti non valido.\n");
        exit(1);
    }

    printf("[AGGIUNGI_UTENTI] Aggiunta di %d utenti dinamici in corso...\n", n);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            execl("./utente_dinamico", "utente_dinamico", NULL);
            perror("execl");
            exit(1);
        }
    }

    // Attesa opzionale per vedere che tutti i processi partano
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("[AGGIUNGI_UTENTI] Tutti gli utenti dinamici sono stati avviati.\n");
    return 0;
}
