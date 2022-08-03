#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <execinfo.h>
#include "log.h"

FILE *fout;																	/* дескриптор для записи сообщений в лог файл */
#define BACKTRACE_BUFF_SIZE 1024											/* длина буфера для фреймов в стеке */
static const char *level[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};		/* массив, с уровнем важности сообщений */

/* вывод стек вызовов */
int print_backtrace() {
	void *array[BACKTRACE_BUFF_SIZE];		
	char **strings;
	int size, i;

	size = backtrace(array, BACKTRACE_BUFF_SIZE); 	/* возвращает число активных вызовов функций */
	strings = backtrace_symbols(array, size);		/* "переводит" адреса в массив строк, описывающих адреса */
	if (strings == NULL) {							
        perror("ERROR:");
        return 1;
    }
	
	/* Вывод числа фреймов и каждого из них в файл */
	fprintf(fout, "Stack frames: %d\n", size);
	for (i = 0; i < size; i++) {
		fprintf(fout, "\t%s\n", strings[i]);
	}
	
	free(strings);	
	return 0;
}

/* функция проверки переданных аргументов */
int check_arguments(char *filename, char *mode) {
	/* Если доступ к файлу отсутствует и передан ключ 'a' (append), то вывести сообщение об ошибке */
	if ((access(filename, F_OK) != 0) && (*mode == 'a')) {
		perror(filename);
		return -1;
	}
	/* Если ничего не передано, то вывести cообщение об ошибке */
	if ((!strlen(filename)) || (!strlen(mode))) {
		printf("ERROR! Empty argument(s) in function check_arguments\n");
		return -1;
	}
	/* Если режим НЕ равен 'w' или 'a', то сообщение об ошибке */
	if ((*mode == 'w') || (*mode == 'a')) { 
		return 0;
	}
	else {
		printf("ERROR! Wrong mode: %s\n", mode);
		printf("Possible values: w - new file\n\t\ta - append to existing file\n");
		return -1;	
	}
}

/* функция инициализации работы с лог файлом */
void log_start(char *filename, char *mode) {
	/* проверка переданных значений */
	if (check_arguments(filename, mode) == -1) {
		exit(1);
	}
	/* открыть файл, с соответствующим режимом */
	if ((fout = fopen(filename, mode)) == NULL) {
		perror("ERROR! Can't open file.\n");
		exit(1);
	}
}

/* Закрыть дескриптор файла */
void log_stop() {
	if(fout) { fclose(fout); }
}

/* функция записи сообщения в лог файл */
void write_log(int index, const char *file, int line, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	
	/* fprintf - выводит сообщение о том, в каком файле и на какой строке появилась ошибка */
	/* vfprintf - выводит непосредственно сообщение в файл */
	/* если вывод в файл одной из функций завершился с ошибкой, то вывод информации об ошибке */
	if((fprintf(fout, "\n%s %s:%d: ", level[index], file, line) < 0) || (vfprintf(fout, fmt, args) < 0)) {
		printf("ERROR! Failed write to file\n");
	}
	/* Если индекс == 3 (ERROR), то в файл вывести текущий стек вызовов в файл */
	if (index == 3) {
		if (print_backtrace()) { exit(1); }
	}
	
	va_end(args);
}
