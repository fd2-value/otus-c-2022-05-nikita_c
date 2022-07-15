#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "hashtable.h"

#define BUFF_SIZE 1024
#define HASH_TABLE_ARRAY_SIZE 10

/* Добавить слово в хеш таблицу. Аргументы: 
 * hashTableArray - массив элементов
 * buf - указатель на массив символов
 * length - длина символов */
void addWordToArray(ht_array hashTableArray, char *buf, int length){
	if(length == 0) return;
	
	node element = NULL;
	/* Копировать строку в буфер длиной length, ошибка в cлучае  */
	char *word = strndup(buf, length);
	if (word == NULL) {
		printf("ERROR! Failed to allocate memory\n");
		exit(1);
	}
	
	/* найти элемент в хеш-таблице */
	element = getItem(hashTableArray, word);
	/* нашли увеличить значение */
	if(element != NULL) {
		element->value++;
		free(word);
	}
	/* Добавить слово в качестве ключа, установить значение в 1 и добавить в хеш-таблицу в качестве элемента */
	else {
		element = setItem(word);
		element->value = 1;
		addItem(hashTableArray, element);
	}
}

int main(int argc, char* argv[]) {
    FILE* fin;
    char buf[BUFF_SIZE], c;
    int length = 0;
    
    if (argc == 1) {
		printf("ERROR! Not enough arguments! See below\n");
		printf("usage: ./hw03 [filename]\n");
		exit(1);
	}
    
    /* Открыть файл на чтение, если не удалось, вывести ошибку */
    fin = fopen(argv[1], "r");
    if (fin == NULL) {
		perror("ERROR");
		exit(1);
	}
    
    /* Создать ассоциативный массив (по сути структура в которой содержаться еще N-е чисkj структур) */
    ht_array hashTableArray = createArray(HASH_TABLE_ARRAY_SIZE);

	/* считать очередной символ из файла */
	while ((c = fgetc(fin)) != EOF) {
		/* есди очередной символ пробел, новая строка и т.п. ИЛИ знак препинания, внести элемент в массив */
        if ((isspace(c)) || (ispunct(c))) {
			/* Добавить слово в массив */
			addWordToArray(hashTableArray, buf, length);
			length = 0;
			continue;
		}
		/* Длина слова >= BUFF_SIZE (1024), добавить слово в массив
		 * Если действительное в каком-либо языке есть такие слова, то добавленное слово может получиться "обрезанным"*/
		 if (length >= BUFF_SIZE) {
			addWordToArray(hashTableArray, buf, length);
			length = 0;
		 }
		 /* очередная буква в буфер */		 
		 buf[length] = c;
		 length++;
	}
	addWordToArray(hashTableArray, buf, length);

	/* Вывести все элементы */
	printItems(hashTableArray);
    /* Удалить ассоциативный массив */
    deleteArray(hashTableArray);

    fclose(fin);
    return 0;
}
