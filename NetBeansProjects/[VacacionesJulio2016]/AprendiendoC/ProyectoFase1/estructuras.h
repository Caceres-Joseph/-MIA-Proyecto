

/* 
 * File:   estructuras.h
 * Author: joseph
 *
 * Created on 11 de junio de 2017, 03:25 AM
 */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

typedef char times[50];

typedef int bool;
#define true 1
#define false 0
#define sizeChar  50
//Lista

typedef struct token {
    char tipo[sizeChar];
    char valor[sizeChar];
} token;

typedef struct Nodo {
    token token;
    struct Nodo* siguiente;
} Nodo;





typedef struct Lista {
    Nodo* cabeza;
} Lista;
//Disco

typedef struct {
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
} partition;

///------------EBR----------------------
typedef struct {
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
} bloqueEBR;
///------------fin EBR----------------------

///------------Mount  ----------------------
typedef struct {
    char mnt_nombre[sizeChar];
    char mnt_ruta[sizeChar];
    char mnt_id[sizeChar];
}mount;

typedef struct{
    mount mnt;
    struct mnt_nodo* siguiente;
}mnt_nodo;

typedef struct{
    mnt_nodo* siguiente;
}mnt_lista;

///------------fin Mount----------------------


typedef struct {/*Contiene la estructura del MBR*/
    int mbr_tamano;
    times mbr_fecha_creacion; //char fecha [128];
    int mbr_disk_asignature;
    partition mbr_partition_1;
    partition mbr_partition_2;
    partition mbr_partition_3;
    partition mbr_partition_4;

} bloqueMBR;

#endif


