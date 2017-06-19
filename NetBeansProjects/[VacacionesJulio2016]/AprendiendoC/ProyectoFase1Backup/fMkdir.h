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


//lista
str_lista* listaDeStrings;
void strInicializar();
str_nodo* strCrearNodo( char palabra[sizeChar]) ;
void strInsertarAlFinal(str_lista* lista,char palabra[sizeChar]);
void strImprimir();

//para las carpetas
void crearCarpeta(char id[sizeChar], char ruta[sizeChar]) ;
int indiceInodoCarpetaEnApuntadorDirecto(char id[sizeChar], int indiceInodo, char nombreCarpeta[sizeChar]) ;
//int  indiceInodoEnElQueEstaLaCarpeta(int inicio, int n, char ruta[sizeChar], char nombre[sizeChar]) ;
int indiceInodoEnElQueEstaLaCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque);
int indiceInodoEnElQueEstaLaCarpeta2(int inicio, int n, char ruta[sizeChar], char nombre[sizeChar]);
int validarQueInodoSeaCarpeta(superBloque sb, int k, particionMontada partMontada);
#endif /* FMKDIR_H */
