#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

const int LOCK_TIME = 1;

const int PID_SIZE = sizeof(pid_t);

const int MAX_FILENAME_LENGTH = 255*8;

int success_locks_count = 0;
int all_locks_count = 0;

void sigint_handler(int signum, char* file_name) {
    static char* lck_file_name = NULL;

    if (lck_file_name == NULL) {
        lck_file_name = file_name;
    }
    int pid = (int)getpid();
    char str_pid[PID_SIZE];
    sprintf(str_pid, "%d", pid);

    if (signum == -1) {
        return;
    }

    FILE* lck_file_read = fopen(lck_file_name, "r+");
    if (lck_file_read != NULL) {
        char lck_file_pid[PID_SIZE];
        fgets(lck_file_pid, 10, lck_file_read);

        if (strcmp(lck_file_pid, str_pid) == 0) {
            remove(lck_file_name);
        }
    }

    FILE* result_file = fopen("result.txt", "a");
    fprintf(result_file, "[pid:%d] Успешных/всего блокировок - %d/%d\n",
            pid, success_locks_count, all_locks_count);
    fclose(result_file);
    exit(0);
}

int file_exists(char* filename) {
    return access(filename, F_OK) == 0;
}

void main(int argc, char** argv) {
    signal(SIGINT, (void (*)(int))sigint_handler);

    if (argc != 2) {
        printf("Передано неверное количество параметров");
        return;
    }

    int pid = (int)getpid();

    char str_pid[PID_SIZE];
    sprintf(str_pid, "%d", pid);

    char* file_name = argv[1];

    char lck_file_name[MAX_FILENAME_LENGTH];
    sprintf(lck_file_name, "%s.lck", file_name);

    sigint_handler(-1, lck_file_name);

    while (1) {
        all_locks_count += 1;
        if (file_exists(lck_file_name)) {
            continue;
        }

        FILE* lck_file_write = fopen(lck_file_name, "wx");
        if (lck_file_write == NULL) {
            printf("! Файл уже существует\n");
            continue;
        }
        fprintf(lck_file_write, "%d", pid);
        fflush(lck_file_write);

        printf("Файл %s заблокирован для использования\n", file_name);

        sleep(LOCK_TIME); // Какие-то действия с файлом

        if (!file_exists(lck_file_name)) {
            printf("[!] lck-файл не существует\n");
            return;
        }

        FILE* lck_file_read = fopen(lck_file_name, "r+");
        char lck_file_pid[PID_SIZE];
        fgets(lck_file_pid, 10, lck_file_read);

        fclose(lck_file_write);
        fclose(lck_file_read);
        remove(lck_file_name);

        if (strcmp(lck_file_pid, str_pid) != 0) {
            printf("[!] pid не совпадает\n");
            return;
        }

        success_locks_count += 1;

        printf("Файл %s свободен для использования\n", file_name);
    }
}