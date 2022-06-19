#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

// Спасибо автору блога https://blog2k.ru/archives/3391 за описание структуры
struct LocalFileHeader
{
    // Минимальная версия для распаковки
    uint16_t versionToExtract;
    // Битовый флаг
    uint16_t generalPurposeBitFlag;
    // Метод сжатия (0 - без сжатия, 8 - deflate)
    uint16_t compressionMethod;
    // Время модификации файла
    uint16_t modificationTime;
    // Дата модификации файла
    uint16_t modificationDate;
    // Контрольная сумма
    uint32_t crc32;
    // Сжатый размер
    uint32_t compressedSize;
    // Несжатый размер
    uint32_t uncompressedSize;
    // Длина название файла
    uint16_t filenameLength;
    // Длина поля с дополнительными данными
    uint16_t extraFieldLength;
} __attribute__((packed));

int main(int argc, char *argv[]) {
	FILE *file;			/* указатель на файл, с которым будем работать */ 
	uint8_t buffer[4];	/* буфер для хранения сигнатуры начала Local File Header */
	size_t bytes = 0;	/* количество считанных байт из файла */
	char *filename;
	
	struct LocalFileHeader lfh; /* переменная lfh типа LocalFileHeader */
	
	/* если в качестве аргумента не передали имя файла, то показать сообщение об ошибке и выйти */
	if (argc == 1) {
		printf("ERROR! Not enough arguments! See below\n");
		printf("usage: ./hw01 [filename]\n");
		exit(1);
	}
	
	/* открыть файл argv[1] в режиме чтения бинарного файла
		показать ошибку, если произошли проблемы при открытии */
	if ((file = fopen(argv[1], "rb")) == NULL) { 
        printf("ERROR! Can't open file!\n");
        exit(1);
    }
    
    /* читаем 4 элемента по 1 байт в буфер */
    while ((bytes = fread(buffer, sizeof(uint8_t), 4, file)) > 0) {
		/* Проверяем, не являются ли считанные нами байты сигнатурой начала Local File Header */
		if (buffer[0] == 0x50 && 
			buffer[1] == 0x4b && 
			buffer[2] == 0x03 && 
			buffer[3] == 0x04) {
				/* Читаем структуру целиком */
				if (fread((char *) &lfh, sizeof(lfh), 1, file) == 0) {
					printf("ERROR! Failed to initializate LocalFileHeader struct\n");
				}
				
				/* Если имя файла != 0 */
				if (lfh.filenameLength) {
					/* Выделим память размером с длину имени файла */
					filename = (char *)malloc(lfh.filenameLength);
					if (filename == NULL) { 
						printf("ERROR! Failed to allocate memory\n");
					}
					/* Читаем имя файла */
					if (fread((char *)filename, lfh.filenameLength, 1, file) == 0) {
						printf("ERROR! Failed to initialize pointer\n");
					}
					/* Выводим его в STDOUT */
					fputs(filename, stdout);
					putchar('\n');
				}
		}
	}
	
	/* Если за предыдущий цикл не инициалировался указатель с именем файла,то 
		можно сделать вывод, что это файл отличный от ZIPJPEG */
	if(!strlen(filename)) {
		printf("NOT a ZIPJPEG file!\n");
		exit(0);
	}
	
	free(filename);
	fclose(file);
	
	return 0;
}
