/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "disco.h"
#include "estructuras.h"

#define tamaDiscoDuro 1000000



bool fdisk_ocurrioError = false;
char metodoDeColocacionExtendida = 'w';
/**************************************************************
 * Prototipos                                                *** 
 **************************************************************/
void fecha();
int tam_disco(int entrada);

bloqueMBR crearMBR();
bloqueMBR leerMBR(char ruta[sizeChar]);
void actualizarMBR(bloqueMBR mbr, char ruta[sizeChar]);



void crearParticionLogica(int size, char ruta[sizeChar], char name[sizeChar], char fit[sizeChar]);
void crearParticion(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add);
void eliminarParticion(char ruta[sizeChar], char name[sizeChar], char delete[sizeChar]);
partition devolverParticion(char ruta[sizeChar], char nombre[sizeChar]);
bloqueEBR devolverLogica(char ruta[sizeChar], char nombre[sizeChar]);

superBloque sb_inicializar(int n, times tiempo, int inicio);
void sb_escribir(char ruta[sizeChar], int inicio, superBloque sb);
superBloque sb_retornar(char id[sizeChar]);

void jr_escribir(int inicio, int n, char ruta[sizeChar], journalie aux);

void bloqueApuntador_escribir(int inicio, int n, char ruta[sizeChar], bloqueApuntador aux);
void inodos_escribir(int inicio, int n, char ruta[sizeChar], inodo aux);
void bmi_escribir(int inicio, int n, char ruta[sizeChar], bmInodo aux[]);

//Lista
mnt_nodo* mntCrearNodo(partition particion, bloqueEBR logica, char ruta[sizeChar]);
void mntInsertarAlFinal(mnt_lista* lista, partition particion, bloqueEBR logica, char ruta[sizeChar]);
void mntPush(partition particion, bloqueEBR logica, char ruta[sizeChar]);
char letraDeDisco(mnt_lista*lista, char ruta[sizeChar]);
char numeroDeDisco(mnt_lista*lista, char letra);
mnt_nodo retornarNodoMount(char ids[sizeChar]);


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
 * REPORTES                                                 *** 
 **************************************************************/
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




/**************************************************************
 * Lista MOUNT                                               *** 
 **************************************************************/

mnt_lista* listaDeParticiones;

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

void inicializarListaMount() {
    listaDeParticiones = (mnt_lista*) malloc(sizeof (mnt_lista)); //inicializando las listas
    listaDeParticiones->cabeza = NULL;
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

//double floor(double x);

/**************************************************************
 * COMANDOS                                                 *** 
 **************************************************************/
void mkfs(char id[sizeChar], char unit, char type[sizeChar], int add) {

    /*
        printf("\tId= %s\n", id);
        printf("\tUnidad= %c\n", unit);
        printf("\tTipo= %s\n", type);
        printf("\tAdd= %i\n", add);
     */

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

        superBloque sb = sb_inicializar(n, particion.tiempo, part_inicio + pimerEspacioEBR);

        sb_escribir(particion.mnt_ruta, part_inicio + pimerEspacioEBR, sb); //Escribiendo el super bloque


        journalie jr;
        jr_escribir(part_inicio + pimerEspacioEBR + sizeof (superBloque), n, particion.mnt_ruta, jr);


        
        //mandando el bloque de inodos
        bmInodo bmi[n];
        bmInodo bm;
        bm.status = '0';
        int l;
        for (l = 0; l < n + 1; l++) {
            bmi[l] = bm;
        }
        bmi[2].status = 'a';

        bmi_escribir(sb.s_bm_inode_start, n, particion.mnt_ruta, bmi);
        
        bmBloque bmb;
        bmb_escribir(sb.s_bm_block_start, n, particion.mnt_ruta, bmb);

        //hay que escribir el super bloque
        //JOURNALING
    }

}

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
    sb.s_bjpurfree=0;
    return sb;
}

void jr_escribir(int inicio, int n, char ruta[sizeChar], journalie aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < 3 * n + 1; j++) {
            fseek(f, inicio + j * (sizeof (journalie)), SEEK_SET);
            fwrite(&aux, sizeof (journalie), 1, f);
        }
        fclose(f);
    }
}

void bmi_escribir(int inicio, int n, char ruta[sizeChar], bmInodo aux[]) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n + 1; j++) {
            fseek(f, inicio + j * (sizeof (bmInodo)), SEEK_SET);
            bmInodo aux2 = aux[j];
            fwrite(&aux2, sizeof (bmInodo), 1, f);
        }
        fclose(f);
    }
}

void bmb_escribir(int inicio, int n, char ruta[sizeChar], bmBloque aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n * 3 + 1; j++) {
            fseek(f, inicio + j * (sizeof (bmBloque)), SEEK_SET);
            fwrite(&aux, sizeof (bmBloque), 1, f);
        }
        fclose(f);
    }
}

