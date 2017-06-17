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
#include "Mbr.h"
#include "fMkfile.h"

#include "fFdisk.h"
//#include "disco.h"

void mountn(char ruta[sizeChar], char nombre[sizeChar]) {
    partition part = devolverParticion(ruta, nombre); //si se econtro en primaria o secundaria
    bloqueEBR eb;
    printf("\tMontando: %s \n", nombre);
    if (part.part_fit == 'b' || part.part_fit == 'f' || part.part_fit == 'w') {//si no hay primaria, buscar en la secundaria
        mntPush(part, eb, ruta); //se ingresa la particion a la lista.
    } else {
        eb = devolverLogica(ruta, nombre);
        if (eb.part_fit == 'b' || eb.part_fit == 'f' || eb.part_fit == 'w')
            mntPush(part, eb, ruta);
        else
            puts("\t[ERROR]No se encontró la particion");
    }

}

/**************************************************************
 * Lista MOUNT                                               *** 
 **************************************************************/
void inicializarListaMount() {
    listaDeParticiones = (mnt_lista*) malloc(sizeof (mnt_lista)); //inicializando las listas
    listaDeParticiones->cabeza = NULL;
}

void mntInsertarAlFinal(mnt_lista* lista, partition particion, bloqueEBR logica, char ruta[sizeChar]) {
    mnt_nodo* nodo = mntCrearNodo(particion, logica, ruta);
    if (lista->cabeza == NULL) {
        //Lista vacia
        lista->cabeza = nodo;
    } else {
        //Lista llena
        mnt_nodo* puntero = lista->cabeza;
        while (puntero->siguiente) {
            puntero = puntero->siguiente;
        }
        puntero->siguiente = nodo;
    }
}

void mntPush(partition particion, bloqueEBR logica, char ruta[sizeChar]) {
    mntInsertarAlFinal(listaDeParticiones, particion, logica, ruta);
}

void imprimirListaDeParticionesMontadas() {
    mnt_lista* lista = listaDeParticiones;
    mnt_nodo* puntero = lista->cabeza;
    puts("\t\t.......................Particiones Montadas........................");
    while (puntero) {
        if (puntero->mnt_particion.part_fit == 'b' || puntero->mnt_particion.part_fit == 'f' || puntero->mnt_particion.part_fit == 'w') {//si no hay primaria, buscar en la secundaria
            printf("\t\tNombreParticion-> %s\tTipo->P\tId-> %s\tRuta->%s\n", puntero->mnt_particion.part_name, puntero->mnt_id, puntero->mnt_ruta);
        } else {
            printf("\t\tNombreParticion-> %s\tTipo->L\tId-> %s\tRuta->%s\n", puntero->mnt_ebr.part_name, puntero->mnt_id, puntero->mnt_ruta);
        }
        puntero = puntero->siguiente;
    }
    puts("\t\t...................................................................");

}

mnt_nodo retornarNodoMount(char ids[sizeChar]) {
    mnt_lista* lista = listaDeParticiones;
    mnt_nodo* puntero = lista->cabeza;
    int retorno;

    while (puntero) {
        retorno = strncmp(ids, puntero->mnt_id, sizeChar);
        if (retorno == 0)//lo encontró
            return *puntero;
        puntero = puntero->siguiente;
    }
    puts("\t[ERROR]No se encontró ese id");
    mnt_nodo re;
    strcpy(re.mnt_ruta, "");
    return re;
}
void verParticionMontada(char id[sizeChar]){
    
    particionMontada m= devolverParticionMontada(id);
    
    printf("\t\t--------------------ParticionMontada:Id=%s------------------------------\n",m.id);
    printf("\t\tRuta:%s\n",m.ruta);
    printf("\t\tNombre:%s\n",m.part_name);
    printf("\t\tTiempoDeMontaje:%s\n",m.part_time);
    printf("\t\tTipo = %c \tInicio= %i\tTamño= %i\n",m.part_type,m.part_inicio,m.part_tamano);
    printf("\t\tColcn= %c \tEspEbr= %i\tStatus=%i\n",m.part_colocacion,m.part_espacioEbr,m.part_status);
    
}

