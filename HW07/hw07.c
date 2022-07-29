#include <stdio.h>
#include <stdlib.h>

/* элемент списка */
struct node {
	struct node *next;							 	/* следующий элемент */
	int element;									/* значение элемента */
};

typedef struct node Node;							/* переменная Node типа struct node */
Node *root = NULL;									/* основной список (формируется после работы функции add_element) */
Node *new_root = NULL;								/* доп. список (формируется после работы функции f aka filter) */
char *empty_str = " ";								/* пустая строка (для puts) */
char *int_format = "%ld ";							/* шаблон вывода */
int data[6] = {4, 8, 15, 16, 23, 42};				/* массив с числами для добавления в список */
unsigned int data_length = 6;						/* длина массива */

Node *add_element(Node *next, int cur);				/* функция добавления элемента массива в список */
void print_int(Node *node);							/* вывод элемента списка */
void m(Node *node);									/* MAP итерирование по списку и вывод его элементов */
void f(Node *node);									/* FILTER отсеивание элементов, не подходящих под условие */
int p(int element);									/* функция условия для f() */
void global_free(Node *node);						/* очистка ранне выделеннной памяти */

int main(){
	int i = data_length - 1;						/* индекс с 0 до 5 */
	
	while(i >= 0){									/* добавление элементов в список начиная с конца массива */
		root = add_element(root, data[i]);
		i--;
	}
	
	m(root);										/* вызов функции m */
	puts(empty_str);								/* вывод пустой сроки */
	f(root);										/* вызов функции f */
	m(new_root);									/* вызов функции m */
	puts(empty_str);								/* вывод пустой сроки */
	
	global_free(root);								/* очистка ранне выделеннной памяти под список root */
	global_free(new_root);							/* очистка ранне выделеннной памяти под список new_root */
	return 0;
}

/* очистка ранне выделеннной памяти */
void global_free(Node *node){
	Node* next = NULL;
    /* очитска очередного элемента списка */
    while(node) {
        next = node->next;
        free(node);
        node = next;
    }
}

/* функция условия для f() */
int p(int element){
	/* побитовое AND */
	return (element & 1);
}

/* FILTER отсеивание элементов, не подходящих под условие */
void f(Node *node){
	/* если node пустая, выход из функции*/	
	if(node == NULL) {
		return;
	}
	/* если результат работы функции 0, переходим к следующему элементу */
	if(p(node->element) == 0) { 
		f(node->next);
	}
	/* отсеивание сработало, добавляем элемент в новый список */
	else {
		new_root = add_element(new_root, node->element);
		f(node->next);
	}
}

/* MAP итерирование по списку и вывод его элементов */
void m(Node *node){
	/* если node пустая, выход из функции*/
	if(node == NULL) {
		return;
	}
	/* вывод на экран очередного элемента списка */
	print_int(node);
	/* переход к следующему элементу */
	m(node->next);
}

/* вывод элемента списка */
void print_int(Node *node){
	/* вывод на экран очередного элемента списка, шаблон формата вывода в переменной int_format */
	printf(int_format, node->element);
	fflush(stdout);
}

/* функция добавления элемента массива в список */
Node *add_element(Node *next, int cur) {
	/* выделение 16 байт, в случае неудачи abort() */
	Node *node = malloc(16);
	if(node == NULL) {
		abort();
	}
	
	/* присвоить элемент масива к списку */
	node->element = cur;
	/* ссылка на следующий элемент */
	node->next = next;
	
	return node;
}
