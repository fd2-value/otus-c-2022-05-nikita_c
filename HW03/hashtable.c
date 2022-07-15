#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hashtable.h"

#define CANARY_VALUE 0xFFEE
#define ARRAY_SIZE 5

#define FNV_OFFSET 14695981039346656037U
#define FNV_PRIME 1099511628211U

/* https://ru.wikipedia.org/wiki/FNV - простая хеш функция */
uint64_t getHash(const char* key) {
	uint64_t hash = FNV_OFFSET;
	
    for(const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
        
    return hash;
}

/* Создать "массив" arraySize элементов */
ht_array createArray(int arraySize) {
	/* Выделить память под структру struct array, в случае ошибки выйти */
	ht_array hashTableArray = malloc(sizeof(struct array));
	if(hashTableArray == NULL) {
		printf("ERROR! Failed to allocate memory for hash table!");
		exit(1);
	}
	
	/* присвоить число элементов в массиве */
	hashTableArray->items = arraySize;
	hashTableArray->trashvalue = 0;
	
	/* выделить память под arraySize элементов размера struct node, в случае ошибки выйти */
	hashTableArray->node = calloc(arraySize, sizeof(node));
	if(hashTableArray->node == NULL) {
		printf("ERROR! Failed to allocate memory for hash table entry!");
		exit(1);
	}
	
	return hashTableArray;
}

/* удалить (освободить память) созданную хеш таблицу */
void deleteArray(ht_array hashTableArray) {
	if (hashTableArray != NULL) {
		/* в цикле по числу элементов в массиве */
		for(int i = 0; i < hashTableArray->items; i++){
			/* если очередной элемент не пустой */
			if(hashTableArray->node[i] != NULL){
				/* "удалить" ключ */
				free(hashTableArray->node[i]->key);
				/* "удалить" элемент */
				free(hashTableArray->node[i]);
			}
		}
		/* "удалить" саму хеш таблицу */
		free(hashTableArray->node);
		free(hashTableArray);	
	}
}

/* функция расширения массива, если места для новых элементов больше нет */
void expandArray(ht_array hashTableArray) {
	/* создаем новую хеш таблицу размера: hashTableArray->items * 2 (Википедия рекомендуют увеличивать сразу вдвое) */
	ht_array newArray = createArray(hashTableArray->items * 2);
	
	/* подготовка элементов к копированию */
	for(int i = 0; i < hashTableArray->items; ++i){
		/* очередной элемент присвоить временной переменной */
		node element = hashTableArray->node[i];
		if(element == NULL) {
			continue;
		}
		/* Очистить ключ, если есть он равен CANARY_VALUE */
		if(element->value == CANARY_VALUE) {
			free(element->key);
			free(element);
			continue;
		}
		/* добавляем элемент в массив */
		addItem(newArray, element);
	}
	/* очистить указатель на node */
	free(hashTableArray->node);
	/* присвоить указатель на новый адрес элемента */
	hashTableArray->node = newArray->node;
	/* скопировать остальные значения */
	hashTableArray->items = newArray->items;
	hashTableArray->trashvalue = newArray->trashvalue;
	free(newArray);
}

/* поиск индекса ключа */
uint64_t searchKeyIndex(ht_array hashTableArray, char *key) {
	/* вычислить хеш от ключа (не более hashTableArray->items) */
	uint64_t index = getHash(key) % hashTableArray->items;
	//uint64_t index = (getHash(key) + SHIFT) % hashTableArray->items;
	
	/* пока есть элементы в массиве и значение ключа не равно CANARY_VALUE, т.е. выполнить линейное пробирование */
	while ((hashTableArray->node[index] != NULL) && (hashTableArray->node[index]->value != CANARY_VALUE)) {
		/* присвоить очередной элемент массива временной переменной element */
		node element = hashTableArray->node[index];
		/* ключи равны, мы нашли индекс */
		if(strcmp(element->key, key) == 0) {
			break;
		}
		/* иначе продолжаем поиск */
		index = (index + 1) % hashTableArray->items;
	}
	return index;
}

/* добавить элемент в массив */
void addItem(ht_array hashTableArray, node element){ 
	/* если заполняемость таблицы >= количества элементов * 0.5 (коэффициент взят из Википедии) */
	if (hashTableArray->trashvalue >= hashTableArray->items * 0.5) {
		/* увеличить хеш таблицу */
		expandArray(hashTableArray);
	}
	
	/* найти индекс ключа */
	uint64_t index = searchKeyIndex(hashTableArray, element->key);
	/* если элемент не пустой */
	if (hashTableArray->node[index] != NULL) {
		/* удалить ключ и сам элемент */
		free(element->key);
		free(hashTableArray->node[index]);
	}
	/* коэф. заполняемости увеличить */
	else {
		++hashTableArray->trashvalue;
	}
	/* присвоить элемент */
	hashTableArray->node[index] = element;
}

/* присвоить ключ элементу массива */
node setItem(char *key) {
	/* выделить память, в случае ошибки выход */
	node element = malloc(sizeof(struct node));
	if(element == NULL){
		printf("ERROR! Failed to allocate memory for element in array %s\n", key);
		exit(1);
	}
	/* присвоить ключ */
	element->key = key;
	element->value = 0;
	
	return element;
}

/* получить элемент массива */
node getItem(ht_array hashTableArray, char *key) {
	/* посчитать индекс ключа */
	uint64_t index = getHash(key) % hashTableArray->items;
	
	/* пока элемент масивы не пустой, сравнить ключ в текущем элементе и ключ полученный функцией аргументом */
	while(hashTableArray->node[index] != NULL){
		if(strcmp(hashTableArray->node[index]->key, key) == 0) {
			break;
		}
		/* линейное пробирование */
		index = (index + 1) % hashTableArray->items;
	}
	return hashTableArray->node[index];
}

/* удалить элемент из массива */
void deleteItem(ht_array hashTableArray, char* key) {
	/* найти индекс ключа */
	uint64_t index = searchKeyIndex(hashTableArray, key);
	/* присвоить элемент временной переменной */
	node element = hashTableArray->node[index];
	/* если она не пустая присвоить временное значение */
	if (element != NULL) {
		element->value = CANARY_VALUE;
	}
}

/* вывести на экран все элементы хеш таблицы */
void printItems(ht_array hashTableArray) {
	/* если функция printItems была вызвана до создания хеш-таблицы, вывести сообщение об ошибке*/
	if (hashTableArray == NULL) {
		printf("ERROR! Hash-table is empty! create one first!");
		exit(1);
	}
    
    printf("---------- HASH TABLE ----------\n");
    /* Если очредной элемент массива не пустой, то вывести пару ключ - значение на экран */
    for (int i = 0; i < hashTableArray->items; i++) {
        if (hashTableArray->node[i] != NULL)
            printf("\t%s: %d\n", hashTableArray->node[i]->key, hashTableArray->node[i]->value);
    }
     printf("-------------------------------\n");
}
