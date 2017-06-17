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

void mkfile(char id[sizeChar], char ruta[sizeChar], char p, int size, char count[sizeChar]) {
    printf("\tId=%s \n", id);
    printf("\tRuta= %s\n", ruta);
    printf("\tP= %c\n", p);
    printf("\tTamaño= %i\n", size);
    printf("\tCount= %s\n", count);
     verParticionMontada(id);
     
}

void crearRoot(char id[sizeChar]) {
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;


    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, bm_Inodos);

    //bit map de bloques 
    bmBloque bm_Bloques[3 * n];
    bmb_leer(sb.s_bm_block_start, n, mountNodo.mnt_ruta, bm_Bloques);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, mountNodo.mnt_ruta, Inodos);
    
    //Modificando los inodos
    Inodos[0].i_size=0;//el tamaño del archivo
    fechaActual(Inodos[0].i_atime);//ultima fecha que se leyó el nodo sin modificarlo
    fechaActual(Inodos[0].i_ctime);
    fechaActual(Inodos[0].i_mtime);
    Inodos[0].i_type=0;
    Inodos[0].i_block[0]= sb.s_first_blo;//primer bloque libre
    inodos_escribir(sb.s_inode_start, n, mountNodo.mnt_ruta, Inodos);//escribiendo lo inodos
    
    //hay que modificar el super bloque
    sb.s_free_blocks_counts--;
    sb.s_free_inodes_count--;
    sb.s_first_ino=sb.s_first_ino+sizeof(inodo);
    sb.s_first_blo=sb.s_first_blo+sizeof(bloqueCarpeta);
    sb.s_bjpurfree=sb.s_bjpurfree+sizeof(journalie);
    
   //sb_escribir(mountNodo.mnt_ruta,)
    
};

void insertarCarpeta(char id[sizeChar], char ruta[sizeChar]){
   superBloque sb = sb_retornar(id);
    mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;


    //bit map de indos.
    bmInodo arregloDeInodos[n];
    bmi_leer(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, arregloDeInodos);

    //bit map de bloques 
    bmBloque matrBloque[3 * n];
    bmb_leer(sb.s_bm_block_start, n, mountNodo.mnt_ruta, matrBloque);

    //indos
    inodo matrInodo[n];
    inodos_leer(sb.s_inode_start, n, mountNodo.mnt_ruta, matrInodo);
}


//funciones 
void fechaActual(char fecha[sizeChar]){
    time_t tiem = time(0);
    struct t*tlocal = localtime(&tiem);
    strftime(fecha, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);
}

void getInfoDiskMount(char id[sizeChar]){
     ////////los datos necesarios
    mnt_nodo mountNodo = retornarNodoMount(id);
    
        int part_inicio = 0;
        int part_tamano = 0;
        char part_colocacion = ' ';
        int pimerEspacioEBR = 0;
        

        if (mountNodo.mnt_particion.part_fit == 'b' || mountNodo.mnt_particion.part_fit == 'f' || mountNodo.mnt_particion.part_fit == 'w') {//es primaria
            part_inicio = mountNodo.mnt_particion.part_start;
            part_tamano = mountNodo.mnt_particion.part_size;
            part_colocacion = mountNodo.mnt_particion.part_type;
            pimerEspacioEBR = 0;
        } else {//del ebr
            part_inicio = mountNodo.mnt_ebr.part_start;
            part_tamano = mountNodo.mnt_ebr.part_size;
            pimerEspacioEBR = sizeof (bloqueEBR);
        }

}