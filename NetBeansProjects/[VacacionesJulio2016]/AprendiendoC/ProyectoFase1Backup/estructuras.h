

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

#define kilobyte  1000
#define megabyte  1000000
#define tamaDiscoDuro 1000000
//Lista

typedef struct token {
    char tipo[sizeChar];
    char valor[sizeChar];
} token;

typedef struct Nodo {
    token token;
    struct Nodo* siguiente;
} Nodo;

typedef struct {
    char part_type;//e o p
    int part_inicio;//inicio 
    int part_tamano;//tamaño d ela particion
    char part_colocacion;//w,b,
    int part_espacioEbr;//si es extendida
    int part_status;//activa o no 
    char part_name[sizeChar];//nombre de la partición.
    char ruta[sizeChar];//ruta del disco
    char id[sizeChar];//id del disco
    times part_time; //tiempo en que se monto el disco

}particionMontada;





typedef struct Lista {
    Nodo* cabeza;
} Lista;
//Disco

typedef struct {
    char part_status;//indica si la particion esta activa o no
    char part_type;//Indica si partición es primaria o extendida P o E
    char part_fit;//Tipo de ajuste de la partición. Tendrá los valores BF (Best), FF (First) o WF (worst)
    int part_start;//Indica en qué byte del disco inicia la partición
    int part_size;//Contiene el tamaño total de la partición en bytes.
    char part_name[16];//Name de la partición
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
    int s_inodes_count;//Guarda el número total de inodos
    int s_blocks_count;//Guarda el número total de bloques
    int s_free_blocks_counts;//Contiene el número de bloques libres
    int s_free_inodes_count;//Contiene el número de inodos libres
    times s_mtime;//Última fecha en el que el sistema fue montado
    times s_unmtime;//Última fecha en que el sistema fue desmontado
    int s_mnt_count;//Indica cuantas veces se ha montado el sistema
    int s_magic;//Valor que identifica al sistema de archivos, tendrá el valor 0xEF53
    int s_inode_size;//Tamaño del inodo
    int s_block_size;//Tamaño del bloque
    int s_first_ino;//Primer inodo libre
    int s_first_blo;//Primer bloque libre
    int s_bm_inode_start;//Guardará el inicio del bitmap de inodos
    int s_bm_block_start;//Guardará el inicio del bitmap de bloques
    int s_inode_start;//Guardará el inicio de la tabla de inodos
    int s_block_start;//Guardará el inico de la tabla de bloques
    int s_bjpurfree;//El padre 
}superBloque;

typedef struct{
    char journal_tipo;
    char journal_nombre[16];
    char journal_contenido;
    times journal_fecha;
    char Padre[12];
}journalie;

typedef struct{
    char status;
}bmInodo;
typedef struct{
    char status;
}bmBloque;

typedef struct{
    int i_uid ;//UID del propietario
    int i_gid;//GID al grupo al que pertenece
    
    int i_size;//tamaño del archivo en bytes
    times i_atime;//ultima fecha que se leyó el inodo sin modificarlo
    times i_ctime;//fecha en la que se creó el inodo
    times i_mtime;//ultima fecha en la que se modifico el inodo
    int i_block[15];//arreglo de punteros, los prieros 12 son directos, 13 indirecto, 14 indirecto doble, 15 indirecto triple
    char i_type;//1 = archivo, 0 = carpeta
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
    char dato[sizeChar];
    struct str_nodo* siguiente;
}str_nodo;
typedef struct{
    str_nodo* cabeza;
    
}str_lista;



typedef struct{
    partition mnt_particion;//esto si es primaria o secundaria 
    bloqueEBR mnt_ebr;//esto si es extendida
    times tiempo;//tiempo en que fue montada la particion
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


