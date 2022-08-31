#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <getopt.h>

#include "make_daemon.h"

/* Размер буфера */
#define BUFF_SIZE 1024

/* Структура для хранения считанных из конфигурационного файла параметров */
struct config {
	char *filename;		/* имя файла */
	char *socket_name;	/* имя сокета */
} config;

/* Массив с названием ошибок */
static char *file_msg_str[] = {
	"File size = %d bytes\n",
	"File %s not found!\n",
	"Can't read/write %s file\n",
	"END Connection! Bye!"
};

/* Индекс массива (см. выше) */
enum file_access_codes {
	OK,
	FILE_NOT_FOUND,
	FAIL_TO_READWRITE,
};

/* Прототипы функций */
void usage();
void make_socket();
int check_access(char *filename);
off_t get_file_size(char *filename);
char *make_human_readable(off_t file_size);
void check_config(char *filename);

int main(int argc, char *argv[]) {
	int c = 0; 					/* для переданных параметров программы */
	char *filename = NULL;
	
	/* Выход из программы, если не переданы аргументы */
	if (argc == 1) {
		usage();
		exit(-1);
	}
	
	/* "разбираем" переданные параметры */
	while ((c = getopt(argc, argv, "dc:")) != -1){
		switch(c){
			case 'd':
				make_daemon();
				break;
			case 'c':
				filename = strdup(optarg); /* имя файла "копируем" в переменную */
				break;
			default:
				usage();
				exit(-1);
		}
	}
	/* Проверить конфиг */
	check_config(filename);
	make_socket();
	
	free(config.filename);
	free(config.socket_name);
	free(filename);
	return 0;
}

void usage() {
	printf("Usage: ./hw09 [d] {c} <path to config file>\n");
	printf("\td - run as daemon\n\tc - path to config file\n");
	printf("\tConfig file format:\n\t\tfile = <your file to watch>\n\t\tsocket = <any_socket_name.socket to listen>\n");
}

void make_socket() {
	int sock, msgsock, rval;
	int option = 1;
	off_t file_size;
	short result;
	struct sockaddr_un server;
	// БУФЕР
	char rx_buf[BUFF_SIZE];
	char tx_buf[BUFF_SIZE];
	
	/* Если вызов функции socket неудачен */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("ERROR");
		exit(-1);
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	/* устанавливаем значение поля sun_family */
	server.sun_family = AF_UNIX;
	/* устанавливаем значение поля sun_path */
	strcpy(server.sun_path, config.socket_name);
	if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
		perror("ERROR");
		exit(-1);
	}
	
	/* Слушаем "входящие" "подключения" */
	listen(sock, 5);
	
	/* "принимаем" соединение на сокете */
	msgsock = accept(sock, 0, 0);
	if (msgsock == -1) {	
		perror("ERROR");
		exit(-1);
	}
	else {
		/* инициализировать массивы '\0' */
		bzero(rx_buf, sizeof(rx_buf));
		bzero(tx_buf, sizeof(tx_buf));
		/* читаем из сокета в буфер 1024 байта */
		if ((rval = read(msgsock, rx_buf, 1024)) < 0) {
			perror("ERROR");}
		else if (rval == 0) {
			perror("ERORR");}
		else {
		/* Если при доступе к файлу произошли какие-либо проблемы (удален и т.п.) */
		if((result = check_access(config.filename)) != 0) {
			snprintf(tx_buf, 1024, file_msg_str[result], config.filename);
		}
		/* Иначе узнать размер файла */
		else {
			file_size = get_file_size(config.filename);
			snprintf(tx_buf, 1024, file_msg_str[result], file_size);
		}
		/* Отправить клиенту */
		send(msgsock, tx_buf, strlen(tx_buf), 0);
		send(msgsock, file_msg_str[3], strlen(file_msg_str[3]), 0);
		}	
	}			
	
	//закрываем сокет
	close(msgsock);
	close(sock);
	unlink(config.socket_name);	
}

/* проверить доступ к файлу */
int check_access(char *filename) {	
	/* Если файл отсутствует*/
	if(access(filename, 0) != 0) {
		return FILE_NOT_FOUND;
	}
	/* Если не досупен для чтения/записи */
	if(access(filename, 4) != 0) {
		return FAIL_TO_READWRITE;
	}
	return OK;
}

/* получить размер файла используя функцию stat() */
off_t get_file_size(char *filename){
	struct stat st;

	stat(filename, &st);

	return st.st_size;
}

/* проверить конфигурационный файл параметр  */
void check_config(char *filename) {
	FILE *fconf;
	char *line = NULL, *letter = NULL;
	size_t length = 0;
	unsigned int counter = 0;
	short result = 0;
	
	/* Если проблемы с доступом */
	if((result = check_access(filename)) != 0) {
		perror(file_msg_str[result]);
		exit(-1);
	}
	/* Открыть файл для чтения */
	if((fconf = fopen(filename, "r")) == NULL) {
		perror("ERROR");
		exit(-1);
	}
		
	while (getline(&line, &length, fconf) > 0) {
		/* получить указатель на начало следующего занзнаком = символа */
		if ((letter = strchr(line,'=')) == NULL) {
			printf("ERROR!: Check your config\n");
			usage();
			exit(-1);
		}
		 /* если строка содержит директиву "file", то скопировать ее + добавить \0 в конце */
		if(!strncmp("file", line, 4)) {
			config.filename = strdup(letter + 2);
			config.filename[strlen(config.filename) - 1] = '\0';
		}
		/* если строка содержит директиву "socket", то скопировать ее + добавить \0 в конце */
		if(!strncmp("socket", line, 4)) {
			config.socket_name = strdup(letter + 2);
			config.socket_name[strlen(config.socket_name) - 1] = '\0';
		}
		counter++;
	}
	
	/* если число строк не равно 2, значит не все параметры в файле конфигурации заданы */
	if (counter < 2) { 
		usage(); 
		exit(-1); 
	}

	free(line);
	fclose(fconf);
}

