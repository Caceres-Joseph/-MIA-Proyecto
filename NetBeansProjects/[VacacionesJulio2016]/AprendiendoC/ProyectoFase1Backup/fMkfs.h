/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fMkfs.h
 * Author: joseph
 *
 * Created on 16 de junio de 2017, 12:45 PM
 */

#ifndef FMKFS_H
#define FMKFS_H

#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"

void crear_ext3(mnt_nodo mountNodo, int n, int inicioParticion);

void mkfs(char id[sizeChar], char unit, char type[sizeChar], int add);

//SuperBloque
superBloque sb_inicializar(int n, times tiempo, int inicio);
void sb_escribir(char ruta[sizeChar], int inicio, superBloque sb);
superBloque sb_retornar(char id[sizeChar]);

//journalie
void jr_escribir(int inicio, int n, char ruta[sizeChar], journalie aux[]);
//bitmap de inodos
void bmi_escribir(int inicio, int n, char ruta[sizeChar], bmInodo aux[]);
void bmi_leer(int inicio, int n, char ruta[sizeChar], bmInodo *aux);
//bit map de blouqes
void bmb_escribir(int inicio, int n, char ruta[sizeChar], bmBloque aux[]);
void bmb_leer(int inicio, int n, char ruta[sizeChar], bmBloque *aux);
//INODOS
void inodos_escribir(int inicio, int n, char ruta[sizeChar], inodo aux[]);
void inodos_leer(int inicio, int n, char ruta[sizeChar], inodo *aux);

//Bloques 
//carpetas
void blqcarp_escribir(int inicio, int n, char ruta[sizeChar], bloqueCarpeta carpeta) ;
//void blqcarp_leer(int inicio, int n, char ruta[sizeChar], bloqueCarpeta *carpeta);
bloqueCarpeta blqcarp_leer(int inicio, int n, char ruta[sizeChar]);
#endif /* FMKFS_H */

