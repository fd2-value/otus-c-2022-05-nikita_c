#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

/* https://linux.die.net/man/7/utf8
   функция кодирования номера символа в Юникоде в необходимую последовательность байт. Аргументы:
   s - указатель на элемент полученный из массива utf_table[256] соответствующих функций iso88595_to_utf8, koi8r_to_utf8, cp1251_to_utf8
   p - указатель на буфер для записи; len - длина октета. */
void to_utf8(uint16_t *s, uint8_t *p, uint8_t *len) {
	/* если код символа < 0x80, то записываем символ как есть (т.к. все вышеперечисленные кодировки совмести с ASCII до символа с кодом 0x80) */
	if (*s < 0x80) {
		p[0] = *s;
		/* октет равен 1 */
		*len = 1;
	}
	/* если код символа < 0x800, то необходимо выполнить преобразования символа из переменной s в последовательность байт вида (см. man выше):
	   0x00000080 - 0x000007FF:
			110xxxxx 10xxxxxx
		где x - биты символа из переменной s */
	else if (*s < 0x800) {
		/* сдвинем младшие 6 бит вправо и выполним побитовое ИЛИ для приведения числа к виду 110xxxxx */
		p[0] = *s >> 6 | 0xc0;
		/*  побитовое И для получения младших 6 бит, затем ИЛИ с 0x80 для приведения числа к виду 10xxxxxx */
		p[1] = (*s & 0x3f) | 0x80;
		/* октет равен 2 */
		*len = 2;
	}
}

/* функция преобразования iso-8859-5 в utf-8 с аргументами:
   c - указатель на считанный символ (см. вызов функции cp1251_to_utf8 в main) 
   p - указатель на буфер для записи байта utf символа (см. вызов функции cp1251_to_utf8 в main) 
   len - длина октета (см. вызов функции cp1251_to_utf8 в main) */
void iso88595_to_utf8(int *c, uint8_t *p, uint8_t *len) {
	/* https://ru.wikipedia.org/wiki/ISO_8859-5 массив шестнадцатеричных кодов подходящего символа в Юникоде.  */
	static const short utf_table[256] = {                    
		0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
		0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
		0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
		0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
		0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,
		0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,
		0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
		0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A, 0x40B, 0x40C, 0xAD, 0x40E, 0x40F,
		0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E, 0x41F,
		0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F,
		0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F,
		0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F,
		0x2116, 0x451, 0x452, 0x453, 0x454, 0x455, 0x456, 0x457, 0x458, 0x459, 0x45A, 0x45B, 0x45C, 0xA7, 0x45E, 0x45F
    };
    
    /* переменной s присваивается символ из массива utf_table (код символа в Юникоде) */
    uint16_t s = utf_table[*c];
    
    /* функция to_utf8 для преобразования кода символа utf в набор байт с аргументами:
		s - код символа из массива utf_table; p - указатель на буфер для записи байта utf символа;
		len - длина октета */
    to_utf8(&s, p, len);
}

/* функция преобразования koi8r в utf-8 с аргументами:
   c - указатель на считанный символ (см. вызов функции cp1251_to_utf8 в main) 
   p - указатель на буфер для записи байта utf символа (см. вызов функции cp1251_to_utf8 в main) 
   len - длина октета (см. вызов функции cp1251_to_utf8 в main) */
void koi8r_to_utf8(int *c, uint8_t *p, uint8_t *len) {
	/* https://en.wikipedia.org/wiki/KOI8-R массив шестнадцатеричных кодов подходящего символа в Юникоде.  */
	static const short utf_table[256] = {                    
		0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
		0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
		0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
		0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
		0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,
		0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,
		0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
		0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f, 0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524, 0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
		0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219, 0x221A, 0x2248, 0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
		0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556, 0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255D, 0x255E,
		0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565, 0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0xA9,
		0x44E, 0x430, 0x431, 0x446, 0x434, 0x435, 0x444, 0x433, 0x445, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E,
		0x43F, 0x44F, 0x440, 0x441, 0x442, 0x443, 0x436, 0x432, 0x44C, 0x44B, 0x437, 0x448, 0x44D, 0x449, 0x447, 0x44A,
		0x42E, 0x410, 0x411, 0x426, 0x414, 0x415, 0x424, 0x413, 0x425, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E,
		0x41F, 0x42F, 0x420, 0x421, 0x422, 0x423, 0x416, 0x412, 0x42C, 0x42B, 0x417, 0x428, 0x42D, 0x429, 0x427, 0x42A
    };
    
    /* переменной s присваивается символ из массива utf_table (код символа в Юникоде) */
    uint16_t s = utf_table[*c];

	/* функция to_utf8 для преобразования кода символа utf в набор байт с аргументами:
		s - код символа из массива utf_table; p - указатель на буфер для записи байта utf символа;
		len - длина октета */
	to_utf8(&s, p, len);
}

/* функция преобразования cp1251 в utf-8 с аргументами:
   c - указатель на считанный символ (см. вызов функции cp1251_to_utf8 в main) 
   p - указатель на буфер для записи байта utf символа (см. вызов функции cp1251_to_utf8 в main) 
   len - длина октета (см. вызов функции cp1251_to_utf8 в main) */
