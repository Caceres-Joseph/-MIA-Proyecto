/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fFdisk.h
 * Author: joseph
 *
 * Created on 16 de junio de 2017, 11:39 AM
 */

#ifndef FFDISK_H
#define FFDISK_H
#include "estructuras.h"

char metodoDeColocacionExtendida;
//char metodoDeColocacionExtendida = 'w';
bool fdisk_ocurrioError ;
//bool fdisk_ocurrioError = false

void inicializarVarFdisk() ;


void fdisk(int size, char path[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delet[sizeChar], int add);

//
partition devolverParticion(char ruta[sizeChar], char nombre[sizeChar]);
bloqueEBR devolverLogica(char ruta[sizeChar], char nombre[sizeChar]);
void crearParticion(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delet[sizeChar], int add);
void eliminarParticion(char ruta[sizeChar], char name[sizeChar], char delet[sizeChar]);
void crearParticionLogica(int size, char ruta[sizeChar], char name[sizeChar], char fit[sizeChar]);

#endif /* FFDISK_H */

