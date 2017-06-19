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


#include "Reporte.h"
#include "estructuras.h"
#include "Mbr.h"
#include "fMount.h"
#include "fMkfs.h"
//#include "disco.h"

/**************************************************************
 * REPORTES                                                 *** 
 **************************************************************/

void reporteMBR(char ruta[sizeChar]) {
    printf("\t................................................MBR.............................................\n");
    bloqueMBR mbr = leerMBR(ruta);
    printf("\tDisco:\n");
    printf("\t\tTamaño:%i\n", mbr.mbr_tamano);
    printf("\t\tFecha :%s\n", mbr.mbr_fecha_creacion);
    printf("\t\tId    :%i\n", mbr.mbr_disk_asignature);

    partition particiones[4];
    particiones[0] = mbr.mbr_partition_1;
    particiones[1] = mbr.mbr_partition_2;
    particiones[2] = mbr.mbr_partition_3;
    particiones[3] = mbr.mbr_partition_4;

    int i;
    for (i = 0; i < 4; i++) {
        printf("\tParticion %i:%s\n", i + 1, particiones[i].part_name);
        printf("\t\tStatus:%c", particiones[i].part_status);
        printf("\tTipo  :%c", particiones[i].part_type);
        printf("\tAjuste:%c", particiones[i].part_fit);
        printf("\t\tInicio:%i", particiones[i].part_start);
        printf("\t\tTamaño:%i", particiones[i].part_size);
        printf("\t\tFinal:%i\n", particiones[i].part_start + particiones[i].part_size);
    }
    printf("\t................................................................................................\n");

}

void reporteEBR(char ruta[sizeChar]) {
    printf("\t................................................EBR.............................................\n");
    bloqueMBR mbr = leerMBR(ruta);
    //buscando la posicion de la partición logica
    partition particiones[4];
    particiones[0] = mbr.mbr_partition_1;
    particiones[1] = mbr.mbr_partition_2;
    particiones[2] = mbr.mbr_partition_3;
    particiones[3] = mbr.mbr_partition_4;

    bool hayExtendida = false;
    int i;
    for (i = 0; i < 4; i++) {
        if (particiones[i].part_type == 'e') {
            hayExtendida = true;
            break;
        }
    }
    int inicio = 0;
    int tamano = 0;
    int espacioDisponible = 0;
    if (hayExtendida == true) {
        printf("\tLa partición extendida es=%i|Nombre: %s\n", i + 1, particiones[i].part_name);
        inicio = particiones[i].part_start;
        tamano = particiones[i].part_size;
        espacioDisponible = inicio + tamano;
        FILE *f;
        if ((f = fopen(ruta, "r+b")) == NULL) {
            printf("\t[ERROR]error al abrir el disco!\n");
        } else {
            bloqueEBR ebr;
            printf("\tinicio = %i\n", inicio);
            fseek(f, inicio, SEEK_SET);
            fread(&ebr, sizeof (bloqueEBR), 1, f);
            if (ebr.part_fit == 'b' || ebr.part_fit == 'f' || ebr.part_fit == 'w') {
                //Si se encontró el primer ERB
                printf("\tNombre:%s\n", ebr.part_name);
                printf("\t\tStatus:%c", ebr.part_status);
                printf("\tAjuste:%c", ebr. part_fit);
                printf("\tInicio:%i", ebr.part_start);
                printf("\tTamaño:%i", ebr.part_size);
                printf("\tPosSiguiente:%i \n", ebr.part_next);

                int siguiente = ebr.part_next;
                while (true) {
                    bloqueEBR aux_ebr;
                    fseek(f, siguiente, SEEK_SET);
                    fread(&aux_ebr, sizeof (bloqueEBR), 1, f);
                    if (aux_ebr.part_fit == 'b' || aux_ebr.part_fit == 'f' || aux_ebr.part_fit == 'w') {
                        printf("\tNombre:%s\n", aux_ebr.part_name);
                        printf("\t\tStatus:%c ", aux_ebr.part_status);
                        printf("\tAjuste:%c", aux_ebr. part_fit);
                        printf("\tInicio:%i", aux_ebr.part_start);
                        printf("\tTamaño:%i ", aux_ebr.part_size);
                        printf("\tPosSiguiente:%i \n", aux_ebr.part_next);
                        siguiente = aux_ebr.part_next;
                        //es que si hay siguiente
                    } else {
                        break;
                    }
                }
            } else {
                puts("\t\t No se encontró el primer EBR");
            }
            fclose(f);
        }
    } else {
        puts("/t[ERROR]No se ha creado una partición extendida");
    }
    printf("\t................................................................................................\n");
}



