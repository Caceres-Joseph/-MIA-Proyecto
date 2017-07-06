/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fRep.h
 * Author: joseph
 *
 * Created on 19 de junio de 2017, 05:44 AM
 */

#ifndef FREP_H
#define FREP_H
#include "estructuras.h"
void rep(char id[sizeChar], char ruta[sizeChar], char name[sizeChar]);

//reporetes
void reporteMbr(char id[sizeChar]);

void exec(char ruta[sizeChar]);

void reporteEbr(char id[sizeChar]);
void reporteDisk(char id[sizeChar]);
void superBloq(char id[sizeChar]);
void reBmInodes(char id[sizeChar]);
void reBmBloques(char id[sizeChar]);

void arbol(char id[sizeChar]);
void dibujarBloquesCarpetas(FILE* fe, int pos, superBloque sb, particionMontada partMontada, int apuntador);
void dibujarBloquesArchivos(FILE* fe, int pos, superBloque sb, particionMontada partMontada, int apuntador);
#endif /* FREP_H */

