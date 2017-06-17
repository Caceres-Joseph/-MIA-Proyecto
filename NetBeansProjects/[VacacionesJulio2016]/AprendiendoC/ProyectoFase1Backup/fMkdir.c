/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#include "estructuras.h"
#include "fMount.h"
#include "fMkfs.h"
#include "fMkfile.h"
#include "fMkdir.h"

void mkdir(char id[sizeChar], char ruta[sizeChar], char p) {
    printf("\tId=%s \n", id);
    printf("\tRuta= %s\n", ruta);
    printf("\tP= %c\n", p);
    //verParticionMontada(id);
    if (p == 'p') {//crear las carpetas padres

    } else {//nos dan directamente la direcion
        //crearCarpeta(id, ruta);
        puts("\tInsertando a la lista");
        strInicializar();
        strInsertarAlFinal(listaDeStrings,"Jhosef");
    }
}

void crearCarpeta(char id[sizeChar], char ruta[sizeChar]) {

    //primor hay que buscar el directorio haciendo split
    char buffer[sizeChar] = "";
    //char concat=
    int i;
    int indiceInodo=-1;
    for (i = 0; ruta[i] != '\0'; i++) {
        if (ruta[i] == '/') {
            printf("\n\tDirectorio= %s\n", buffer);
            int resultado = strncmp(buffer, "", sizeChar);
            if(resultado==0){
                indiceInodo=0; 
            }else{
                if(indiceInodo!=-1){
                                       
                    
                }else{
                    printf("\t[Error]No se envio la raíz\n");
                }
            }            
            memset(buffer, 0, sizeChar);
            
        } else {
            int o;

            for (o = 0; buffer[o] != '\0'; o++) {
            }

            buffer[o] = ruta[i];

            /*
                        printf("%c", ruta[i]);
             */
            //char de[1]= {};
            //strcat(buffer, de);
        }
    }
}

inodo retornarInodoCarpeta(char id[sizeChar], int indiceInodo) {
    
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    //mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;
    particionMontada partMontada = devolverParticionMontada(id);

    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //bit map de bloques 
    bmBloque bm_Bloques[3 * n];
    bmb_leer(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);
    
}



/**************************************************************
 * Lista de String                                          *** 
 **************************************************************/

void strInicializar() {
    listaDeStrings = (str_lista*) malloc(sizeof (str_lista)); //inicializando las listas
    listaDeStrings->cabeza = NULL;
}
str_nodo* strCrearNodo( char palabra[sizeChar]) {
    str_nodo* nodo = (str_nodo*) malloc(sizeof (str_nodo));
    strcpy(nodo->dato,palabra);
    nodo->siguiente= NULL;
    return nodo;
}

void strInsertarAlFinal(str_lista* lista,char palabra[sizeChar]) {
    str_nodo* nodo = strCrearNodo(palabra);
    if (lista->cabeza == NULL) {
        //Lista vacia
        lista->cabeza = nodo;
    } else {
        //Lista llena
        str_nodo* puntero = lista->cabeza;
        while (puntero->siguiente) {
            puntero = puntero->siguiente;
        }
        puntero->siguiente = nodo;
    }
}