particionMontada devolverParticionMontada(char id[sizeChar]) {
    mnt_nodo mountNodo = retornarNodoMount(id);
    char part_type; //e o p
    int part_inicio; //inicio 
    int part_tamano; //tamaño d ela particion
    char part_colocacion; //w,b,
    int part_espacioEbr; //si es extendida
    int part_status = 0; //activa o no 
    char part_name[sizeChar]; //nombre de la partición.

    
    char ruta[sizeChar]; //ruta del disco
    
    
    times part_time; //tiempo en que se monto el disco
    fechaActual(part_time);


    if (mountNodo.mnt_particion.part_fit == 'b' || mountNodo.mnt_particion.part_fit == 'f' || mountNodo.mnt_particion.part_fit == 'w') {//es primaria

        part_type = mountNodo.mnt_particion.part_type; //e o p
        part_inicio = mountNodo.mnt_particion.part_start; //inicio 
        part_tamano = mountNodo.mnt_particion.part_size; //tamaño d ela particion
        part_colocacion = mountNodo.mnt_particion.part_fit; //w,b,
        part_espacioEbr = 0; //si es extendida
        part_status = 1; //activa o no 
        strcpy(part_name, mountNodo.mnt_particion.part_name); //nombre de la partición.

    } else {//del ebr

        part_type = 'l'; //e o p
        part_inicio = mountNodo.mnt_ebr.part_start; //inicio 
        part_tamano = mountNodo.mnt_ebr.part_size; //tamaño d ela particion
        part_colocacion = mountNodo.mnt_ebr.part_fit; //w,b, hay que buscar la particion extendida
        part_espacioEbr = sizeof (bloqueEBR); //si es extendida
        part_status = 1; //activa o no 
        strcpy(part_name, mountNodo.mnt_ebr.part_name); //nombre de la partición.
    }
    particionMontada retorno;

    retorno.part_type = part_type; //e o p
    retorno.part_inicio = part_inicio; //inicio 
    retorno.part_tamano = part_tamano; //tamaño d ela particion
    retorno.part_colocacion = part_colocacion; //w,b,
    retorno.part_espacioEbr = part_espacioEbr; //si es extendida
    retorno.part_status = part_status; //activa o no 

    strcpy(retorno.part_name, part_name); //nombre de la partición.
    strcpy(retorno.ruta, mountNodo.mnt_ruta); //ruta del disco
    strcpy(retorno.id, id); //id del disco
    strcpy(retorno.part_time, part_time); //tiempo en que se monto el disco

    return retorno;
}

char numeroDeDisco(mnt_lista*lista, char letra) {
    mnt_nodo*puntero = lista->cabeza;
    char letraTemporal = '1';
    int retorno = 1;
    if (lista->cabeza == NULL) {
        return '1';
    }
    while (puntero) {
        if (letra == puntero->mnt_id[2])
            retorno = 0;
        //retorno= strcmp(&letra,&puntero->mnt_id[2]);//retorna 0 si son iguales
        if (retorno == 0) {

            if ((puntero->mnt_id[3]) > letraTemporal)
                letraTemporal = puntero->mnt_id[3]; //se va buscando el mas grande

            //si lo encontro retorna 0
        }
        puntero = puntero->siguiente;
    };

    if (retorno != 0) {
        return '1';
    } else {
        return letraTemporal + 1;
    }

}

char letraDeDisco(mnt_lista*lista, char ruta[sizeChar]) {
    mnt_nodo*puntero = lista->cabeza;
    char letraTemporal = 'a';
    int retorno = 0;
    if (lista->cabeza == NULL) {
        return 'a';
    }
    while (puntero) {
        retorno = strncmp(ruta, puntero->mnt_ruta, sizeChar);
        if (retorno == 0) {
            char reto = puntero->mnt_id[2];

            return reto;
            //si lo encontro retorna 0
        } else {
            /*
                        int resul = strncmp(letraTemporal, puntero->mnt_id[2],1);
             */

            int resul = strcmp(&letraTemporal, &puntero->mnt_id[2]);
            if ((puntero->mnt_id[2]) > letraTemporal)
                letraTemporal = puntero->mnt_id[2]; //se va buscando el mas grande
        }
        puntero = puntero->siguiente;
    };

    if (retorno == 0) {
        return 'a';

    } else {
        return letraTemporal + 1;
    }
}

mnt_nodo* mntCrearNodo(partition particion, bloqueEBR logica, char ruta[sizeChar]) {
    mnt_nodo* nodo = (mnt_nodo*) malloc(sizeof (mnt_nodo));
    nodo->mnt_particion = particion;
    nodo->mnt_ebr = logica;
    strcpy(nodo->mnt_ruta, ruta);

    char resultado = letraDeDisco(listaDeParticiones, ruta);
    strcpy(nodo->mnt_id, "vd00"); //aquí tengo que recorrere la lista para colocarle el id que se necesita
    nodo->mnt_id[2] = resultado;
    nodo->mnt_id[3] = numeroDeDisco(listaDeParticiones, resultado);

    time_t tiem = time(0);
    struct t*tlocal = localtime(&tiem);
    strftime(nodo->tiempo, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);


    nodo->siguiente = NULL;
    return nodo;
}

