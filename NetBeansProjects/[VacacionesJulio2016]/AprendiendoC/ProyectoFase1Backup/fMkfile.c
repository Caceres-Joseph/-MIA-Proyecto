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
#include "fMkdir.h"

void mkfile(char id[sizeChar], char ruta[sizeChar], char p, int size, char count[sizeChar]) {
    printf("\tId=%s \n", id);
    printf("\tRuta= %s\n", ruta);
    printf("\tP= %c\n", p);
    printf("\tTamaño= %i\n", size);
    printf("\tCount= %s\n", count);
    //verParticionMontada(id);

    crearArchivo(id, ruta, size, count);
}

void crearArchivo(char id[sizeChar], char ruta[sizeChar], int size, char count[sizeChar]) {


    //primero hay que buscar el directorio haciendo split
    char buffer[sizeChar] = "";
    //char concat=
    int i;
    int indiceInodo = 0;
    int indTemp;
    for (i = 0; ruta[i] != '\0'; i++) {
        if (ruta[i] == '/') {
            //printf("\n\tDirectorio= %s", buffer);
            int resultado = strncmp(buffer, "", sizeChar);
            if (resultado == 0) {//indica que me mandaron la raíz.
                //printf("raíz\n");
                indiceInodo = 0;
            } else {
                printf("\t\t----Buscando %s en Inodo-%i-----\n", buffer, indiceInodo);
                indTemp = indiceInodoCarpetaEnApuntadorDirecto(id, indiceInodo, buffer);
                printf("El indice inodo temporal es = %i\n", indTemp);

                indiceInodo = indTemp; //me pasa el siguiente nodo 

            }
            memset(buffer, 0, sizeChar);

        } else {
            int o;

            for (o = 0; buffer[o] != '\0'; o++) {
            }

            buffer[o] = ruta[i];
        }
    }
    //aqui salgo del for y debo de insertar el archivo dentro del inodo de la carpeta

    
    printf("$$IndTem es %i\n", indTemp);
    printf("$$Indice Inodo es = %i\n", indiceInodo);
    printf("$$Buffer %s\n", buffer);
    printf("$$Size %i\n", size);
    crearArchivoEnInodo(id, indiceInodo, buffer, size);
    
    
}

