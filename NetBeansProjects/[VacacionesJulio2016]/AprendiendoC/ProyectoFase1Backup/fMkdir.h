/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fMkdir.h
 * Author: joseph
 *
 * Created on 17 de junio de 2017, 08:47 AM
 */

#ifndef FMKDIR_H
#define FMKDIR_H
#include "estructuras.h"

void mkdir(char id[sizeChar], char ruta[sizeChar], char p);

void crearCarpeta(char id[sizeChar], char ruta[sizeChar]);//carpetas sin P
//lista
str_lista* listaDeStrings;
void strInicializar();
str_nodo* strCrearNodo( char palabra[sizeChar]) ;
void strInsertarAlFinal(str_lista* lista,char palabra[sizeChar]);

#endif /* FMKDIR_H */
