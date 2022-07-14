#include <stdio.h>
#include "memory.h"

size_t write_data_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	/* сколько реально получено функцией curl_easy_setopt и соотвенственно передано в функцию write_data_callback данных */
	size_t realsize = size * nmemb;
	/* приводим указатель типа void к типу struct memory_node */
	struct mem *mem = (struct mem *)userp;
	
	/* выделить память размера mem->size + realsize + 1 байт, иначе показать ошибку  */
	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
			printf("ERROR! Not enough memory\n");
			exit(1);
	}
	
	/* адрес начала выделенного блока присвоить переменной memory */
	mem->memory = ptr;
	/* скопировать realsize байт contents в адрес mem->memory[mem->size] */
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	/* усановить актуальный размер данных на realsize */
	mem->size += realsize;
	/* поставить в конце 0 */
	mem->memory[mem->size] = 0;
 
	return realsize;
}