void arreglo(int numeros[]) {

}

void inodos_escribir(int inicio, int n, char ruta[sizeChar], inodo aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n * 3 + 1; j++) {
            fseek(f, inicio + j * (sizeof (inodo)), SEEK_SET);
            fwrite(&aux, sizeof (inodo), 1, f);
        }
        fclose(f);
    }
}

void bloqueApuntador_escribir(int inicio, int n, char ruta[sizeChar], bloqueApuntador aux) {
    FILE *f;
    if ((f = fopen(ruta, "r+b")) == NULL) {
        printf("\t[ERROR]error al abrir el disco!\n");
    } else {
        int j;
        for (j = 0; j < n * 3 + 1; j++) {
            fseek(f, inicio + j * (sizeof (bloqueApuntador)), SEEK_SET);
            fwrite(&aux, sizeof (bloqueApuntador), 1, f);
        }
        fclose(f);
    }
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

void sb_reporte(char id[sizeChar]) {
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
    printf("'t\tPrimer Jorunal libre = %i",sb.s_bjpurfree);
    printf("\t.......................................................................\n");

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

    //imprimirListaDeParticionesMontadas();
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

    if (unit == 'k')
        size = size * 1000;
    else if (unit == 'm')
        size = size * 1000000;

    if (borrar == 0) {//eliminar particion
        puts("\t        .........Eliminando partición........");

        eliminarParticion(ruta, name, delete);
    } else if (add != 0) {
        puts("Agregar o restar");
    } else {//se crea una particion
        crearParticion(size, ruta, name, unit, type, fit, delete, add);
    }
}

/**************************************************************
 * PARTICIONES                                              *** 
 **************************************************************/



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

void crearParticionLogica(int size, char ruta[sizeChar], char name[sizeChar], char fit[sizeChar]) {
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
    if (hayExtendida == true) {//Hay que agregar la particion extendida antes de la anterior

        printf("\tLa partición extendida es Particion_%i\n", i + 1);
        inicio = particiones[i].part_start;
        tamano = particiones[i].part_size;
        espacioDisponible = inicio + tamano;
        FILE *f;
        if ((f = fopen(ruta, "r+b")) == NULL) {
            printf("\t[ERROR]error al abrir el disco!\n");
        } else {
            bloqueEBR ebr;
            printf("\tInicio = %i\n", inicio);
            printf("\tTamañoExtendida=%i\n", tamano);

            fseek(f, inicio, SEEK_SET);
            fread(&ebr, sizeof (bloqueEBR), 1, f);
            if (ebr.part_fit == 'b' || ebr.part_fit == 'f' || ebr.part_fit == 'w') {
                bloqueEBR bloqueAnterior;
                bloqueAnterior = ebr;
                int siguiente = ebr.part_next;
                bloqueEBR aux_ebr;
                while (true) {
                    fseek(f, siguiente, SEEK_SET);
                    fread(&aux_ebr, sizeof (bloqueEBR), 1, f);
                    if (aux_ebr.part_fit == 'b' || aux_ebr.part_fit == 'f' || aux_ebr.part_fit == 'w') {
                        siguiente = aux_ebr.part_next;
                        bloqueAnterior = aux_ebr;
                        //es que si hay siguiente
                    } else {//es el ultimo bloque
                        //agregar la direccion al bloque anterior
                        aux_ebr.part_start = bloqueAnterior.part_start + sizeof (bloqueEBR) + bloqueAnterior.part_size + 1;
                        fseek(f, aux_ebr.part_start, SEEK_SET);
                        aux_ebr.part_status = '0';
                        aux_ebr.part_fit = fit[0];
                        //aux_ebr.part_start = bloqueAnterior->part_start+sizeof(bloqueEBR)+bloqueAnterior->part_size+1;
                        aux_ebr.part_size = size;
                        aux_ebr.part_next = -1;
                        //  printf("\tEl nombre que le voy a poner es = %s\n", name);

                        strcpy(aux_ebr.part_name, name);
                        bloqueAnterior.part_next = aux_ebr.part_start;
                        if (aux_ebr.part_start + aux_ebr.part_size > espacioDisponible) {
                            printf("\t[ERROR]%i>%iEl tamaño supera el espacio disponible en la partición extendida\n", aux_ebr.part_start + aux_ebr.part_size, espacioDisponible);
                        } else {
                            fwrite(&aux_ebr, sizeof (bloqueEBR), 1, f);
                            fseek(f, bloqueAnterior.part_start, SEEK_SET);
                            fwrite(&bloqueAnterior, sizeof (bloqueEBR), 1, f);
                            puts("\tSe creo la partición logica");
                        }


                        break;
                    }
                }
                //leer el mbr
            } else {
                fseek(f, inicio, SEEK_SET);
                //crear el mbr
                ebr.part_status = '0';
                ebr.part_fit = fit[0];
                ebr.part_start = inicio;
                ebr.part_size = size;
                ebr.part_next = -1;
                strcpy(ebr.part_name, name);
                fwrite(&ebr, sizeof (bloqueEBR), 1, f);
                puts("\tSe escribió el primer EBR");
            }
            fclose(f);
        }

    } else {
        puts("/t[ERROR]No se ha creado una partición extendida");
    }
    printf("\t.....................................................\n");
}

void crearParticion(int size, char ruta[sizeChar], char name[sizeChar], char unit, char type, char fit[sizeChar], char delete[sizeChar], int add) {
    puts("\t             .......Creando partición.......");
    if (type == 'l') {

        crearParticionLogica(size, ruta, name, fit);
        return;
    }

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

bloqueEBR devolverLogica(char ruta[sizeChar], char nombre[sizeChar]) {
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
    bool seEncontroLogica = false;

    if (hayExtendida == true) {
        //printf("\tLa partición extendida es=%i|Nombre: %s\n", i + 1, particiones[i].part_name);
        inicio = particiones[i].part_start;
        tamano = particiones[i].part_size;
        espacioDisponible = inicio + tamano;
        FILE *f;
        if ((f = fopen(ruta, "r+b")) == NULL) {
            printf("\t[ERROR]error al abrir el disco!\n");
        } else {
            bloqueEBR ebr;
            //printf("\tinicio = %i\n", inicio);
            fseek(f, inicio, SEEK_SET);
            fread(&ebr, sizeof (bloqueEBR), 1, f);
            if (ebr.part_fit == 'b' || ebr.part_fit == 'f' || ebr.part_fit == 'w') {
                //Si se encontró el primer ERB
                int result = strncmp(ebr.part_name, nombre, sizeChar);
                if (result == 0) {
                    metodoDeColocacionExtendida = particiones[i].part_fit;
                    seEncontroLogica = true;
                    return ebr;
                }
                int siguiente = ebr.part_next;

                while (true) {
                    bloqueEBR aux_ebr;
                    fseek(f, siguiente, SEEK_SET);
                    fread(&aux_ebr, sizeof (bloqueEBR), 1, f);
                    if (aux_ebr.part_fit == 'b' || aux_ebr.part_fit == 'f' || aux_ebr.part_fit == 'w') {
                        int resultado = strncmp(aux_ebr.part_name, nombre, sizeChar);
                        if (resultado == 0) {
                            metodoDeColocacionExtendida = particiones[i].part_fit;
                            seEncontroLogica = true;
                            return aux_ebr;
                            break;
                        } else {
                            siguiente = aux_ebr.part_next;
                        }
                        //es que si hay siguiente
                    } else {
                        break;
                    }
                }
            }
            //else NO hay primer EBR
            fclose(f);
        }
    }
    if (seEncontroLogica == false) {
        bloqueEBR retor;
        return retor;
    }
    /*
        //////////////////////////////////////////////
        //buscando dentro de una particion extendida//
        //////////////////////////////////////////////
         bloqueMBR mbr = leerMBR(ruta);
        //buscando la posicion de la partición logica
        partition particiones[4];
        particiones[0] = mbr.mbr_partition_1;
        particiones[1] = mbr.mbr_partition_2;
        particiones[2] = mbr.mbr_partition_3;
        particiones[3] = mbr.mbr_partition_4;

        bool seEncontroLaParticion = false;
        int i;
        for (i = 0; i < 4; i++) {
            if (particiones[i].part_type = 'e') {
                seEncontroLaParticion = true;
                break;
            }
        }
        //ahora hay que buscar dentro de la partición extendida que se encontro para retornar el ebr
    
    
    
        if (seEncontroLaParticion == true) {
            return particiones[i];
        } else {
            //no se encontro la partición
            partition retorno;
            return retorno;
        }
     */
}

partition devolverParticion(char ruta[sizeChar], char nombre[sizeChar]) {//hay que buscar dentro de las particiones extendidas
    bloqueMBR mbr = leerMBR(ruta);
    //buscando la posicion de la partición logica
    partition particiones[4];
    particiones[0] = mbr.mbr_partition_1;
    particiones[1] = mbr.mbr_partition_2;
    particiones[2] = mbr.mbr_partition_3;
    particiones[3] = mbr.mbr_partition_4;

    bool seEncontroLaParticion = false;
    int o;
    for (o = 0; o < 4; o++) {
        int resultado = strncmp(particiones[o].part_name, nombre, sizeChar);
        if (resultado == 0) {
            if (particiones[o].part_type == 'p') {
                seEncontroLaParticion = true;
                return particiones[o];
            } else {
                break;
            }
        }
    }

    if (seEncontroLaParticion == false) {
        partition reto;
        return reto;
    }


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

