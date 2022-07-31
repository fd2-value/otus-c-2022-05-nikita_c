### Домашнее задание №8
**1) Fix #1**. Одна из ошибок:
```sh
==6945== 5 bytes in 1 blocks are definitely lost in loss record 5 of 350
==6945==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==6945==    by 0x4264E4: strdup (strdup.c:24)
==6945==    by 0x4076F4: clib_package_install (clib-package.c:1384)
==6945==    by 0x402B8E: main (package-install.c:35)
```
Проблемы при освобождении ресурсов, выделенных функцией **strdup** встречаются и в других ситуациях:  
```sh
==6945== 7 bytes in 1 blocks are definitely lost in loss record 8 of 350
==6945==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==6945==    by 0x4264E4: strdup (strdup.c:24)
==6945==    by 0x4076F4: clib_package_install (clib-package.c:1384)
==6945==    by 0x404A0C: install_packages (clib-package.c:374)
==6945==    by 0x40809F: clib_package_install_dependencies (clib-package.c:1616)
==6945==    by 0x407F8A: clib_package_install (clib-package.c:1580)
==6945==    by 0x402EB5: main (package-install.c:56)
```
В файле **clib-package.c:1384** вызывается функция hash_set (hash.c):  
```sh
hash_set(hash_t *self, char *key, void *val)
```
Один из параметров - это ключ **key**, который получается в результате работы функции **strdup**.  
Внутри функции **hash_set** вызывается функция (внутри макроса KHASH_INIT - khash.h) **kh_put**: 
```sh
kh_put(ptr, self, key, &ret)
```
В нее передается параметр **key**. В определенных ситуациях она устанавливает значение переменной **ret=0** и параметр **key** не освобождает ресурсы.  
Сам patch:  
```sh
--- hash.c	2022-07-31 23:19:09.137616859 +0500
+++ hash_fix.c	2022-07-31 23:21:50.322636014 +0500
@@ -16,6 +16,7 @@
   int ret;
   khiter_t k = kh_put(ptr, self, key, &ret);
   kh_value(self, k) = val;
+  if(ret == 0) { free(key); }
 }
 
 /*
 ```
**1) Fix #2**. Одна из ошибок:
```sh
==8003== 94 (64 direct, 30 indirect) bytes in 2 blocks are definitely lost in loss record 60 of 344
==8003==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==8003==    by 0x4203F9: http_get_shared (http-get.c:46)
==8003==    by 0x4057D3: clib_package_new_from_slug_with_package_name (clib-package.c:660)
==8003==    by 0x405E02: clib_package_new_from_slug (clib-package.c:797)
==8003==    by 0x40296F: main (package-install.c:23)
 ```
 В файле **clib-package.c:660** указателю **res** на структуру **http_get_response_t** присваивается результат выполнения функции **http_get_shared (файл - http-get.c)**:
 ```sh
 res = http_get_shared(json_url, clib_package_curl_share);
  ```
  При определенных условиях программа может перейти к метке **download**, yt 
