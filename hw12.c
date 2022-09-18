#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define SERVER "telehack.com"
#define PORT "23"
#define BUFF_SIZE 4096

char buf[BUFF_SIZE];

/* Информация о использовании программы */
void usage() {
	printf("USAGE: ./hw12 -f <font> -t \"<text>\"\n");
	printf("\t<font> - see Readme.md in github repo\n");
	printf("\t<text> - any text that you want\n");
}

/* Прототипы функций */
int make_socket();
void make_request(int sock_id, char *font, char *text);
void get_responce(int sock_id);
int get_prompt(int sock);
char *concat_args(char *args[], int *num);

int main(int argc, char *argv[]) {
	char *font = NULL, *text = NULL;	/* font - шрифт, text - текст для отображения*/
	int c = 0, sock_id = 0;					/* дескриптор сокета */
	
	
	/* если число аргументов меньше 5, вывести сообщение о работе программы и выйти */
	if(argc < 5) {
		usage();
		exit(-1);
	}
	
	/* "разбираем" переданные параметры */
	while ((c = getopt(argc, argv, "f:t:")) != -1){
		switch(c){
			case 'f':
				font = strdup(optarg); // копируем текст в переменную font 
				break;
			case 't':
				text = concat_args(argv, &argc); // копируем текст в переменную text 
				break;
			default:
				usage();
				exit(-1);
		}
	}
	
	
	sock_id = make_socket();					/* создать сокет */
	if(get_prompt(sock_id)) {					/* если приглашение получено */
		make_request(sock_id, font, text);		/* отправить запрос на сервер */
		get_responce(sock_id);					/* получить ответ */
	}
	
	free(text);
	shutdown(sock_id, 2);

	return 0;
}

/* отправить запрос на сервер */
void make_request(int sock_id, char *font, char *text) {
	int req_str_len = 0; 
	char *req_str = NULL;
	
	/* сумма длин строк = figlet + длина имени шрифта и текста + пробелы */
	req_str_len = strlen("figlet") + strlen(font) + strlen(text) + 4;
	req_str = (char *)malloc(req_str_len);
	sprintf(req_str, "figlet /%s %s\r\n", font, text);

	/* отправить запрос на сервер */
    ssize_t len = send(sock_id, req_str, req_str_len + 1, 0);
    if (len < 0) {
        perror("Failed to send data to server");
        exit(-1);
    }
    if (len == 0) {
        fprintf(stderr, "Connection was closed");
        exit(-1);
    }
    
    free(req_str);
}

/* прочитать ответ */
void get_responce(int sock_id) {
	ssize_t len;	
	
	while(1) {
		len = recv(sock_id, buf, BUFF_SIZE, 0);
		if (len == -1) {
			printf("ERROR! Can't receive data from server\n");
			exit(-1);
		}
		if (len == 0) {
			printf("ERROR! Remote Server close connection\n");
			exit(-1);
		}
		/* если предпоследний символ . , то мы получили ответ сервера */
		if (buf[len - 1] == '.') {
			buf[len - 1] = '\0';
			printf("%s\n", buf);
			break;
		}
		/* если последний символ новой строки а предпоследний . , то мы получили ответ сервера */
		if ((buf[len - 2] == '\n') && (buf[len - 1] == '.')) {
			buf[len - 1] = '\0';
			printf("%s\n", buf);
			break;
		}
		buf[len] = '\0';
        printf("%s", buf);
	}
}

/* Конкатенация аргументов */
char *concat_args(char *args[], int *num) {
	int len = 0, tmp = 0;
	char *str = NULL;
	
	/* подсчет длины каждого переданного аргумента */
	for(int i = 4; i < *num; i++) {
		len += strlen(args[i]);
	}
	/* количество аргументов = числу пробелов */
	len += (*num - 4);
	
	/* выделить память */
	if((str = (char *)malloc(len)) == NULL) {
		printf("ERROR! failed to allocate memory\n");
		exit(-1);
	}
	
	/* Каждый переданный аргумент записывается в участок памати на который указывает указатель */
	for(int i = 4; i < *num; i++) {
		snprintf(str + tmp, len - tmp, "%s", args[i]);
		tmp += strlen(args[i]);
		str[tmp++] = ' ';
	}
	str[tmp] = '\0';
	
	return str;
}

/* "прочитать" приглашение сервера */
int get_prompt(int sock) {
	ssize_t len;

	while(1) {
		len = recv(sock, buf, BUFF_SIZE, 0);
		if (len == -1) {
			printf("ERROR! Can't receive data from server\n");
			return 0;
		}
		if (len == 0) {
			printf("ERROR! Remote Server close connection\n");
			return 0;
		}
		/* если предпоследний символ . , то мы получили приглашение */
		if (buf[len - 1] == '.') {
			return 1;
		}
		/* если последний символ новой строки а предпоследний . , то мы получили приглашение */
		if ((buf[len - 2] == '\n') && (buf[len - 1] == '.')) {
			return 1;
		}
	}
}

/* создать сокет */
int make_socket() {
	int skt = 0;
	struct addrinfo hints, *result;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;
	
	/* получить IPv4 адрес сервера */
	if(getaddrinfo(SERVER, PORT, &hints, &result) != 0) {
		perror("ERROR! getaddrinfo()\n");
		exit(-1);
	}
	
	/* создать сокет */
	if((skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("ERROR! socket()\n");
		exit(-1);
	}	
	
	/* установить соединение */
	if(connect(skt, result->ai_addr, result->ai_addrlen) == -1) {
		perror("ERROR! connect()");
		close(skt);
		exit(-1);
	}

	freeaddrinfo(result);
	return skt;
}
