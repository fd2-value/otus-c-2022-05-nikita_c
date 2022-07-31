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

