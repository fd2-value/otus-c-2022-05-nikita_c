#define BACKTRACE_BUFF_SIZE 1024											/* длина буфера для фреймов в стеке */
#define log_debug(...) write_log(0, __FILE__, __LINE__, __VA_ARGS__)		/* функции для логирования сообщения с заданными: */
#define log_info(...) write_log(1, __FILE__, __LINE__, __VA_ARGS__) 		/* индексом - индекс для соответствующего сообщения в лог файле */
#define log_warning(...) write_log(2, __FILE__, __LINE__, __VA_ARGS__)		/* __FILE__ - имя файла, где вызвалась функция логирования */
#define log_error(...) write_log(3, __FILE__, __LINE__, __VA_ARGS__)		/* __LINE__ - номер строки, где вызвалась функция */
																			/* __VA_ARGS__ - любой тект сообщения, который нужно записать в лог */

static const char *level[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};		/* массив, с уровнем важности сообщений */

void log_start(char *filename, char mode);									/* функция инициализации работы с лог файлом */
void log_stop();															/* окончание работы с лог файлом */

void write_log(int index, const char *file, int line, const char *fmt, ...); /* главная функция, для работы с лог файлом (записи) */
