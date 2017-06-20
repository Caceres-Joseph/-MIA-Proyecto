/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fMkfile.h
 * Author: joseph
 *
 * Created on 16 de junio de 2017, 02:01 PM
 */

#ifndef FMKFILE_H
#define FMKFILE_H
#include "estructuras.h"

void mkfile(char id[sizeChar],char ruta[sizeChar], char p, int size,char count[sizeChar]);

//funciones 
void fechaActual(char fecha[sizeChar]);


//carpetas
void crearRoot(char id[sizeChar]);
void crearHome(char id[sizeChar]) ;

void insertarCarpeta(char id[sizeChar], char ruta[sizeChar]);

void crearArchivo(char id[sizeChar], char ruta[sizeChar], int size, char count[sizeChar]);
void crearArchivoEnInodo(char id[sizeChar], int indiceInodoAInsertar, char nombreCarpeta[sizeChar], int size) ;
#endif /* FMKFILE_H */

