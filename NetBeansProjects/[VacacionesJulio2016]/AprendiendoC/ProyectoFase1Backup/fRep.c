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
#include "Mbr.h"


///void reporteEbr(char id[sizeChar]) 

void reporteEbr(char id[sizeChar]);
void reporteDisk(char id[sizeChar]);
void superBloq(char id[sizeChar]);
void reBmInodes(char id[sizeChar]);
void reBmBloques(char id[sizeChar]);
void arbol(char id[sizeChar]);

void exec(char ruta[sizeChar]) {
    //printf("la ruta es : %s\n", ruta);

    FILE *ptr_file;
    char buf[200];

    ptr_file = fopen(ruta, "r");
    if (!ptr_file)
        return 1;

    while (fgets(buf, 200, ptr_file) != NULL) {
        printf("%s", buf);
        if (buf[0] != '/') {
            getchar();
            strcat(buf, "##");
            consola2(buf);
        }

    }

    fclose(ptr_file);
    return 0;

}

void rep(char id[sizeChar], char ruta[sizeChar], char name[sizeChar]) {
    printf("id=%s\n", id);
    printf("ruta= %s\n", ruta);
    printf("nombre= %s\n", name);

    if (strcmpi("mbr", name)) {//si son iguales
        reporteMbr(id);
        reporteEbr(id);
    } else if (strcmpi("disk", name)) {
        reporteDisk(id);
    } else if (strcmpi("block", name)) {
        puts("Blouqe");
    } else if (strcmpi("journalie", name)) {
        puts("");
    } else if (strcmpi("sb", name)) {
        superBloq(id);
    } else if (strcmpi("bm_block", name)) {
        reBmBloques(id); //este es el que falta
    } else if (strcmpi("bm_inode", name)) {
        reBmInodes(id);
    } else if (strcmpi("tree", name)) {
        arbol(id);
    }

}

