/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "disco.h"
#include "estructuras.h"

#define tamaDiscoDuro 1000000



bool fdisk_ocurrioError = false;

/**************************************************************
 * Prototipos                                                *** 
 **************************************************************/
void fecha();
int tam_disco(int entrada);

bloqueMBR crearMBR();
bloqueMBR leerMBR(char ruta[sizeChar]);
void actualizarMBR(bloqueMBR mbr, char ruta[sizeChar]);


void crearParticion(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add);
void eliminarParticion(char ruta[sizeChar], char name[sizeChar], char delete[sizeChar]);

void errorDeEspacio();

/**************************************************************
 * Funciones                                                *** 
 **************************************************************/

int tam_disco(int entrada) {
    if (entrada % 4 == 0) {
        return entrada;
    } else {
        entrada++;
        return tam_disco(entrada);
    }
}

void fecha() {/*Obtengo la fecha actual del sistema*/

    time_t tiem = time(0);
    //struct t*tlocal = localtime(&tiem);
    //strftime(mbr.mbr_fecha_creacion, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);
}

bloqueMBR crearMBR(int tamanioDeDisco) {
    bloqueMBR mbr2;
    time_t tiem = time(0);
    struct t*tlocal = localtime(&tiem);
    strftime(mbr2.mbr_fecha_creacion, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);
    mbr2.mbr_tamano = tamanioDeDisco;
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

void errorDeEspacio() {
    printf("[ERROR AL PARTICIONAR]El tamaño de la particion supera el espacio disponible\n");
    fdisk_ocurrioError = true;
}

/**************************************************************
 * DISCO                                                    *** 
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

void fdisk(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add) {
    /*
     * parametros opcionales
     */
    fdisk_ocurrioError = false;

    if (unit == ' ')
        unit = 'k';
    if (type == ' ')
        type = 'p';
    int fitp = 1;
    fitp = strncmp("wf", fit, sizeChar);
    if (fitp != 0)
        fitp = strncmp("bf", fit, sizeChar);
    else
        if (fitp != 0)
        fitp = strncmp("ff", fit, sizeChar);

    if (fitp != 0)
        strcpy(fit, "wf");
    /*
     * Fn parametros opcionales        
     */

    int borrar = 1;
    borrar = strncmp("fast", delete, sizeChar);
    if (borrar != 0)
        borrar = strncmp("full", delete, sizeChar);

    if (borrar == 0) {//eliminar particion
        puts("\t        .........Eliminando partición........");

        eliminarParticion(ruta, name, delete);
    } else if (add != 0) {
        puts("Agregar o restar");
    } else {//se crea una particion
        crearParticion(size, ruta, name, unit, type, fit, delete, add);
    }
}

void eliminarParticion(char ruta[sizeChar], char name[sizeChar], char delete[sizeChar]) {
    bloqueMBR mbr = leerMBR(ruta);
    partition particiones[4];
    particiones[0] = mbr.mbr_partition_1;
    particiones[1] = mbr.mbr_partition_2;
    particiones[2] = mbr.mbr_partition_3;
    particiones[3] = mbr.mbr_partition_4;
    int i;
    char nombre[sizeChar];
    char nombre2[sizeChar];

    bool encontrado = false;
    for (i = 0; i < 4; i++) {
        strcpy(nombre, particiones[i].part_name);
        strcpy(nombre2, name);

        if (strncmp(nombre2, nombre, sizeChar) == 0) {
            encontrado = true;
            break;
        }
    }
    if (encontrado == true) {


        printf("\t\tEliminando la Particion%i, Nombre=%s\n", i + 1, particiones[i].part_name);
       
        if (strncmp("fast", delete, sizeof ("fast")) == 0) {
             printf("\t\tTipo =fast, size %d\n", sizeof (delete));
            //elimminacion fast
            particiones[i].part_size = 0;
            particiones[i].part_start = 0;
            particiones[i].part_fit = ' ';
            strcpy(particiones[i].part_name, "");
            particiones[i].part_status = ' ';
            particiones[i].part_type = ' ';


        } else {
             printf("\t\tTipo =full, size %d\n", sizeof (delete));
            //elimminacion full
            FILE *f;
            if ((f = fopen(ruta, "r+b")) == NULL) {
                printf("\t\t[ERROR]error al leer el disco!\n");
            } else {
                fseek(f, particiones[i].part_start, SEEK_SET);
                //llenando los espacios en blanco
                char vacio = '\0';
                int i = 0;
                for (i = 0; i < particiones[i].part_size; i++) {
                    fwrite(&vacio, 1, 1, f);
                }
                fclose(f);               
            }

            particiones[i].part_size = 0;
            particiones[i].part_start = 0;
            particiones[i].part_fit = ' ';
            strcpy(particiones[i].part_name, "");
            particiones[i].part_status = ' ';
            particiones[i].part_type = ' ';
            //eliminacion full
        }

        if (i == 0)
            mbr.mbr_partition_1 = particiones[i];
        else if (i == 1)
            mbr.mbr_partition_2 = particiones[i];
        else if (i == 2)
            mbr.mbr_partition_3 = particiones[i];
        else if (i == 3)
            mbr.mbr_partition_4 = particiones[i];

        actualizarMBR(mbr, ruta);
    } else {
        printf("\t\t[ERROR]No se econtro partición con el nombre de %s\n", name);
    }
    puts("\t.....................................................");

}

