

#ifndef DISCOH
#define DISCOH

#include <stdio.h>
#include <stdlib.h>
#define sizeChar  50

//consola
void mkdisk(int size, char ruta[sizeChar], char nombre[sizeChar]);
void rmkdisk(char ruta[sizeChar]);
void fdisk(int size, char path[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add);

//reportes
void reporteMBR(char ruta[sizeChar]) ;
#endif
