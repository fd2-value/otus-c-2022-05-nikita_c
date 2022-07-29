#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"


/* Структура student */
struct student {
	char *name;
	unsigned int age;
};

/* Проверка работы библиотеки */
int main()
{
	struct student Student[2];

	Student[0].name = "Alice";
	Student[0].age = 20;
	Student[1].name = "Bob";
	Student[1].age = 21;
	
	/* имя файла и режим работы w (write) */
	log_start("simple_log",'w');
	
	/* вывод различных сообщений */
	log_debug("%s", "Hello World!");
	log_info("%d %d %d", 123, 456, 789);
	log_warning("%s", Student[0].name);
	log_error("%s\n", Student[1]);
	
	log_stop();
	
	return 0;
}
