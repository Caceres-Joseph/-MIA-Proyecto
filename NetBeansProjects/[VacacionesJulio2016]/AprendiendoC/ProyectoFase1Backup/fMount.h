/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fMount.h
 * Author: joseph
 *
 * Created on 16 de junio de 2017, 11:20 AM
 */

#ifndef FMOUNT_H
#define FMOUNT_H
#include "estructuras.h"
//la cabeza
mnt_lista* listaDeParticiones;


void mountn(char ruta[sizeChar],char nombre[sizeChar]);

//lista
void inicializarListaMount();
void mntInsertarAlFinal(mnt_lista* lista, partition particion, bloqueEBR logica, char ruta[sizeChar]);
void mntPush(partition particion, bloqueEBR logica, char ruta[sizeChar]);
void imprimirListaDeParticionesMontadas() ;
mnt_nodo retornarNodoMount(char ids[sizeChar]);
char numeroDeDisco(mnt_lista*lista, char letra);
char letraDeDisco(mnt_lista*lista, char ruta[sizeChar]);
mnt_nodo* mntCrearNodo(partition particion, bloqueEBR logica, char ruta[sizeChar]);

#endif /* FMOUNT_H */

