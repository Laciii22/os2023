#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filterPrimes(int in_fd, int out_fd, int prime) {
    int number;
    while (read(in_fd, &number, sizeof(int)) == sizeof(int)) {
        if (number % prime != 0) {
            write(out_fd, &number, sizeof(int));
        }
    }
    close(in_fd);
    close(out_fd);
    exit(0);
}

int main() {
    int pipes[2];
    pipe(pipes);

    for (int i = 2; i <= 35; i++) {
        write(pipes[1], &i, sizeof(int));
    }

    close(pipes[1]);

    int prime;
    while (read(pipes[0], &prime, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", prime);

        int new_pipes[2];
        pipe(new_pipes);

        int pid = fork();
        if (pid == 0) {
            close(new_pipes[0]);
            filterPrimes(pipes[0], new_pipes[1], prime);
        } else {
            close(new_pipes[1]);
            close(pipes[0]);
            pipes[0] = new_pipes[0];
        }
    }

    close(pipes[0]);
    wait(0);

    return 0;
}


