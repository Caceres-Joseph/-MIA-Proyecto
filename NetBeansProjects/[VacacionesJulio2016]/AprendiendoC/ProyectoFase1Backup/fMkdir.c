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
    //verParticionMontada(id);
    if (p == 'p') {//crear las carpetas padres
        strInicializar();
    } else {//nos dan directamente la direcion
        crearCarpeta(id, ruta);
        
        
/*
        puts("\tInsertando a la lista");
        strInsertarAlFinal(listaDeStrings, id);
        strImprimir();
*/
    }
}

void crearCarpeta(char id[sizeChar], char ruta[sizeChar]) {

    //primor hay que buscar el directorio haciendo split
    char buffer[sizeChar] = "";
    //char concat=
    int i;
    int indiceInodo = 0;
    for (i = 0; ruta[i] != '\0'; i++) {
        if (ruta[i] == '/') {
           //printf("\n\tDirectorio= %s", buffer);
            int resultado = strncmp(buffer, "", sizeChar);
            if (resultado == 0) {//indica que me mandaron la raíz.
                //printf("raíz\n");
                indiceInodo = 0;
            } else {
                printf("\t\t----Buscando %s en Inodo-%i-----\n", buffer, indiceInodo);
                int indTemp= indiceInodoCarpetaEnApuntadorDirecto(id, indiceInodo, buffer);
                if(indTemp==-1){//hay que hacer la carpeta
                    printf("\t\tHaciendo la carpeta en el inodo %i\n", indiceInodo);
                    
                    //return; 
                }else{
                    indiceInodo = indTemp;//me pasa el siguiente nodo 
                }
            }
            memset(buffer, 0, sizeChar);

        } else {
            int o;

            for (o = 0; buffer[o] != '\0'; o++) {
            }

            buffer[o] = ruta[i];
        }
    }
}

int indiceInodoCarpetaEnApuntadorDirecto(char id[sizeChar], int indiceInodo, char nombreCarpeta[sizeChar]) {
    //int retorno = -1; //quiere decir que no lo ha encontrado
    superBloque sb = sb_retornar(id);
    //mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;
    particionMontada partMontada = devolverParticionMontada(id);

    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);

    int ind;
    int indiIno = 0;
    for (ind = 0; ind < 12; ind++) {//recorriendo los bloques directos
        int nRetorno = Inodos[indiceInodo].i_block[ind];
        if (nRetorno == -1) {
            //el bloque no apunta a nada
        } else if (ind == 0) {//este es el primer apuntador, hay que omitir los dos primeros.
            //tengo que obtener el bloque de este apuntador.
            indiIno = indiceInodoEnElQueEstaLaCarpeta(sb, nRetorno,  partMontada, nombreCarpeta,2);
            if (indiIno != -1) {
                return indiIno;
            }
        } else {//hay que realizar la busqueda en todas las posiciones 
            indiIno = indiceInodoEnElQueEstaLaCarpeta(sb, nRetorno,  partMontada, nombreCarpeta,0);
            if (indiIno != -1) {
                return indiIno;
            }
        }
    }
    return -1;
}

int indiceInodoEnElQueEstaLaCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque) {//para el primer aputnador directo

    int inicio = sb.s_block_start;

    bloqueCarpeta carpeta= blqcarp_leer(inicio, n, partMontada.ruta);

    int retorno = -1;
    int resultado = -1;
    int i;
    for (i = inicioEnBloque; i < 4; i++) {
        char aux[sizeChar] = "";
        strcat(aux, carpeta.b_content[i].b_name);
        resultado = strncmp(nombre, aux, sizeChar);
        if (resultado == 0) {//lo encontró pero hay que validar que el inodo a devolver sea de tipo carpeta.}
            int posInodo = carpeta.b_content[i].b_inodo;
            int valCarpeta = validarQueInodoSeaCarpeta(sb, posInodo, partMontada);
            if (valCarpeta == 0) {//es por que es de tipo carpeta
                return posInodo; //bloque que apunta al inodo de esa carpeta
            }

        }
    }
    return retorno;
}

int validarQueInodoSeaCarpeta(superBloque sb, int k, particionMontada partMontada) {
    int n = sb.s_inodes_count;
    
    inodo Inodos[n];
    
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);
    
    if (Inodos[k].i_type=='0') {//es carpeta
        return 0;
    }else{
        return 1;
    }

    
}

int indiceInodoEnElQueEstaLaCarpeta2(int inicio, int n, char ruta[sizeChar], char nombre[sizeChar]) {//cuando no es el apuntador cero
    bloqueCarpeta carpeta;

    //blqcarp_leer(inicio, n, ruta, carpeta);

    int retorno = -1;
    int resultado = -1;
    int i;
    for (i = 0; i < 4; i++) {
        char aux[sizeChar] = "";
        strcat(aux, carpeta.b_content[i].b_name);
        resultado = strncmp(nombre, aux, sizeChar);
        if (resultado == 0) {//lo encontró
            return carpeta.b_content[i].b_inodo; //bloque que apunta al inodo de esa carpeta
        }
    }
    return retorno;
}

bloqueCarpeta retornarBloqueCarpeta(int inicio, int n, char ruta[sizeChar], bloqueCarpeta *carpeta) {
    bloqueCarpeta retorno;
    //blqcarp_leer(inicio, n, ruta, retorno);
    return retorno;

}

inodo retornoInodoCarpeta(char id[sizeChar], int indiceInodo) {
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    //mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;
    particionMontada partMontada = devolverParticionMontada(id);

    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);


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
 * Inodos                                                   *** 
 **************************************************************/

/**************************************************************
 * Lista de String                                          *** 
 **************************************************************/

void strInicializar() {
    listaDeStrings = (str_lista*) malloc(sizeof (str_lista)); //inicializando las listas
    listaDeStrings->cabeza = NULL;
}

str_nodo* strCrearNodo(char palabra[sizeChar]) {
    str_nodo* nodo = (str_nodo*) malloc(sizeof (str_nodo));
    strcpy(nodo->dato, palabra);
    nodo->siguiente = NULL;
    return nodo;
}

void strInsertarAlFinal(str_lista* lista, char palabra[sizeChar]) {
    str_nodo* nodo = strCrearNodo(palabra);
    if (lista->cabeza == NULL) {
        //Lista vacia
        lista->cabeza = nodo;
    } else {
        //Lista llena
        //  str_nodo* puntero = lista->cabeza;
        nodo->siguiente = lista->cabeza;
        lista->cabeza = nodo;
        /*
                while (puntero->siguiente) {
                    puntero = puntero->siguiente;
                }
                puntero->siguiente = nodo;
         */
    }
}

void strImprimir() {
    str_lista* lista = listaDeStrings;
    str_nodo* puntero = lista->cabeza;
    puts("\t\t.......................Lo que contiene el string........................");
    while (puntero) {
        printf("\t\t%s\n", puntero->dato);
        puntero = puntero->siguiente;
    }
    puts("\t\t...................................................................");

}