void crearArchivoEnInodo(char id[sizeChar], int indiceInodoAInsertar, char nombreCarpeta[sizeChar], int size) {
    if (size == 0) {
        printf("size es igual a cero\n");
        size = 64;
    }
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    int n = sb.s_inodes_count;

    //la particion que tiene los datos
    particionMontada partMontada = devolverParticionMontada(id);


    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //bit map de bloques 
    bmBloque bm_Bloques[3 * n];
    bmb_leer(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);

    //journalie
    journalie jour[n * 3];
    jr_leer(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (journalie), n, partMontada.ruta, jour);

    //////////////////////////////////////////|>[aquí finalizo la lectura.]<|/////////////////////////////////////////////

    //superBloque
    //Journaling
    //bitMapInodos
    //bitMapBloques
    //Inodos
    //Blouqes



    //////////////////////////////////////////[Modificando el inodo y el Bloque anterior]

    int ind;
    int indiIno = 0;
    int punteroLibre = 0;
    for (ind = 0; ind < 12; ind++) {//recorriendo los bloques directos
        int nRetorno = Inodos[indiceInodoAInsertar].i_block[ind];
        if (nRetorno == -1) {
            punteroLibre = ind; //este es el puntero que esta libre
            break;
            //el bloque no apunta a nada
        } else if (ind == 0) {//este es el primer apuntador, hay que omitir los dos primeros.
            //tengo que obtener el bloque de este apuntador.
            indiIno = modificarBloqueCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 2); //escribir el inodo en el primer espacio libre y modificar el bloque de la carpeta
            if (indiIno == 0) {//si encontró un blouqe libre
                punteroLibre = 0;
                break;
            }
        } else {//hay que realizar la busqueda en otros bloques, y si no existe crearlo  
            indiIno = modificarBloqueCarpeta(sb, nRetorno, partMontada, nombreCarpeta, 0);
            if (indiIno == 0) {
                punteroLibre = 0;
                break;
            }
        }
    }
    printf("el puntero libre es el %i\n", punteroLibre);
    //tengo que definir cuantos bloques son los que voy a utilizar men.
    double residuo = size%64; //el tamaño es la cantidad de char a usar y cada bloque es de 64 bytes
    int numBloques= size/64+(int)residuo;
    printf("El numero de bloques a utilizar es de : %i\n",numBloques );
    
    
    if (punteroLibre != 0) {//Hay que crear un nuevo bloque en el puntero e inicializarlo con -1's
        //aquí se ocupa dos bloques
        content contenido;
        contenido.b_inodo = sb.s_first_ino;
        strcpy(contenido.b_name, nombreCarpeta);

        bloqueCarpeta bloqCarp;
        bloqCarp.b_content[0] = contenido;
        bloqCarp.b_content[1].b_inodo = -1;
        bloqCarp.b_content[2].b_inodo = -1;
        bloqCarp.b_content[3].b_inodo = -1;
        blqcarp_escribir(sb.s_block_start, sb.s_first_blo, partMontada.ruta, bloqCarp); //ya se modificó el blouqe
        //hay que modificar también el  superbloque
        Inodos[indiceInodoAInsertar].i_block[punteroLibre] = sb.s_first_blo; //aquí el puntero

        bm_Bloques[sb.s_first_blo].status = '1';
        sb.s_first_blo = bmb_primerLibre(n, bm_Bloques);
        sb.s_free_blocks_counts--;
    } else {//si cabe en el bloque
        //aquí ocupa un bloque // y ya apunta al primer inodo.

    }
    //////////////////////////////////////////[Modificando el nuevo inodo]//

    printf("\tEl primer inodo libre es = %i\n", sb.s_first_ino);
    Inodos[sb.s_first_ino].i_size = size; //el tamaño del archivo
    strcpy(Inodos[ sb.s_first_ino].i_atime, partMontada.part_time); //ultima fecha que se leyó el nodo sin modificarlo
    fechaActual(Inodos[ sb.s_first_ino].i_ctime);
    fechaActual(Inodos[ sb.s_first_ino].i_mtime);
    Inodos[ sb.s_first_ino].i_type = '1'; //0 si es carpeta


    //////////////////////////////////////////[Los nuevos bloques ]//
    int cantidadDeBlouqes = size / 64;
    int cantBlo;
    for (cantBlo = 0; cantBlo < cantidadDeBlouqes; cantBlo++) {
        if (cantBlo > 12) {
            printf("\tPasando a apuntadores indirectos\n");
            //SOBREPASA EL BLOQUE DE APUNTADORES INDIRECTOS
            break;
        }
        bloqueArchivo block;

        int i;
        for (i = 0; i < 64; i++) {
            block.b_content[i] = (char) i;
        }

        //blqArch_escribir(sb.s_block_start, sb.s_first_blo, partMontada.ruta, block); //escribo el bloque
        Inodos[ sb.s_first_ino].i_block[cantBlo] = sb.s_first_blo; //los apuntdores al bloque
        bm_Bloques[sb.s_first_blo].status = '1';
        sb.s_first_blo = bmb_primerLibre(n, bm_Bloques);
        sb.s_free_blocks_counts--;
    }

    //////////////////////////////////////////[BitMapDeBloques]//

    bmb_escribir(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);
    inodos_escribir(sb.s_inode_start, n, partMontada.ruta, Inodos); //escribiendo los inodos


    //////////////////////////////////////////[BitMapDeInodos]//
    bm_Inodos[sb.s_first_ino].status = '1';
    bmi_escribir(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);


    //////////////////////////////////////////[Journalin]//
    journalie journ;
    fechaActual(journ.journal_fecha);
    strcpy(journ.journal_nombre, nombreCarpeta);
    journ.journal_tipo = '0';
    jour[sb.s_bjpurfree] = journ;
    jr_escribir(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (journalie), n, partMontada.ruta, jour);
    puts("Aqui no entra men");
    //////////////////////////////////////////[SuperBloque]//
    sb.s_first_ino = bmi_primerLibre(n, bm_Inodos);
    sb.s_bjpurfree++;
    sb.s_first_blo = bmb_primerLibre(n, bm_Bloques);

    sb.s_free_inodes_count--;
    sb_escribir(partMontada.ruta, partMontada.part_inicio + partMontada.part_espacioEbr, sb);

}

/**************************************************************
 * ROOT y HOME                                              *** 
 **************************************************************/

