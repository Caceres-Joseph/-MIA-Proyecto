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


#include "Mbr.h"
#include "estructuras.h"


void actualizarMBR(bloqueMBR mbr, char ruta[sizeChar]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fseek(f, 0, SEEK_SET);
        fwrite(&mbr, sizeof (bloqueMBR), 1, f);
        fclose(f);
    }
}

bloqueMBR leerMBR(char ruta[sizeChar]) {
    bloqueMBR retorno;
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fseek(f, 0, SEEK_SET);
        fread(&retorno, sizeof (bloqueMBR), 1, f);

        fclose(f);
    }
    return retorno;
}


bloqueMBR crearMBR(int tamanioDeDisc) {
    
    bloqueMBR mbr2;
    time_t tiem = time(0);
    struct t*tlocal = localtime(&tiem);
    strftime(mbr2.mbr_fecha_creacion, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);
    mbr2.mbr_tamano = tamanioDeDisc;
    int r = 10 + (rand() % 99);
    mbr2.mbr_disk_asignature = r;

    mbr2.mbr_partition_1.part_size = 0;
    mbr2.mbr_partition_2.part_size = 0;
    mbr2.mbr_partition_3.part_size = 0;
    mbr2.mbr_partition_4.part_size = 0;

    mbr2.mbr_partition_1.part_start = 0;
    mbr2.mbr_partition_2.part_start = 0;
    mbr2.mbr_partition_3.part_start = 0;
    mbr2.mbr_partition_4.part_start = 0;

    mbr2.mbr_partition_1.part_status = ' ';
    mbr2.mbr_partition_2.part_status = ' ';
    mbr2.mbr_partition_3.part_status = ' ';
    mbr2.mbr_partition_4.part_status = ' ';

    mbr2.mbr_partition_1.part_type = ' ';
    mbr2.mbr_partition_2.part_type = ' ';
    mbr2.mbr_partition_3.part_type = ' ';
    mbr2.mbr_partition_4.part_type = ' ';

    mbr2.mbr_partition_1.part_fit = ' ';
    mbr2.mbr_partition_2.part_fit = ' ';
    mbr2.mbr_partition_3.part_fit = ' ';
    mbr2.mbr_partition_4.part_fit = ' ';

    strcpy(mbr2.mbr_partition_1.part_name, "");
    strcpy(mbr2.mbr_partition_2.part_name, "");
    strcpy(mbr2.mbr_partition_3.part_name, "");
    strcpy(mbr2.mbr_partition_4.part_name, "");
    return mbr2;
}