void arbol(char id[sizeChar]) {
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
    FILE *fe;

    if ((fe = fopen("/home/joseph/Documentos/arbol.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fprintf(fe, "digraph demo { node [shape=record] ;rankdir=UD;\n"); //encabezado

        int p;
        for (p = 0; p < n; p++) {
            if (bm_Inodos[p].status == '1') {//dibujando el inodo
                inodo aux = Inodos[p];
                fprintf(fe, "{");
                fprintf(fe, "I%i[label=\"{", aux.i_uid);
                fprintf(fe, "I%i|tamaño=%i", aux.i_uid, aux.i_size);
                //fprintf(fe, "|FechaMontaje=%s", aux.i_ctime);
                //fprintf(fe, "|Tipo=%c", aux.i_type);
                fprintf(fe, "|{<N0>|<N1>|<N2>|<N3>|<N4>|<N5>|<N6>|<N7>|<N8>|<N9>|<N10>|<N11>|<N12>}");
                fprintf(fe, "}\"];");
                fprintf(fe, "};");
                int yu;
                for (yu = 0; yu < 12; yu++) {
                    if (aux.i_block[yu] != -1) {//hay que hacer su relación
                        fprintf(fe, "I%i:N%i->B%i;", aux.i_uid, yu, aux.i_block[yu]);
                    }
                }

            }
        }
        int q = 0;
        for (q = 0; q < n * 3; q++) {
            if (bm_Bloques[q].status == '1') {//dibujando el bloque; 
                fprintf(fe, "{");
                fprintf(fe, "B%i[label=\"{", q);
                fprintf(fe, "|B%i", q);
                fprintf(fe, "}\"];");
                fprintf(fe, "};");
            }
        }
        fprintf(fe, "}");
        fclose(fe);
        system("dot -Tpng -o /home/joseph/Documentos/arbol.png /home/joseph/Documentos/arbol.dot");
    }



}

void reBmInodes(char id[sizeChar]) {
    superBloque sb = sb_retornar(id);

    mnt_nodo mountNodo = retornarNodoMount(id);
    int n = sb.s_inodes_count;


    FILE *f;
    if ((f = fopen("/home/joseph/Documentos/mb_inodo.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fprintf(f, "digraph demo { node [shape=record] ;rankdir=UD;{rank=same;I1[label=\"{ \n"); //encabezado

        bmInodo aule[n];
        bmi_leer(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, aule);
        int div = n / 4;
        int p;
        fprintf(f, "BitMapInodos");
        for (p = 0; p < n; p++) {
            fprintf(f, "|%c", aule[p].status);
        }
        printf("\n");
        fprintf(f, "}\"];}}");
        fclose(f);
        system("dot -Tpng -o /home/joseph/Documentos/mb_inodo.png /home/joseph/Documentos/mb_inodo.dot");

    }


}

void reBmBloques(char id[sizeChar]) {
    superBloque sb = sb_retornar(id);

    mnt_nodo mountNodo = retornarNodoMount(id);
    int n = sb.s_inodes_count;


    FILE *f;
    if ((f = fopen("/home/joseph/Documentos/mb_bloque.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fprintf(f, "digraph demo { node [shape=record] ;rankdir=UD;{rank=same;I1[label=\"{ \n"); //encabezado

        bmBloque aule[n];
        bmb_leer(sb.s_bm_inode_start, n, mountNodo.mnt_ruta, aule);
        int div = n / 4;
        int p;
        fprintf(f, "BitMapBlouqe");
        for (p = 0; p < 3 * n; p++) {
            fprintf(f, "|%c", aule[p].status);
        }
        printf("\n");
        fprintf(f, "}\"];}}");
        fclose(f);
        system("dot -Tpng -o /home/joseph/Documentos/mb_bloque.png /home/joseph/Documentos/mb_bloque.dot");

    }


}

void superBloq(char id[sizeChar]) {
    FILE *f;
    if ((f = fopen("/home/joseph/Documentos/sb.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fprintf(f, "digraph demo { node [shape=record] ;rankdir=UD;{rank=same;I1[label=\"{ \n"); //encabezado
        superBloque sb = sb_retornar(id);

        fprintf(f, "total de inodos: %i|", sb.s_inodes_count); //Guarda el número total de inodos
        fprintf(f, "número total de bloques:%i|", sb. s_blocks_count); //Guarda el número total de bloques
        fprintf(f, "número de bloques libres:%i|", sb.s_free_blocks_counts); //Contiene el número de bloques libres
        fprintf(f, "número de inodos libres:%i|", sb. s_free_inodes_count); //Contiene el número de inodos libres
        fprintf(f, "fecha en el que el sistema fue montado:%s|", sb. s_mtime); //Última fecha en el que el sistema fue montado
        fprintf(f, "fecha en que el sistema fue desmontado:%s|", sb.s_unmtime); //Última fecha en que el sistema fue desmontado


        fprintf(f, "Tamaño del inodo:%i|", sb.s_inode_size); //Tamaño del inodo
        fprintf(f, "Tamaño del bloque:%i|", sb.s_block_size); //Tamaño del bloque
        fprintf(f, "Primer inodo libre:%i|", sb. s_first_ino); //Primer inodo libre
        fprintf(f, "Primer bloque libre:%i|", sb.s_first_blo); //Primer bloque libre
        fprintf(f, "Inicio del bitmap de inodos:%|i", sb.s_bm_inode_start); //Guardará el inicio del bitmap de inodos
        fprintf(f, "inicio del bitmap de bloques:%i|", sb. s_bm_block_start); //Guardará el inicio del bitmap de bloques
        fprintf(f, "inicio de la tabla de inodos:%i|", sb.s_inode_start); //Guardará el inicio de la tabla de inodos
        fprintf(f, "inico de la tabla de bloques:%i|", sb.s_block_start); //Guardará el inico de la tabla de bloques
        fprintf(f, "Primer Journalie:%i", sb.s_bjpurfree); //Primer Journalie libre
        fprintf(f, "}\"];}}");
        fclose(f);
        system("dot -Tpng -o /home/joseph/Documentos/sb.png /home/joseph/Documentos/sb.dot");

    }


}

void reporteMbr(char id[sizeChar]) {

    FILE *f;
    if ((f = fopen("/home/joseph/Documentos/mbr.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {

        fprintf(f, "digraph demo { node [shape=record] ;rankdir=UD;{rank=same;I1[label=\"{ \n"); //encabezado




        particionMontada partMontada = devolverParticionMontada(id);

        char rut[sizeChar];
        strcpy(rut, partMontada.ruta);
        bloqueMBR mbr = leerMBR(rut);
        fprintf(f, "Tamaño:%i|", mbr.mbr_tamano);
        fprintf(f, "Fecha :%s|", mbr.mbr_fecha_creacion);
        fprintf(f, "Id    :%i|", mbr.mbr_disk_asignature);

        partition particiones[4];
        particiones[0] = mbr.mbr_partition_1;
        particiones[1] = mbr.mbr_partition_2;
        particiones[2] = mbr.mbr_partition_3;
        particiones[3] = mbr.mbr_partition_4;
        int i;
        for (i = 0; i < 4; i++) {
            fprintf(f, "disk_asignature:    :%i|", mbr.mbr_disk_asignature);
            fprintf(f, "Nombre %i:%s|", i + 1, particiones[i].part_name);
            fprintf(f, "Status:%c|", particiones[i].part_status);
            fprintf(f, "Tipo:%c|", particiones[i].part_type);
            fprintf(f, "Ajuste:%c|", particiones[i].part_fit);
            fprintf(f, "Inicio:%i|", particiones[i].part_start);
            fprintf(f, "Tamaño:%i|", particiones[i].part_size);
            fprintf(f, "Final:%i|", particiones[i].part_start + particiones[i].part_size);
        }
        fprintf(f, "}\"];}}");
        fclose(f);
        system("dot -Tpng -o /home/joseph/Documentos/mbr.png /home/joseph/Documentos/mbr.dot");
    }
}

void reporteDisk(char id[sizeChar]) {



    FILE *fe;
    if ((fe = fopen("/home/joseph/Documentos/disk.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        //fprintf(fe, "digraph demo { node [shape=record] ;rankdir=UD; \n"); //encabezado
        fprintf(fe, "digraph demo { node [shape=record] ;rankdir=UD;{rank=same;I1[label=\"{ \n"); //encabezado


        particionMontada partMontada = devolverParticionMontada(id);

        char rut[sizeChar];
        strcpy(rut, partMontada.ruta);
        bloqueMBR mbr = leerMBR(rut);


        //buscando la posicion de la partición logica
        partition particiones[4];
        particiones[0] = mbr.mbr_partition_1;
        particiones[1] = mbr.mbr_partition_2;
        particiones[2] = mbr.mbr_partition_3;
        particiones[3] = mbr.mbr_partition_4;

        int i;
        for (i = 0; i < 4; i++) {
            fprintf(fe, "|");
            fprintf(fe, "disk_asignature:%i", mbr.mbr_disk_asignature);
            fprintf(fe, "Nombre %i:%s", i + 1, particiones[i].part_name);
            fprintf(fe, "Status:%c", particiones[i].part_status);
            fprintf(fe, "Tipo:%c", particiones[i].part_type);
            
            fprintf(fe, "Ajuste:%c", particiones[i].part_fit);
            fprintf(fe, "Inicio:%i", particiones[i].part_start);
            fprintf(fe, "Tamaño:%i", particiones[i].part_size);
            fprintf(fe, "Final:%i", particiones[i].part_start + particiones[i].part_size);
            fprintf(fe, "");
        }
        fprintf(fe, "}\"];};}");
        bool hayExtendida = false;
        int o;
        for (i = 0; o < 4; o++) {
            if (particiones[o].part_type == 'e') {
                hayExtendida = true;
                break;
            }
        }

        int inicio = 0;
        int tamano = 0;
        int espacioDisponible = 0;
        if (hayExtendida == true) {

            inicio = particiones[i].part_start;
            tamano = particiones[i].part_size;
            espacioDisponible = inicio + tamano;
            FILE *f;
            if ((f = fopen(partMontada.ruta, "r+b")) == NULL) {
                printf("\t[ERROR]error al abrir el disco!\n");
            } else {



                bloqueEBR ebr;
                // printf("\tinicio = %i\n", inicio);
                fseek(f, inicio, SEEK_SET);
                fread(&ebr, sizeof (bloqueEBR), 1, f);
                if (ebr.part_fit == 'b' || ebr.part_fit == 'f' || ebr.part_fit == 'w') {
                    //Si se encontró el primer ERB
                    /*
                                        fprintf(fe, "{rank=same;I%i[label=\"{\n", ebr.part_start);
                                        fprintf(fe, "Nombre:%s|", ebr.part_name);
                                        fprintf(fe, "Status:%c|", ebr.part_status);
                                        fprintf(fe, "Ajuste:%c|", ebr. part_fit);
                                        fprintf(fe, "Inicio:%i|", ebr.part_start);
                                        fprintf(fe, "Tamaño:%i|", ebr.part_size);
                                        fprintf(fe, "PosSiguiente:%i |", ebr.part_next);
                                        fprintf(fe, "}\"];};");
                                        fprintf(fe, "I%i->I%i;\n", ebr.part_start, ebr.part_next);
                     */


                    int siguiente = ebr.part_next;
                    while (true) {
                        bloqueEBR aux_ebr;
                        fseek(f, siguiente, SEEK_SET);
                        fread(&aux_ebr, sizeof (bloqueEBR), 1, f);
                        if (aux_ebr.part_fit == 'b' || aux_ebr.part_fit == 'f' || aux_ebr.part_fit == 'w') {


                            /*
                                                        fprintf(fe, "{rank=same;I%i[label=\"{\n", aux_ebr.part_start);
                                                        fprintf(fe, "Nombre:%s|", aux_ebr.part_name);
                                                        fprintf(fe, "Status:%c|", aux_ebr.part_status);
                                                        fprintf(fe, "Ajuste:%c|", aux_ebr. part_fit);
                                                        fprintf(fe, "Inicio:%i|", aux_ebr.part_start);
                                                        fprintf(fe, "Tamaño:%i|", aux_ebr.part_size);
                                                        fprintf(fe, "PosSiguiente:%i |", aux_ebr.part_next);
                                                        fprintf(fe, "}\"];};");
                                                        fprintf(fe, "I%i->I%i;\n", aux_ebr.part_start, aux_ebr.part_next);
                             */

                            siguiente = aux_ebr.part_next;
                            //es que si hay siguiente
                        } else {
                            break;
                        }
                    }
                } else {

                }
                fclose(f);
            }
        } else {
            puts("/t[ERROR]No se ha creado una partición extendida");
        }

        fclose(fe);
        system("dot -Tpng -o /home/joseph/Documentos/disk.png /home/joseph/Documentos/disk.dot");
    }

    //system("dot -Tpng -o ebr.png /home/joseph/Documentos/ebr.dot");

}

void reporteEbr(char id[sizeChar]) {



    FILE *fe;
    if ((fe = fopen("/home/joseph/Documentos/ebr.dot", "w+")) == NULL) {
        printf("[ERROR]error al abrir el disco!\n");
    } else {
        fprintf(fe, "digraph demo { node [shape=record] ;rankdir=UD; \n"); //encabezado


        particionMontada partMontada = devolverParticionMontada(id);

        char rut[sizeChar];
        strcpy(rut, partMontada.ruta);
        bloqueMBR mbr = leerMBR(rut);


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

            inicio = particiones[i].part_start;
            tamano = particiones[i].part_size;
            espacioDisponible = inicio + tamano;
            FILE *f;
            if ((f = fopen(partMontada.ruta, "r+b")) == NULL) {
                printf("\t[ERROR]error al abrir el disco!\n");
            } else {



                bloqueEBR ebr;
                // printf("\tinicio = %i\n", inicio);
                fseek(f, inicio, SEEK_SET);
                fread(&ebr, sizeof (bloqueEBR), 1, f);
                if (ebr.part_fit == 'b' || ebr.part_fit == 'f' || ebr.part_fit == 'w') {
                    //Si se encontró el primer ERB
                    fprintf(fe, "{rank=same;I%i[label=\"{\n", ebr.part_start);
                    fprintf(fe, "Nombre:%s|", ebr.part_name);
                    fprintf(fe, "Status:%c|", ebr.part_status);
                    fprintf(fe, "Ajuste:%c|", ebr. part_fit);
                    fprintf(fe, "Inicio:%i|", ebr.part_start);
                    fprintf(fe, "Tamaño:%i|", ebr.part_size);
                    fprintf(fe, "PosSiguiente:%i |", ebr.part_next);
                    fprintf(fe, "}\"];};");
                    fprintf(fe, "I%i->I%i;\n", ebr.part_start, ebr.part_next);


                    int siguiente = ebr.part_next;
                    while (true) {
                        bloqueEBR aux_ebr;
                        fseek(f, siguiente, SEEK_SET);
                        fread(&aux_ebr, sizeof (bloqueEBR), 1, f);
                        if (aux_ebr.part_fit == 'b' || aux_ebr.part_fit == 'f' || aux_ebr.part_fit == 'w') {


                            fprintf(fe, "{rank=same;I%i[label=\"{\n", aux_ebr.part_start);
                            fprintf(fe, "Nombre:%s|", aux_ebr.part_name);
                            fprintf(fe, "Status:%c|", aux_ebr.part_status);
                            fprintf(fe, "Ajuste:%c|", aux_ebr. part_fit);
                            fprintf(fe, "Inicio:%i|", aux_ebr.part_start);
                            fprintf(fe, "Tamaño:%i|", aux_ebr.part_size);
                            fprintf(fe, "PosSiguiente:%i |", aux_ebr.part_next);
                            fprintf(fe, "}\"];};");
                            fprintf(fe, "I%i->I%i;\n", aux_ebr.part_start, aux_ebr.part_next);

                            siguiente = aux_ebr.part_next;
                            //es que si hay siguiente
                        } else {
                            break;
                        }
                    }
                } else {

                }
                fclose(f);
            }
        } else {
            puts("/t[ERROR]No se ha creado una partición extendida");
        }
        fprintf(fe, "}");
        fclose(fe);
        system("dot -Tpng -o /home/joseph/Documentos/ebr.png /home/joseph/Documentos/ebr.dot");
    }

    //system("dot -Tpng -o ebr.png /home/joseph/Documentos/ebr.dot");

}