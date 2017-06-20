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
        crearCarpetas(id, ruta);
        /*
                puts("\tInsertando a la lista");
                strInsertarAlFinal(listaDeStrings, id);
                strImprimir();
         */
    }
}

void crearCarpetas(char id[sizeChar], char ruta[sizeChar]) {


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
                int indTemp = indiceInodoCarpetaEnApuntadorDirecto(id, indiceInodo, buffer);
                if (indTemp == -1) {//hay que hacer la carpeta
                    printf("\t\tHaciendo la carpeta en el inodo %i\n", indiceInodo);
                    crearCarpetaEnInodo(id, indiceInodo, buffer);
                    //return;
                } else {
                    indiceInodo = indTemp; //me pasa el siguiente nodo 
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
/**************************************************************
 * Haciendo la carpeta                                      *** 
 **************************************************************/

//id, particion montada, indiceInodoAInsertar, nombreDeLaCarpeta

void crearCarpetaEnInodo(char id[sizeChar], int indiceInodoAInsertar, char nombreCarpeta[sizeChar]) {
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    int n = sb.s_inodes_count;

    //la particion que tiene los datos
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

    //journalie
    journalie jour[n * 3];
    jr_leer(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (journalie), n, partMontada.ruta, jour);

    //////////////////////////////////////////|>[aquí finalizo la lectura.]<|/////////////////////////////////////////////

    //superBloque
    //Journaling
    //bitMapInodos
    //bitMapBloques
    //Inodos
    //Blouqes

    //printf("\tPrimer Journal Libre= %i \n", sb.s_bjpurfree);
    //printf("\tPrimer Bloque Libre = %i \n", sb.s_first_blo);
    //printf("\tPrimer Inodo Libre = %i \n", sb.s_first_ino);


    //////////////////////////////////////////[Modificando el inodo y el Bloque anterior]

    int ind;
    int indiIno = 0;
    int punteroLibre = 0;
    for (ind = 0; ind < 12; ind++) {//recorriendo los bloques directos
        int nRetorno = Inodos[indiceInodoAInsertar].i_block[ind];
        if (nRetorno == -1) {
            punteroLibre = ind; //este es el puntero que esta libre
            break;
            //el bloque no apunta a nada
        } else if (ind == 0) {//este es el primer apuntador, hay que omitir los dos primeros.
            //tengo que obtener el bloque de este apuntador.
            indiIno = modificarBloqueCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 2); //escribir el inodo en el primer espacio libre y modificar el bloque de la carpeta
            if (indiIno == 0) {//si encontró un blouqe libre
                punteroLibre = 0;
                break;
            }
        } else {//hay que realizar la busqueda en otros bloques, y si no existe crearlo  
            indiIno = modificarBloqueCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 0);
            if (indiIno == 0) {
                punteroLibre = 0;
                break;
            }
        }
    }

    if (punteroLibre != 0) {//Hay que crear un nuevo bloque en el puntero e inicializarlo con -1's
        //aquí se ocupa dos bloques
        content contenido;
        contenido.b_inodo = sb.s_first_ino;
        strcpy(contenido.b_name, nombreCarpeta);

        bloqueCarpeta bloqCarp;
        bloqCarp.b_content[0] = contenido;
        bloqCarp.b_content[1].b_inodo = -1;
        bloqCarp.b_content[2].b_inodo = -1;
        bloqCarp.b_content[3].b_inodo = -1;
        blqcarp_escribir(sb.s_block_start, sb.s_first_blo, partMontada.ruta, bloqCarp); //ya se modificó el blouqe
        //hay que modificar también el  superbloque
        Inodos[indiceInodoAInsertar].i_block[punteroLibre] = sb.s_first_blo; //aquí el puntero

        bm_Bloques[sb.s_first_blo].status = '1';
        sb.s_first_blo = bmb_primerLibre(n, bm_Bloques);
        sb.s_free_blocks_counts--;
    } else {//si cabe en el bloque
        //aquí ocupa un bloque // y ya apunta al primer inodo.

    }
    //////////////////////////////////////////[Modificando el nuevo inodo]//

    printf("El primer inodo libre es = %i\n", sb.s_first_ino);
    Inodos[sb.s_first_ino].i_size = 0; //el tamaño del archivo
    strcpy(Inodos[ sb.s_first_ino].i_atime, partMontada.part_time); //ultima fecha que se leyó el nodo sin modificarlo
    fechaActual(Inodos[ sb.s_first_ino].i_ctime);
    fechaActual(Inodos[ sb.s_first_ino].i_mtime);
    Inodos[ sb.s_first_ino].i_type = 0; //0 si es carpeta
    Inodos[ sb.s_first_ino].i_block[0] = sb.s_first_blo; //a que bloque apunta? al primer bloque libre
    int lol;
    for (lol = 1; lol < 12; lol++) {
        Inodos[sb.s_first_ino].i_block[lol] = -1;
    }
    inodos_escribir(sb.s_inode_start, n, partMontada.ruta, Inodos); //escribiendo los inodos

    //////////////////////////////////////////[BitMapDeInodos]//
    bm_Inodos[sb.s_first_ino].status = '1';
    bmi_escribir(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //////////////////////////////////////////[El nuevo bloque]//
    content rootPadre;
    rootPadre.b_inodo = indiceInodoAInsertar;
    strcpy(rootPadre.b_name, " ");

    content root;
    root.b_inodo = sb.s_first_ino;
    strcpy(root.b_name, nombreCarpeta);

    bloqueCarpeta carpeta;
    carpeta.b_content[0] = rootPadre;
    carpeta.b_content[1] = root;

    carpeta.b_content[2].b_inodo = -1; //no apuntan a nada
    carpeta.b_content[3].b_inodo = -1; //no apuntan a nada


    blqcarp_escribir(sb.s_block_start, sb.s_first_blo, partMontada.ruta, carpeta);
    //////////////////////////////////////////[BitMapDeBloques]//
    bm_Bloques[sb.s_first_blo].status = '1';
    bmb_escribir(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //////////////////////////////////////////[Journalin]//
    journalie journ;
    fechaActual(journ.journal_fecha);
    strcpy(journ.journal_nombre, nombreCarpeta);
    journ.journal_tipo = '1';
    jour[sb.s_bjpurfree] = journ;
    jr_escribir(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (journalie), n, partMontada.ruta, jour);

    //////////////////////////////////////////[SuperBloque]//
    sb.s_first_ino = bmi_primerLibre(n, bm_Inodos);
    sb.s_bjpurfree++;
    sb.s_first_blo = bmb_primerLibre(n, bm_Bloques);
    sb.s_free_blocks_counts--;
    sb.s_free_inodes_count--;
    sb_escribir(partMontada.ruta, partMontada.part_inicio + partMontada.part_espacioEbr, sb);

}

int modificarBloqueCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque) {
    int inicio = sb.s_block_start;

    bloqueCarpeta carpeta = blqcarp_leer(inicio, n, partMontada.ruta);

    int i;
    for (i = inicioEnBloque; i < 4; i++) {
        if (carpeta.b_content[i].b_inodo == -1) {//si es -1, quiere decir que esta libre men
            printf("\tInsertando en la posición %i del bloque\n", i + 1);
            carpeta.b_content[i].b_inodo = sb.s_first_ino; //ya le mande el primer inodo libre
            strcpy(carpeta.b_content[i].b_name, nombre);
            blqcarp_escribir(inicio, n, partMontada.ruta, carpeta); //ya se modificó el blouqe
            return 0;
        }
    }
    return -1;
}

/**************************************************************
 * Busqueda de carpeta                                      *** 
 **************************************************************/
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
    int indiIno = -1;
    for (ind = 0; ind < 12; ind++) {//recorriendo los bloques directos
        int nRetorno = Inodos[indiceInodo].i_block[ind];
        if (nRetorno == -1) {
            //el bloque no apunta a nada
        } else if (ind == 0) {//este es el primer apuntador, hay que omitir los dos primeros.
            //tengo que obtener el bloque de este apuntador.
            indiIno = indiceInodoEnElQueEstaLaCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 2);
            if (indiIno != -1) {
                return indiIno;
            }
        } else {//hay que realizar la busqueda en todas las posiciones 
            indiIno = indiceInodoEnElQueEstaLaCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 0);
            if (indiIno != -1) {
                return indiIno;
            }
        }
    }
    return -1;
}

int indiceInodoEnElQueEstaLaCarpeta(superBloque sb, int n, particionMontada partMontada, char nombre[sizeChar], int inicioEnBloque) {//para el primer aputnador directo

    int inicio = sb.s_block_start;
    //printf("Blouqe->Lo estoy leyendo en %i\n", sb.s_block_start);
    bloqueCarpeta carpeta = blqcarp_leer(inicio, n, partMontada.ruta);

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
            //if (valCarpeta == 0) {//es por que es de tipo carpeta
                return posInodo; //bloque que apunta al inodo de esa carpeta
            //}
        }
    }
    return retorno;
}

int validarQueInodoSeaCarpeta(superBloque sb, int k, particionMontada partMontada) {
    int n = sb.s_inodes_count;

    inodo Inodos[n];

    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);

    if (Inodos[k].i_type == '0') {//es carpeta
        return 0;
    } else {
        return 1;
    }
}

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