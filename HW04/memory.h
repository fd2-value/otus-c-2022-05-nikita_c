#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* структура для работы с получеными данными библиотеки libcurl */
struct mem {
    char *memory;
    size_t size;
};

/* прототип функции для записи полученных данных в поля структуры mem */
size_t write_data_callback(void *contents, size_t size, size_t nmemb, void *userp);
