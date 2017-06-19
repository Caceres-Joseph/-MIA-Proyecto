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
#include "fFdisk.h"
#include "fMount.h"
#include "fMkfs.h"
#include "fMkfile.h"

void mkfs(char id[sizeChar], char unit, char type[sizeChar], int add) {

    if (unit == ' ')//por si no viene unidad
        unit = 'k';
    int retorno = 1;
    retorno = strncmp(type, "", sizeChar);
    if (retorno == 0)
        strcpy(type, "full");

    /*
     * Fn parametros opcionales        
     */

    if (unit == 'k')
        add = add * kilobyte;
    else if (unit == 'm')
        add = add * megabyte;

    if (add != 0) {
        puts("\tAgregar o restar");
    } else {//formatear normal
        mnt_nodo particion = retornarNodoMount(id);
        int re = strncmp(particion.mnt_ruta, "", sizeChar);
        if (re == 0)
            return;

        ////////los datos necesarios
        int part_inicio = 0;
        int part_tamano = 0;
        char part_colocacion = ' ';
        int pimerEspacioEBR = 0;

        if (particion.mnt_particion.part_fit == 'b' || particion.mnt_particion.part_fit == 'f' || particion.mnt_particion.part_fit == 'w') {//es primaria
            part_inicio = particion.mnt_particion.part_start;
            part_tamano = particion.mnt_particion.part_size;
            part_colocacion = particion.mnt_particion.part_type;
            pimerEspacioEBR = 0;
        } else {//del ebr
            part_inicio = particion.mnt_ebr.part_start;
            part_tamano = particion.mnt_ebr.part_size;
            part_colocacion = metodoDeColocacionExtendida;
            pimerEspacioEBR = sizeof (bloqueEBR);
        }

        double partta = (double) part_tamano;
        double nu;
        nu = (partta - sizeof (superBloque)) / (4.0 + 3.0 * 64.0 + sizeof (inodo) + sizeof (journalie));
        printf("\tSuperBloque=%i| Inodo = %i|Journalie=%i\n", sizeof (superBloque), sizeof (inodo), sizeof (journalie));
        printf("\tTamaño de la partición=%i\n", part_tamano);
        printf("\tN en double=%f\n", nu);
        int n = (int) nu;
        printf("\tN en entero = %i\n", n);
        int disk = sizeof (superBloque) + n + n * sizeof (journalie) + 3 * n + n * sizeof (inodo) + 3 * n * sizeof (bloqueCarpeta);
        printf("\tTamaño de formato es= %i\n", disk);

        crear_ext3(particion, n, part_inicio + pimerEspacioEBR); //creando los sectores, super bloque, inodos
        
        //luego tengo que crear la raíz por que si no voy a pisar
        printf("\tCreando la carpeta raíz\n");
        crearRoot(id);
        //crearHome(id); 

    }
}

/**************************************************************
 * Funciones                                                 *** 
 **************************************************************/


void crear_ext3(mnt_nodo mountNodo, int n, int inicioParticion) {
    //n=n-1;
    superBloque sb = sb_inicializar(n, mountNodo.tiempo, inicioParticion);
    sb_escribir(mountNodo.mnt_ruta, inicioParticion, sb); //Escribiendo el super bloque


    
    journalie jr[n]; //arreglo de journalie
    jr_escribir(inicioParticion + sizeof (superBloque), n, mountNodo.mnt_ruta, jr);

    
    //Bitmap de bloques
    bmBloque agrregloBmb[n * 3];
    bmBloque bmb;
    bmb.status = '0';
    int u;
    for (u = 0; u < 3 * n; u++) {
        agrregloBmb[u] = bmb;
    }
    bmb_escribir(sb.s_bm_block_start, n, mountNodo.mnt_ruta, agrregloBmb);




    //Bit map de inodos
    bmInodo bmi[n];
    bmInodo bm;
    bm.status = '0';
    int l;
    for (l = 0; l < n; l++) {
        bmi[l] = bm;
    }
    bmi_escribir(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, bmi);
    
    
    // inodos
    inodo agrregloInodo[n];
    inodo ino;
    int p;
    for (p = 0; p < 15; p++) {//llenando los puntero con -1
        ino.i_block[p]=-1;
    }
    
    int t;
    for (t = 0; t < n; t++) {
        ino.i_uid = t;
        agrregloInodo[t] = ino;
    }
    printf("");
    inodos_escribir(sb.s_inode_start, n, mountNodo.mnt_ruta, agrregloInodo);
}

/**************************************************************
 * SuperBloque                                              *** 
 **************************************************************/

