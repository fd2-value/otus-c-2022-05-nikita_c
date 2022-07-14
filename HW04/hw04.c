#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include "memory.h"
#include "libs/cJSON.h"

// Сформировать и сделать запрос на указанный url
CURLcode request(char *url, struct mem *chunk) {
	CURL *curl_handle;							/* указатель яв-ся "Ручкой" для работы с функциями библиотеки libcurl */
	CURLcode res;								/* переменная типа CURLcode для хранения кода ошибки */
	struct curl_slist *content_type = NULL;		/* указатель для добавления необходимых параметров в тело запроса */	 
	
	/* инициализация указателя для работы с функциями библиотеки libcurl, иначе ошибка */
	curl_handle = curl_easy_init();
	if(curl_handle == NULL) {
		printf("ERROR! Failed to initialize cURL handle!\n");
		exit(1);
	}
	else {
		/* Добавить в связный список строки(параметры) для запроса */
		content_type = curl_slist_append(content_type, "Content-Type: application/json");
		content_type = curl_slist_append(content_type, "charset: utf-8");
		
		/* Установка опций для задания "поведения" libcurl 
		 * url - адрес сервера для запроса
		 * content_type - тип передаваемого контента
		 * write_data_callback - вызывается, когда libcurl получит какие-либо данные
		 * chunk - указатель на структуру передаваемый 4 аругментом в функцию write_data_callback
		 * ibcurl-agent/1.0 - user-agent
		 * */
		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, content_type);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data_callback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        /* выполнить запрос */
        res = curl_easy_perform(curl_handle);
        
        /* закрыть все соединения и "очистить ресурсы" */ 
        curl_easy_cleanup(curl_handle);
        return res;
	}
	
	return CURLE_FAILED_INIT;
}

/* Функция для перевода букв в слове в нижний регистр */
void to_lowercase(char *s) {
	int i = 0;
	/* каждый символ в слове в нижний регистр */
	for(i = 0; s[i]; i++){
		s[i] = tolower(s[i]);
	}
}

/* Функция для создания запроса */
char *make_url(char *city_name) {
	char *server_name = "https://wttr.in/";	/* адрес сервера */
	char *params = "?format=j1";			/* параметр запроса */
	char *url = NULL;						/* укзатель на полный сформированный запрос */
	
	/* Выделить память размером strlen(server_name) + strlen(city_name) + strlen(params) + 1 байт, иначе ошибка */
	url = malloc((strlen(server_name) + strlen(city_name) + strlen(params) + 1) * sizeof(char));
	if (url == NULL) {
		printf("ERROR! failed to allocate memory for url\n");
		exit(1);
	}
	
	/* копирование strlen(server_name) байт в указатель url*/
	memcpy(url, server_name, strlen(server_name));
	/* копирование strlen(city_name) байт в указатель url смещенный на url + strlen(server_name) байт и т.д.*/
	memcpy(url + strlen(server_name), city_name, strlen(city_name));
	memcpy(url + strlen(server_name) + strlen(city_name), params, strlen(params) + 1);
	 
	return url;
}

int main(int argc, char *argv[]) {
	char *url = NULL; 									/* указатель содержит URL сервера для запроса */
	struct mem chunk;									/* переменная для работы с данными получеными в качестве ответа от сервера */
	CURLcode res;										/* переменная для вывода подробного описания ошибки при работе с функциями из libcurl */
    /* указатели на структуру cJSON для работы с библиотекой cJSON
     * root - указатель на "разобраный" JSON
     * item, item_next - очередной элемент полученный при обходе JSON
     * object - объект для поиска в JSON
     *  */
    cJSON *root, *item, *item_next, *object;
    char *space = " "; /* пробел в качестве заглушки, если значние у ключа нет */
	
	/* Число аргументов меньше чем нужно? Сообщение об ошибке, информация для запуска программы и выход из программы */
	if (argc < 2) {
		printf("ERROR! Not enough arguments! See below:\n");
		printf("USAGE:\t./hw04 <City name>\n");
		printf("\t<City name> - any city that you want\n");
		exit(1);
	}
		
	/* Буквы в слове к нижнему регистру */
	to_lowercase(argv[1]);
	
	/* сформировать URL */
	url = make_url(argv[1]);
	
	/* выделить 1 байт, необходимый объем памяти будет выделен при получении данных (см. memory.c) */
	chunk.memory = malloc(1);
	if (chunk.memory == NULL) {
		printf("ERROR! failed to allocate memory for url\n");
		exit(1);
	}
	chunk.size = 0;
	
	/* выполнить запрос, в случае ошибки вывести описание и выйти*/
	res = request(url, &chunk);
	if(res != CURLE_OK) {
		printf("ERROR! Failed to send request! Reason: %s\n", curl_easy_strerror(res));
		exit(1);
	}
	
	/* "Разбор" JSON, root - указатель на JSON, выход в результате ошибки */
	root = cJSON_Parse(chunk.memory);
	if(root == NULL) {
		const char *error = cJSON_GetErrorPtr();
		if (error != NULL) {
			printf("ERROR! %s\n", error);
		}
		exit(1);
	}
	
	/* https://wttr.in - возвращает объект, с массивом объектов внутри
	 * ищем "current_condition" */
	object = cJSON_GetObjectItemCaseSensitive(root, "current_condition");
	/* по всем ключам в объекте */
    cJSON_ArrayForEach(item, object) {
		printf("\t*** Current weather in %s ***\n", argv[1]);

		/* получить и вывести текущую температуру, valuestring - поле структуры cJSON */
        cJSON *temp_C= cJSON_GetObjectItemCaseSensitive(item, "temp_C");
		printf("\tTemperature: %sC\n", (temp_C->valuestring == NULL) ? space : temp_C->valuestring);
		
		/* найти строку с текущей температурой */
		cJSON *weatherDesc = cJSON_GetObjectItemCaseSensitive(item, "weatherDesc");
		/* пройтись по объектам в массиве */
		cJSON_ArrayForEach(item_next, weatherDesc){
			/* найти строку value и вывести значение */
			cJSON *weatherDesc_value = cJSON_GetObjectItemCaseSensitive(item_next, "value");
			printf("\tWeather description: %s\n", (weatherDesc_value->valuestring == NULL) ? space : weatherDesc_value->valuestring);
		}	
		
		/* найти строку winddir16Point и вывести значение */
		cJSON *winddir = cJSON_GetObjectItemCaseSensitive(item, "winddir16Point");
		printf("\tWind direction: %s\n", (winddir->valuestring == NULL) ? space : winddir->valuestring);
		
		/* найти строку windspeedKmph и вывести значение */
		cJSON *windspeed = cJSON_GetObjectItemCaseSensitive(item, "windspeedKmph");
		printf("\tWind speed (kmph): %s\n", (windspeed->valuestring == NULL) ? space : windspeed->valuestring);		
    }
    
    /* очистить выделенную память при работе с JSON, chunk и url */
	cJSON_Delete(root);
	free(chunk.memory);
	free(url);

	return 0;
}
