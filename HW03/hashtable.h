/* Структура описывающая элемент ассоциативного массива. Ключ и значение. */
struct node {
    char *key; 
    int value;
};

typedef struct node *node;

/* Структура содержащая фактически массив структур */
struct array {
    node *node;
	int items;
    int trashvalue;
};

typedef struct array *ht_array;

/* Что типа API для работы */

/* Создать массив с элементами */
ht_array createArray(int arraySize);
/* Удалить массив с элементами */
void deleteArray(ht_array hashTableArray);
/* добавить элемент в массив */
void addItem(ht_array hashTableArray, node element);
/* присвоить ключ элементу массива */
node setItem(char* key);
/* получить элемент массива */
node getItem(ht_array hashTableArray, char *key);
/* удалить элемент из массива */
void deleteItem(ht_array hashTableArray, char* key);
/* вывести на экран все элементы хеш таблицы */
void printItems(ht_array hashTableArray);
