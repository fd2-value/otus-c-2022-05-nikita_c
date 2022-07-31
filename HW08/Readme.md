### Домашнее задание №8
**1) Fix #1**. Вывод ошибки:
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