//del superBloque

void reporteEXT(char id[sizeChar]) {
    printf("\t...........................SB:%s......................................\n", id);
    superBloque sb = sb_retornar(id);
    printf("\t\tFechaMontaje =%s\tFechaDesmontaje=%s\n", sb.s_mtime, sb.s_unmtime);
    printf("\t\tNo.Montajes=%i\n", sb.s_mnt_count);

    printf("\t\tNumeros de Inodos =%i\tNúmero de Bloques=%i\n", sb.s_inodes_count, sb.s_blocks_count);
    printf("\t\tInodos Libres  =  %i\tBloques libres  =  %i\n", sb.s_free_blocks_counts, sb.s_free_inodes_count);
    printf("\t\tTamaño de Inodos  =%i\tTamaño de Bloques =%i\n", sb.s_inode_size, sb.s_block_size);
    printf("\t\tPrimer.InodoLibre =%i\tPrimer.BloqueLibre=%i\n", sb.s_first_ino, sb.s_first_blo);
    printf("\t\tInicioBitMapInodo =%i\tInicioBitMapBloque=%i\n", sb.s_bm_inode_start, sb.s_bm_block_start);
    printf("\t\tInicio de Inodos = %i\tInicio de Bloques =%i\n", sb.s_inode_start, sb.s_block_start);
    printf("\t\tPrimer Jorunal libre = %i\t\n", sb.s_bjpurfree);



    //bitmap de indos

    mnt_nodo mountNodo = retornarNodoMount(id);
    int n = sb.s_inodes_count;

    bmInodo aule[n];
    bmi_leer(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, aule);
    int p;
    printf("\t.....................BitMap de inodos:................................\n\t");


    for (p = 0; p < n; p++) {
        printf("%c", aule[p].status);
    }
    printf("\n");
    printf("\t....................BitMap de bloques:................................\n\t");
    //bit map de bloques 
    bmBloque matrBloque[3 * n];
    bmb_leer(sb.s_bm_block_start, n, mountNodo.mnt_ruta, matrBloque);
    int k;


    for (k = 0; k < 3 * n; k++) {
        printf("%c", matrBloque[k].status);
    }
    printf("\n");

    printf("\t................................Inodos:................................\n\t");
    //indos
    inodo matrInodo[n];
    inodos_leer(sb.s_inode_start, n, mountNodo.mnt_ruta, matrInodo);
    int q;

    for (q = 0; q < 4; q++) {
        int u;
        printf("\nInodo %i\n\t",q);
        for (u = 0; u < 11; u++) {
            printf("%i->Ap:%i|,", u, matrInodo[q].i_block[u]);
        }
    }
/*
    printf("\t................................Bloques:................................\n\t");
    //indos
    bloqueCarpeta carpeta;
    // blqcarp_escribir(sb.s_block_start, 1, partMontada.ruta, carpeta);
    int os;
    for (os = 0; os < 10; os++) {
        //void blqcarp_leer(int inicio, int n, char ruta[sizeChar], bloqueCarpeta *carpeta) 
        //blqcarp_leer(sb.s_block_start, os, mountNodo.mnt_ruta, carpeta);
        carpeta = blqcarp_leer(sb.s_block_start, os, mountNodo.mnt_ruta);
        int ku;
        printf("Ap %i\n\t", os);
        for (ku = 0; ku < 4; ku++) {
            printf("|Nom: %s, Ind: %i|", carpeta.b_content[ku].b_name, carpeta.b_content[ku].b_inodo);
        }
    }
*/

    printf("\n");
}

