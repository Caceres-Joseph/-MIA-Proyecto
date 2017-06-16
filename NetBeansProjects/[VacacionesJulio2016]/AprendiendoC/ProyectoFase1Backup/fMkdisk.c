/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "Mbr.h"
#include "estructuras.h"

//definicion de funciones
int tam_disco(int entrada);



//metodos 

int tam_disco(int entrada) {
    if (entrada % 4 == 0) {
        return entrada;
    } else {
        entrada++;
        return tam_disco(entrada);
    }
}

void mkdisk(int size, char ruta[50], char nombre[50]) {

    strcat(ruta, nombre);
    printf("\t\tRuta: %s\n", ruta);
    FILE *f;
    size = tam_disco(size) * tamaDiscoDuro;
    printf("\t\tTamaño del disco :%ibytes\n", tam_disco(size));
    if ((f = fopen(ruta, "w+b")) == NULL) {
        printf("[ERROR]error al crear el disco!\n");
    } else {
        //escribir el mbr
        bloqueMBR mbr5 = crearMBR(size);
        fwrite(&mbr5, sizeof (bloqueMBR), 1, f);

        //llenando los espacios en blanco
        char vacio = '\0';
        int i = 0;

        for (i = sizeof (bloqueMBR); i < size; i++) {
            fwrite(&vacio, 1, 1, f);
        }

        fclose(f);

        printf("\t\tNombre del disco: %s\n", nombre);
        printf("\t\tTamaño del disco: %iMB\n", size / tamaDiscoDuro);
        puts("\t...................El disco fue creado..............");


    }
}

void rmkdisk(char ruta[sizeChar]) {
    int i = unlink(ruta);
    //printf("Num : %i\n", i);
    if (i == 0) {
        puts("\t..................El disco fue eliminado............");

    } else {
        puts("\t................El disco no fue eliminado............");
    }
}