

#ifndef DISCOH
#define DISCOH

#include <stdio.h>
#include <stdlib.h>
#define sizeChar  50

//Funciones
void inicializarListaMount();
void imprimirListaDeParticionesMontadas() ;
//consola
void mkfs(char id[sizeChar], char unit, char type[sizeChar],  int add);
void mkdisk(int size, char ruta[sizeChar], char nombre[sizeChar]);
void rmkdisk(char ruta[sizeChar]);
void fdisk(int size, char path[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add);
void mountn(char ruta[sizeChar],char nombre[sizeChar]);


//reportes
void reporteMBR(char ruta[sizeChar]) ;
void reporteEBR(char ruta[sizeChar]) ;
void sb_reporte(char id[sizeChar]);

#endif
