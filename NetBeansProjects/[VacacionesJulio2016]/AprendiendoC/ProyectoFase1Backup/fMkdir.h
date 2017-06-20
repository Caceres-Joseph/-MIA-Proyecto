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
void crearCarpetas(char id[sizeChar], char ruta[sizeChar]) ;
//buscando la carpeta
int indiceInodoCarpetaEnApuntadorDirecto(char id[sizeChar], int indiceInodo, char nombreCarpeta[sizeChar]) ;
int indiceInodoEnElQueEstaLaCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque);
int validarQueInodoSeaCarpeta(superBloque sb, int k, particionMontada partMontada);

//creando la carpeta dentro del inodo
void crearCarpetaEnInodo(char id[sizeChar], int indiceInodoAInsertar, char nombreDeLaCarpeta[sizeChar]);
int modificarBloqueCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque);
#endif /* FMKDIR_H */