void crearParticion(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add) {
    puts("\t             .......Creando partición.......");
    bloqueMBR mbr = leerMBR(ruta);
    /*
            estadoDeParticiones(mbr);
     */
    int tamanoDisponible = 0;
    int tamanoDeParticion = size; //tiene que estar en bytes
    //buscando espacio para insertar la particion
    int particionActiva[5] = {0, 0, 0, 0, 0};
    int particionSeleccionada = 0;


    int tamanoDisponible1 = 0;
    int tamanoDisponible2 = 0;
    int tamanoDisponible3 = 0;
    int tamanoDisponible4 = 0;


    if (mbr.mbr_partition_1.part_size != 0)
        particionActiva[1] = 1;
    if (mbr.mbr_partition_2.part_size != 0)
        particionActiva[2] = 1;
    if (mbr.mbr_partition_3.part_size != 0)
        particionActiva[3] = 1;
    if (mbr.mbr_partition_4.part_size != 0)
        particionActiva[4] = 1;
    /*
     * Las posibles combinaciones de como se encuentren los espacios dentro del disco, las cuales existen 16 posibles combinaciones
     */


    printf("\t\tDisponibles dsk1=%i, dsk2=%i,dsk3=%i,dsk4=%i\n", particionActiva[1], particionActiva[2], particionActiva[3], particionActiva[4]);
    // while (true) {
    if (particionActiva[1] == 0 && particionActiva[2] == 0 && particionActiva[3] == 0 && particionActiva[4] == 0) {//1          //0000
        tamanoDisponible = mbr.mbr_tamano - (sizeof (bloqueMBR) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 1;
            //break;
        }
    } else if (particionActiva[1] == 0 && particionActiva[2] == 0 && particionActiva[3] == 0 && particionActiva[4] == 1) {//2    //0001
        tamanoDisponible = mbr.mbr_partition_4.part_start - (sizeof (bloqueMBR) + 1);
        if (tamanoDeParticion > tamanoDisponible)
            errorDeEspacio();
        else
            particionSeleccionada = 1;
    } else if (particionActiva[1] == 0 && particionActiva[2] == 0 && particionActiva[3] == 1 && particionActiva[4] == 0) {//3    //0010

        tamanoDisponible1 = mbr.mbr_partition_3.part_start - (sizeof (bloqueMBR) + 1);
        tamanoDisponible4 = mbr.mbr_tamano - ((mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size) + 1);

        if (tamanoDeParticion > tamanoDisponible1) {
            if (tamanoDeParticion > tamanoDisponible4) {
                errorDeEspacio();
            } else {
                tamanoDisponible = tamanoDisponible4;
                particionSeleccionada = 4;
            }
        } else {
            tamanoDisponible = tamanoDisponible1;
            particionSeleccionada = 1;
        }

    } else if (particionActiva[1] == 0 && particionActiva[2] == 0 && particionActiva[3] == 1 && particionActiva[4] == 1) {//4    //0011
        tamanoDisponible = mbr.mbr_partition_3.part_start - (sizeof (bloqueMBR) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 1;
        }

    } else if (particionActiva[1] == 0 && particionActiva[2] == 1 && particionActiva[3] == 0 && particionActiva[4] == 0) {//5    //0100
        tamanoDisponible1 = mbr.mbr_partition_2.part_start - (sizeof (bloqueMBR) + 1);
        tamanoDisponible3 = mbr.mbr_tamano - ((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size) + 1);

        if (tamanoDeParticion > tamanoDisponible1) {
            if (tamanoDeParticion > tamanoDisponible3) {
                errorDeEspacio();
            } else {
                tamanoDisponible = tamanoDisponible3;
                particionSeleccionada = 3;
            }
        } else {
            tamanoDisponible = tamanoDisponible1;
            particionSeleccionada = 1;
        }

    } else if (particionActiva[1] == 0 && particionActiva[2] == 1 && particionActiva[3] == 0 && particionActiva[4] == 1) {//6    //0101
        tamanoDisponible1 = mbr.mbr_partition_2.part_start - (sizeof (bloqueMBR) + 1);
        tamanoDisponible3 = mbr.mbr_partition_4.part_start - ((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size) + 1);

        if (tamanoDeParticion > tamanoDisponible1) {
            if (tamanoDeParticion > tamanoDisponible3) {
                errorDeEspacio();
            } else {
                tamanoDisponible = tamanoDisponible3;
                particionSeleccionada = 3;
            }
        } else {
            tamanoDisponible = tamanoDisponible3;
            particionSeleccionada = 1;
        }
    } else if (particionActiva[1] == 0 && particionActiva[2] == 1 && particionActiva[3] == 1 && particionActiva[4] == 0) {//7    //0110
        tamanoDisponible1 = mbr.mbr_partition_2.part_start - (sizeof (bloqueMBR) + 1);
        tamanoDisponible4 = mbr.mbr_tamano - ((mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size) + 1);

        if (tamanoDeParticion > tamanoDisponible1) {
            if (tamanoDeParticion > tamanoDisponible4) {
                errorDeEspacio();
            } else {
                tamanoDisponible = tamanoDisponible4;
                particionSeleccionada = 4;
            }
        } else {
            tamanoDisponible = tamanoDisponible1;
            particionSeleccionada = 1;
        }

    } else if (particionActiva[1] == 0 && particionActiva[2] == 1 && particionActiva[3] == 1 && particionActiva[4] == 1) {//8    //0111
        tamanoDisponible = mbr.mbr_partition_2.part_start - (sizeof (bloqueMBR) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 1;
        }

    } else if (particionActiva[1] == 1 && particionActiva[2] == 0 && particionActiva[3] == 0 && particionActiva[4] == 0) {//9    //1000
        tamanoDisponible = mbr.mbr_tamano - ((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 2;
        }
    } else if (particionActiva[1] == 1 && particionActiva[2] == 0 && particionActiva[3] == 0 && particionActiva[4] == 1) {//10   //1001
        tamanoDisponible = mbr.mbr_partition_4.part_start - ((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 2;
        }
    } else if (particionActiva[1] == 1 && particionActiva[2] == 0 && particionActiva[3] == 1 && particionActiva[4] == 0) {//11   //1010

        tamanoDisponible2 = mbr.mbr_partition_3.part_start - ((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size) + 1);
        tamanoDisponible4 = mbr.mbr_tamano - ((mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size) + 1);

        if (tamanoDeParticion > tamanoDisponible2) {
            if (tamanoDeParticion > tamanoDisponible4) {
                errorDeEspacio();
            } else {
                tamanoDisponible = tamanoDisponible4;
                particionSeleccionada = 4;
            }
        } else {
            tamanoDisponible = tamanoDisponible2;
            particionSeleccionada = 2;
        }

    } else if (particionActiva[1] == 1 && particionActiva[2] == 0 && particionActiva[3] == 1 && particionActiva[4] == 1) {//12   //1011
        tamanoDisponible = mbr.mbr_partition_3.part_start - ((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 2;
        }
    } else if (particionActiva[1] == 1 && particionActiva[2] == 1 && particionActiva[3] == 0 && particionActiva[4] == 0) {//13   //1100
        tamanoDisponible = mbr.mbr_tamano - ((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 3;
        }
    } else if (particionActiva[1] == 1 && particionActiva[2] == 1 && particionActiva[3] == 0 && particionActiva[4] == 1) {//14   //1101
        tamanoDisponible = mbr.mbr_partition_4.part_start - ((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 3;
        }

    } else if (particionActiva[1] == 1 && particionActiva[2] == 1 && particionActiva[3] == 1 && particionActiva[4] == 0) {//15   //1110
        tamanoDisponible = mbr.mbr_tamano - ((mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size) + 1);
        if (tamanoDeParticion > tamanoDisponible) {
            errorDeEspacio();
        } else {
            particionSeleccionada = 4;
        }

    } else if (particionActiva[1] == 1 && particionActiva[2] == 1 && particionActiva[3] == 1 && particionActiva[4] == 1) {//16   //1111
        errorDeEspacio();
    }
    //}

    printf("\t\tEl tamaño disponible %i\n", tamanoDisponible);
    if (fdisk_ocurrioError == false) {




        if (unit == 'k')
            tamanoDeParticion = tamanoDeParticion * 1000;
        else if (unit == 'm')
            tamanoDeParticion = tamanoDeParticion * 1000000;
        printf("\t\tTamaño de la partición a crear %iB\n", tamanoDeParticion);
        printf("\t\tSe escribirá en la particion No. %i\n", particionSeleccionada);
        switch (particionSeleccionada) {
            case 0:

                //no hay seleccion
                break;
            case 1:
                mbr.mbr_partition_1.part_status = '0';
                mbr.mbr_partition_1.part_type = type;
                mbr.mbr_partition_1.part_fit = fit[0];
                mbr.mbr_partition_1.part_start = (sizeof (bloqueMBR) + 1);
                mbr.mbr_partition_1.part_size = tamanoDeParticion;
                strcpy(mbr.mbr_partition_1.part_name, name);
                break;
            case 2:
                mbr.mbr_partition_2.part_status = '0';
                mbr.mbr_partition_2.part_type = type;
                mbr.mbr_partition_2.part_fit = fit[0];
                mbr.mbr_partition_2.part_start = (mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size + 1);
                mbr.mbr_partition_2.part_size = tamanoDeParticion;
                strcpy(mbr.mbr_partition_2.part_name, name);
                break;
            case 3:
                mbr.mbr_partition_3.part_status = '0';
                mbr.mbr_partition_3.part_type = type;
                mbr.mbr_partition_3.part_fit = fit[0];
                mbr.mbr_partition_3.part_start = (mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size + 1);
                mbr.mbr_partition_3.part_size = tamanoDeParticion;
                strcpy(mbr.mbr_partition_3.part_name, name);
                break;
            case 4:
                mbr.mbr_partition_4.part_status = '0';
                mbr.mbr_partition_4.part_type = type;
                mbr.mbr_partition_4.part_fit = fit[0];
                mbr.mbr_partition_4.part_start = (mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size + 1);
                mbr.mbr_partition_4.part_size = tamanoDeParticion;
                strcpy(mbr.mbr_partition_4.part_name, name);
                break;
        }
        actualizarMBR(mbr, ruta);
    }
    printf("\t.....................................................\n");
}

/*
 * 
         // Otra forma de buscar espacio es recorrere todas las particiones
         
        bool bander = true;

        int i;
        partition particiones[4];
        particiones[0] = mbr.mbr_partition_1;
        particiones[1] = mbr.mbr_partition_2;
        particiones[2] = mbr.mbr_partition_3;
        particiones[3] = mbr.mbr_partition_4;

        int posicionInicio = 0;
        int posicionFinal = 0;
        int posicionInsertar = 0;
        int posicionEspacio = 0;

        while (true) {
            for (i = 0; i < 4; i++) {
                if (i == 3) {
                    posicionFinal = mbr.mbr_tamano;
                    posicionInicio = particiones[i].part_start + particiones[i].part_size;
                    posicionEspacio = posicionFinal - posicionInicio;
                    
                    if (tamanoDeParticion <= posicionEspacio){
                        posicionEspacio = 4;
                        break;
                    }
                } else if (i == 0) {
                    posicionInicio = (sizeof (bloqueMBR) + 1);
                    posicionFinal = particiones[i + 1].part_start;
                    posicionEspacio = posicionFinal - posicionInicio;
                    if (tamanoDeParticion <= posicionEspacio){
                        posicionEspacio = 1;
                        break;
                    }
                } else {
                    posicionInicio = particiones[i].part_start + particiones[i].part_size;
                    posicionFinal = particiones[i + 1].part_start;
                    
                    posicionEspacio = posicionFinal - posicionInicio;
                    if (tamanoDeParticion <= posicionEspacio){
                        posicionEspacio = i;
                        break;
                    }
                }
                if(i==3)
                    break;
            }

        }
 */