superBloque sb_inicializar(int n, times tiempo, int inicio) {//inicializo las variables del superbloque

    printf("\tinicio en = %i\n", inicio);
    superBloque sb;

    sb.s_inodes_count = n;
    sb.s_blocks_count = 3 * n;

    sb.s_free_blocks_counts = n;
    sb.s_free_inodes_count = 3 * n;

    strcpy(sb.s_mtime, tiempo);
    strcpy(sb.s_unmtime, tiempo);

    sb.s_mnt_count;

    sb.s_magic = -1;

    sb.s_inode_size = sizeof (inodo);
    sb.s_block_size = sizeof (bloqueArchivo);

    sb.s_first_ino = inicio + sizeof (superBloque) + 3 * n * sizeof (journalie) + 3 * n + n; //esta es la primer posicion y ya se agrego el +1 //primer inodo libre
    sb.s_first_blo = sb.s_first_ino + n * sizeof (inodo); //primer bloque libre

    sb.s_bm_inode_start = inicio + sizeof (superBloque) + 3 * n * sizeof (journalie);
    sb.s_bm_block_start = sb.s_bm_inode_start + n;

    sb.s_inode_start = inicio + sizeof (superBloque) + 3 * n * sizeof (journalie) + 3 * n + n;
    sb.s_block_start = sb.s_inode_start + n * sizeof (inodo);
    sb.s_bjpurfree = inicio + sizeof (superBloque);
    return sb;
}
void sb_escribir(char ruta[sizeChar], int inicio, superBloque sb) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {

        fseek(f, inicio, SEEK_SET);
        fwrite(&sb, sizeof (superBloque), 1, f);
        fclose(f);
    }

}
superBloque sb_retornar(char id[sizeChar]) {
    superBloque sb;
    mnt_nodo particion = retornarNodoMount(id);
    int re = strncmp(particion.mnt_ruta, "", sizeChar);
    if (re == 0)
        return;

    ////////los datos necesarios
    int part_inicio = 0;
    int part_tamano = 0;
    char part_colocacion = ' ';

    int tamanoEBR = 0;
    if (particion.mnt_particion.part_fit == 'b' || particion.mnt_particion.part_fit == 'f' || particion.mnt_particion.part_fit == 'w') {//es primaria
        part_inicio = particion.mnt_particion.part_start;
        part_tamano = particion.mnt_particion.part_size;
        part_colocacion = particion.mnt_particion.part_type;
        tamanoEBR = 0;
    } else {//del ebr
        part_inicio = particion.mnt_ebr.part_start;
        part_tamano = particion.mnt_ebr.part_size;
        part_colocacion = metodoDeColocacionExtendida;
        tamanoEBR = sizeof (bloqueEBR);
    }
    //hay que abirir el archivo 
    FILE *f;
    if ((f = fopen(particion.mnt_ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {

        fseek(f, part_inicio + tamanoEBR, SEEK_SET);
        fread(&sb, sizeof (superBloque), 1, f);
        fclose(f);
    }

    return sb;

}


/**************************************************************
 * Journalie                                                *** 
 **************************************************************/

void jr_escribir(int inicio, int n, char ruta[sizeChar], journalie aux[]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < 3 * n + 1; j++) {
            fseek(f, inicio + j * (sizeof (journalie)), SEEK_SET);
            fwrite(&aux[j], sizeof (journalie), 1, f);
        }
        fclose(f);
    }
}

/**************************************************************
 * BitMap de inodos                                         *** 
 **************************************************************/

void bmi_escribir(int inicio, int n, char ruta[sizeChar], bmInodo aux[]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n; j++) {
            fseek(f, inicio + j * (sizeof (bmInodo)), SEEK_SET);
            //bmInodo aux2 = aux[j];
            fwrite(&aux[j], sizeof (bmInodo), 1, f);
        }
        fclose(f);
    }
}

void bmi_leer(int inicio, int n, char ruta[sizeChar], bmInodo *aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n; j++) {
            //bmInodo aux2 = aux[j];
            fseek(f, inicio + j * (sizeof (bmInodo)), SEEK_SET);
            fread(&aux[j], sizeof (bmInodo), 1, f);
        }
        fclose(f);
    }
}

/**************************************************************
 * BitMap de bloques                                        *** 
 **************************************************************/

void bmb_escribir(int inicio, int n, char ruta[sizeChar], bmBloque aux[]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n * 3; j++) {
            fseek(f, inicio + j * (sizeof (bmBloque)), SEEK_SET);
            fwrite(&aux[j], sizeof (bmBloque), 1, f);
        }
        fclose(f);
    }
}

void bmb_leer(int inicio, int n, char ruta[sizeChar], bmBloque *aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n * 3; j++) {
            fseek(f, inicio + j * (sizeof (bmBloque)), SEEK_SET);
            fread(&aux[j], sizeof (bmBloque), 1, f);
        }
        fclose(f);
    }
}

/**************************************************************
 * INODOS                                                   *** 
 **************************************************************/
void inodos_escribir(int inicio, int n, char ruta[sizeChar], inodo aux[]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n; j++) {
            fseek(f, inicio + j * (sizeof (inodo)), SEEK_SET);
            fwrite(&aux[j], sizeof (inodo), 1, f);
        }
        fclose(f);
    }
}

void inodos_leer(int inicio, int n, char ruta[sizeChar], inodo *aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n; j++) {
            fseek(f, inicio + j * (sizeof (inodo)), SEEK_SET);
            fread(&aux[j], sizeof (inodo), 1, f);
        }
        fclose(f);
    }
}

/**************************************************************
 * Carpetas                                                 *** 
 **************************************************************/

void blqcarp_escribir(int inicio, int n, char ruta[sizeChar], bloqueCarpeta carpeta) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        //la n que recibo es la posicion del bloque.
        fseek(f,inicio+n*(sizeof(bloqueCarpeta)),SEEK_SET);
        fwrite(&carpeta, sizeof (bloqueCarpeta), 1, f);//insertando la carpeta;
        fclose(f);
    }
}

bloqueCarpeta blqcarp_leer(int inicio, int n, char ruta[sizeChar]) {
    FILE *f;
    bloqueCarpeta carpeta;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        //la n que recibo es la posicion del bloque.
        fseek(f,inicio+n*(sizeof(bloqueCarpeta)),SEEK_SET);
        fread(&carpeta, sizeof (bloqueCarpeta), 1, f);//insertando la carpeta;
        fclose(f);
    }
    return carpeta;
}