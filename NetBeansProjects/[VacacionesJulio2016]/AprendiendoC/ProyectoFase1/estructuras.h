

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

typedef struct{
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_counts;
    int s_free_inodes_count;
    times s_mtime;
    times s_unmtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_size;
    int s_block_size;
    int s_first_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
    
}superBloque;

typedef struct{
    char journal_tipo;
    char journal_nombre[16];
    char journal_contenido;
    times journal_fecha;
};
typedef struct{
    char status;
}bmInodo;
typedef struct{
    char status;
}bmBloque;

typedef struct{
    int i_uid ;
    int i_gid;
    int i_size;
    times i_atime;
    times i_ctime;
    times i_mtime;
    int i_block[15];
    char i_type;
}inodo;
typedef struct{
    char b_name[12];
    int b_inodo;
}content;

typedef struct{
    content b_content[4];    
}bloqueCarpeta;


typedef struct{
    char b_content[64];
    
}bloqueArchivo;

typedef struct{
    int b_pointers[16];
}bloqueApuntador;


///------------fin EBR----------------------

///------------Mount  ----------------------


typedef struct{
    partition mnt_particion;//esto si es primaria o secundaria 
    bloqueEBR mnt_ebr;//esto si es extendida
    char mnt_ruta[sizeChar];
    char mnt_id[sizeChar];
    struct mnt_nodo* siguiente;
}mnt_nodo;

typedef struct mnt_lista{
    mnt_nodo* cabeza;
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