void crearHome(char id[sizeChar]) {

    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);
    //mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;
    particionMontada partMontada = devolverParticionMontada(id);

    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //bit map de bloques 
    bmBloque bm_Bloques[3 * n];
    bmb_leer(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);

    ///////////////////////////////////////////////////////////////////////////////////////aquí finalizo la lectura.

    //hay que modificar el super bloque
    sb.s_free_blocks_counts--;
    sb.s_free_inodes_count--;
    sb.s_first_ino = 2;
    sb.s_first_blo = 2;
    sb.s_bjpurfree = 2;
    sb_escribir(partMontada.ruta, partMontada.part_inicio + partMontada.part_espacioEbr, sb);


    /*
        sb.s_first_ino = sb.s_first_ino + sizeof (inodo);
        sb.s_first_blo = sb.s_first_blo + sizeof (bloqueCarpeta);
        sb.s_bjpurfree = sb.s_bjpurfree + sizeof (journalie);
     */

    //Modificar el bitmap de inodos.
    bm_Inodos[1].status = '1';
    bmi_escribir(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //Modificando el bitmap de bloques
    bm_Bloques[1].status = '1';
    bmb_escribir(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //Modificando los inodos
    Inodos[1].i_size = 0; //el tamaño del archivo
    strcpy(Inodos[1].i_atime, partMontada.part_time); //ultima fecha que se leyó el nodo sin modificarlo
    fechaActual(Inodos[1].i_ctime);
    fechaActual(Inodos[1].i_mtime);
    Inodos[0].i_type = '0'; //0 si es carpeta
    Inodos[0].i_block[0] = 0; //apuntador primer bloque libre
    inodos_escribir(sb.s_inode_start, n, partMontada.ruta, Inodos); //escribiendo lo inodos

    content rootPadre;
    rootPadre.b_inodo = 0;
    strcpy(rootPadre.b_name, "/");

    //Modificando los bloques
    content home;
    home.b_inodo = -1;
    strcpy(home.b_name, "home");


    bloqueCarpeta carpeta;
    carpeta.b_content[0] = rootPadre;
    carpeta.b_content[1] = home;
    //en los otros dos van los hijos de root
    blqcarp_escribir(sb.s_block_start, 1, partMontada.ruta, carpeta);
};

void crearRoot(char id[sizeChar]) {
    //primero tengo que obtener el super bloque
    superBloque sb = sb_retornar(id);

    //mnt_nodo mountNodo = retornarNodoMount(id); //la particion que tiene los datos
    int n = sb.s_inodes_count;
    particionMontada partMontada = devolverParticionMontada(id);

    //bit map de indos.
    bmInodo bm_Inodos[n];
    bmi_leer(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //bit map de bloques 
    bmBloque bm_Bloques[3 * n];
    bmb_leer(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //indos
    inodo Inodos[n];
    inodos_leer(sb.s_inode_start, n, partMontada.ruta, Inodos);

    //journalie
    journalie jour[n * 3];
    jr_leer(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (journalie), n, partMontada.ruta, jour);


    ///////////////////////////////////////////////////////////////////////////////////////aquí finalizo la lectura.

    //hay que modificar el super bloque
    sb.s_free_blocks_counts--;
    sb.s_free_inodes_count--;
    sb.s_first_ino = 1;
    sb.s_first_blo = 1;
    sb.s_bjpurfree = 1;
    sb_escribir(partMontada.ruta, partMontada.part_inicio + partMontada.part_espacioEbr, sb);



    /*
        sb.s_first_ino = sb.s_first_ino + sizeof (inodo);
        sb.s_first_blo = sb.s_first_blo + sizeof (bloqueCarpeta);
        sb.s_bjpurfree = sb.s_bjpurfree + sizeof (journalie);
     */


    //Modificar el bitmap de inodos.
    bm_Inodos[0].status = '1';
    bmi_escribir(sb.s_bm_inode_start, n, partMontada.ruta, bm_Inodos);

    //Modificando el bitmap de bloques
    bm_Bloques[0].status = '1';
    bmb_escribir(sb.s_bm_block_start, n, partMontada.ruta, bm_Bloques);

    //Modificando los inodos
    Inodos[0].i_size = 0; //el tamaño del archivo
    strcpy(Inodos[0].i_atime, partMontada.part_time); //ultima fecha que se leyó el nodo sin modificarlo
    fechaActual(Inodos[0].i_ctime);
    fechaActual(Inodos[0].i_mtime);
    Inodos[0].i_type = '0'; //0 si es carpeta
    Inodos[0].i_block[0] = 0; //primer bloque libre
    int lol;
    for (lol = 1; lol < 12; lol++) {
        Inodos[0].i_block[lol] = -1;
    }
    //strcpy(Inodos[0].)

    inodos_escribir(sb.s_inode_start, n, partMontada.ruta, Inodos); //escribiendo lo inodos





    //Modificando los bloques
    content root;
    root.b_inodo = -1;
    strcpy(root.b_name, "/");
    content rootPadre;
    rootPadre.b_inodo = 0;
    strcpy(rootPadre.b_name, "");

    bloqueCarpeta carpeta;
    carpeta.b_content[0] = rootPadre;
    carpeta.b_content[1] = root;

    carpeta.b_content[2].b_inodo = -1; //no apuntan a nada
    carpeta.b_content[3].b_inodo = -1; //no apuntan a nada

    //en los otros dos van los hijos de root
    blqcarp_escribir(sb.s_block_start, 0, partMontada.ruta, carpeta);
    // printf("Blouqe->Lo estoy escribiendo en %i\n", sb.s_block_start);


    //
    journalie journ;
    fechaActual(journ.journal_fecha);
    strcpy(journ.journal_nombre, "/");
    journ.journal_tipo = '1';
    jour[0] = journ;

    jr_escribir(partMontada.part_inicio + partMontada.part_espacioEbr + sizeof (superBloque), n, partMontada.ruta, jour);


    superBloque sb2 = sb_retornar(id);



};

void insertarCarpeta(char id[sizeChar], char ruta[sizeChar]) {
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

void fechaActual(char fecha[sizeChar]) {
    time_t tiem = time(0);
    struct t*tlocal = localtime(&tiem);
    strftime(fecha, sizeChar, "%d/%m/%y %H:%M:%S", tlocal);
}

void getInfoDiskMount(char id[sizeChar]) {
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