void cp1251_to_utf8(int *c, uint8_t *p, uint8_t *len) {
	/* https://ru.wikipedia.org/wiki/Windows-1251 массив шестнадцатеричных кодов подходящего символа в Юникоде.  */
	static const short utf_table[256] = {                    
		0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
		0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
		0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
		0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
		0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,
		0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,
		0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
		0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x402,0x403,0x201a,0x453,0x201e,
		0x2026,0x2020,0x2021,0x20ac,0x2030,0x409,0x2039,0x40a,0x40c,0x40b,0x40f,0x452,
		0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0,0x2122,0x459,0x203a,0x45a,
		0x45c,0x45b,0x45f,0xa0,0x40e,0x45e,0x408,0xa4,0x490,0xa6,0xa7,0x401,0xa9,0x404,
		0xab,0xac,0xad,0xae,0x407,0xb0,0xb1,0x406,0x456,0x491,0xb5,0xb6,0xb7,0x451,
		0x2116,0x454,0xbb,0x458,0x405,0x455,0x457,0x410,0x411,0x412,0x413,0x414,0x415,
		0x416,0x417,0x418,0x419,0x41a,0x41b,0x41c,0x41d,0x41e,0x41f,0x420,0x421,0x422,
		0x423,0x424,0x425,0x426,0x427,0x428,0x429,0x42a,0x42b,0x42c,0x42d,0x42e,0x42f,
		0x430,0x431,0x432,0x433,0x434,0x435,0x436,0x437,0x438,0x439,0x43a,0x43b,0x43c,
		0x43d,0x43e,0x43f,0x440,0x441,0x442,0x443,0x444,0x445,0x446,0x447,0x448,0x449,
		0x44a,0x44b,0x44c,0x44d,0x44e,0x44f
    };
	
	/* переменной s присваивается символ из массива utf_table (код символа в Юникоде) */
    uint16_t s = utf_table[*c];
   
	/* функция to_utf8 для преобразования кода символа utf в набор байт с аргументами:
		s - код символа из массива utf_table; p - указатель на буфер для записи байта utf символа;
		len - длина октета */
	to_utf8(&s, p, len);
}

/* вывод информации о запуске программы и необходимых аргументах для ее корректной работы */
void usage() {
		printf("USAGE: ./hw02 <source file> <encoding> <dest. file>\n");
		printf("<source file> - any text file existing in FS\n");
		printf("<encoding> - one of cp1251, koi8-r, iso-8859-5\n");
		printf("<dest. file> - new file name\n");
}

/* функция проверяет правильность переданных аргументов */
int check_arguments(int *elements, char *arguments[]){
	/* если число аргументов < 4, то вывеси сообщение об ошибке и вернуть 1 (ошибка) */
	if (*elements < 4){
		printf("Error! Not enough arguments:\n");
		usage();
		return 1;
	}
	/* если в качестве src. file передан файл, которого нет, то вывести ошибку и вернуть 1 (ошибка) */
	if (access(arguments[1], F_OK) != 0) {
		perror(arguments[1]);
		usage();
		return 1;
	}
	/* если аргумент отвечающий за название кодировки написан неправильно\неполно и т.п., то вывести сообщение об ошибке */
	if ( (strcmp(arguments[2],"cp1251") == 0) ||
		 (strcmp(arguments[2],"koi8-r") == 0) ||
		 (strcmp(arguments[2],"iso-8859-5") == 0) ) {
		return 0;
	}
	else {
		printf("Error! check <encoding> argument!\n");
		usage();
		return 1;
	}
	
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *fin, *fout;				/* указатели на файлы, fin - src. file. fout - dest. file */
	int c = 0, enc_t = 0;			/* c - очередной считаный символ из файла, enc_t - номер кодировки из которой будем декодировать в UTF-8  */
	uint8_t buffer[2], len = 0;		/* buffer[2] - записываем полученные байты UTF символа. len - длина октета */
	
	/* проверить переданные программе аргументы на правильность через функцию check_arguments 
	   если функция вернула -1, то аргументы переданые не все\неправильно и т.п.
		*/
	if (check_arguments(&argc, argv) == 1) {
		exit(1);
	}
	
	/* открыть файл argv[1] в режиме чтения
		показать ошибку, если произошли проблемы при открытии */
	if ((fin = fopen(argv[1], "r")) == NULL) { 
        perror("ERROR");
        exit(1);
    }
    
    /* открыть файл argv[3] в режиме записи
		показать ошибку, если произошли проблемы при открытии */
    if ((fout = fopen(argv[3], "w")) == NULL) { 
        perror("ERROR");
        exit(1);
    }
    
    /* выясним из какой кодировки нужно будет выполнять преобразования.
	   1 - cp1251, 2 - koi8-r, 3 - iso-8859-5. Соответствующую цифру запишем в enc_t */
    if (strcmp(argv[2],"cp1251") == 0) { enc_t = 1; }
	else if (strcmp(argv[2],"koi8-r") == 0) { enc_t = 2; }
	else { enc_t = 3; }
    
    /* пока не достигнем конца файла */
    while ((c = fgetc(fin)) != EOF) {
		/* в зависимости от типа кодировки вызываем соответствующую функцию со следующими аргументами:
		   c - очередной считанный символ; buffer - буфер для записи полученных байт; len - количество октетов */
		if (enc_t == 1) { cp1251_to_utf8(&c, buffer, &len); }
        else if (enc_t == 2) { koi8r_to_utf8(&c, buffer, &len); }
		else { iso88595_to_utf8(&c, buffer, &len); };

		/* если длина октета == 1, то записываем 1 байт полученного utf кода символа в файл */
		if (len == 1){
			fwrite(buffer, sizeof(uint8_t), 1, fout);
		}
		/* если длина октета == 2, то записываем 2 байта полученного utf кода символа в файл */
		if (len == 2) {
			fwrite(buffer, sizeof(uint8_t), 2, fout);
		}
	}
		
	fclose(fin);
	fclose(fout);
	
	return 0;
}