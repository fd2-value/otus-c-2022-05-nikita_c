#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

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
	long int fileposition = 0; /* позиция в файле */
	
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
        perror("ERROR");
        exit(1);
    }
    
    /* читаем 4 элемента по 1 байт в буфер */
    while ((bytes = fread(buffer, sizeof(uint8_t), 4, file)) > 0) {
		/* Проверяем, не являются ли считанные нами байты сигнатурой начала Local File Header  */
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
						exit(1);
					}
					/* Читаем имя файла */
					if (fread((char *)filename, lfh.filenameLength, 1, file) == 0) {
						printf("ERROR! Failed to initialize pointer\n");
						exit(1);
					}
					/* Выводим его в STDOUT */
					fputs(filename, stdout);
					putchar('\n');
					
					free(filename);
				}
		}
		/* Проверяем, не являются ли считанные байты сигнатурой начала JPEG файла */
		if (buffer[0] == 0xff && 
		    buffer[1] == 0xd8 && 
		    buffer[2] == 0xff) {
				/* Если первые 3 байта говорят о том, что перед нами может быть JPEG файл, то
				   немедленно считать с конца последние 2 байта файла */  
				fflush(file);
				fileposition = ftell(file);
				fseek(file, -2, SEEK_END);
				if (fread(buffer, sizeof(uint8_t), 2, file) == 0) {
					printf("ERROR! Failed to read data from file\n");
					exit(1);
				}
				/* Сигнатура окончания JPEG файла */
				if (buffer[0] == 0xff && 
					buffer[1] == 0xd9) {
					/* перед нами JPEG файл */
					printf("This is JPEG file!\n");
					exit(0);
				}
				else {
					/* перед нами не JPEG файл, возвращаем индикатор позиции на прежнее место */
					fseek(file, fileposition, SEEK_SET);
				}
		}
	}
	
	/* Если у нас чистый JPEG файл, то выполнение программы НЕ дойдет до этого участка программы.
	   Если у нас JPEGZIP, то сигнатура начала у нас хотя бы для 1 файла будет проинициализирована.
	   Если ничего из предидущих двух пунктов НЕ произошло, то перед нами другой файл. */
	if (!lfh.versionToExtract) {
			printf("This is not JPEGZIP or JPEG file\n");
	}
	
	fclose(file);
	
	return 0;
}
