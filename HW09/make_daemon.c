#include "make_daemon.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/stat.h>

/* путь к файлу для хранения PID процесса */
#define PIDFILE "/tmp/hw09.pid"
static int pid_d = -999;

/* записать значение PID процесса в файл */
void write_pid() {
	char buf[16];
	
	sprintf(buf, "%ld", (long)getpid());
	if(write(pid_d, buf, strlen(buf) + 1) < 0) {
		return;
	}
}

/* заблокировать файл на время работы демона */
void lock_pid_file() {
	struct flock lock;
	int fd;
	
	fd = open(PIDFILE, (O_RDWR | O_CREAT), (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP));
	if (fd < 0) {
		exit(-1);
	}
	
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    
    fcntl(fd, F_SETLK, &lock);
    
    pid_d = fd;
    
    write_pid();
}

/* перехватить сигнал и обновить файл pid */
void catch_SIGHUP(int s) {
	if ((s == SIGHUP) && (pid_d != -999)) {
		write_pid();
	} 
}

void make_daemon() {
	pid_t pid;

	umask(0);
	signal(SIGHUP, catch_SIGHUP);

    /* Создаем дочерний процесс */
    pid = fork();
    /* Попытка неудачна */
    if (pid < 0) { exit(-1); }
    /* Создан потомок, родитель получает его PID. Завершаем процесс родителя */
    if (pid != 0) { exit(0); }

    /* Потомок становится "лидером" группы */
    if (setsid() < 0) { exit(-1); }

    /* Потомок от 1-го fork-a теперь становится родителем */
    pid = fork();
    /* Попытка неудачна */
    if (pid < 0) { exit(-1); }
    /* Создан потомок, родитель получает его PID. Завершаем процесс родителя */
    if (pid != 0) { exit(0); }
    
    /* Меняем рабочую директорию */
    if (chdir("/tmp") < 0) {
		
		exit(-1);
	}
    /*
     * Close all file descriptors (except PID file descriptor)
     */
    /* Закрыть все дексрипторы, кроме самого фа */
    struct rlimit rl ;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        exit(EXIT_FAILURE);
    }
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (rlim_t i = 0; i < rl.rlim_max; i++) {
        if ((int)i != pid_d) {
            close((int)i);
        }
    }
    
     /*
     * Set stdin/stdout/stderr file descriptors to /dev/null
     */
    /* установить стандартные дексрипторы STDIN/STDOUT/STDERR в /dev/null */
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        exit(-1);
    }
	
	
	lock_pid_file();
}

