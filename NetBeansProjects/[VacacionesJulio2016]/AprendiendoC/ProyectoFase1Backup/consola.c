/*
 *Aquí van los analizadores(el lexico y el sintactico)
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include "funciones.h"

#include "estructuras.h"
#include "Reporte.h"
#include "fMkdisk.h"
#include "fMount.h"
#include "fFdisk.h"
#include "fMkfs.h"
#include "fMkfile.h"
//del analizador

bool ocurrioError = false;
/**************************************************************
 * Prototipos                                            ****** 
 **************************************************************/
void analizadorLexico();
void analizadorSintactico();
void quitar_espacios(char *linea);
void toMinusc(char cadena[sizeChar]);
bool strcmpi(char string1[sizeChar], char string2[sizeChar]);

/**************************************************************
 *      La lista                                              * 
 **************************************************************/





//Lista
void InsertarAlFinal(Lista* lista, token* token);
Nodo* CrearNodo(token* token);
token* CrearToken(char* tipo, char* valor);
void push(char tokenDato[sizeChar], char valor[sizeChar]);
token pop(Lista* lista);


Lista* listaDeTokens;

Nodo* CrearNodo(token* token) {
    Nodo* nodo = (Nodo*) malloc(sizeof (Nodo));
    strncpy(nodo->token.tipo, token->tipo, sizeChar);
    strncpy(nodo->token.valor, token->valor, sizeChar);
    nodo->siguiente = NULL;
    return nodo;
}

token* CrearToken(char* tipo, char* valor) {

    token* nuevo_elemento = (token*) malloc(sizeof (token));

    /*
        nuevo_elemento->tipo = (char *) malloc(50 * sizeof (char));
        nuevo_elemento->valor = (char *) malloc(50 * sizeof (char));
     */
    //if ((nuevo_elemento->tipo = (char *) malloc(50 * sizeof (char))) == NULL) ;
    strcpy(nuevo_elemento->tipo, tipo);
    strcpy(nuevo_elemento->valor, valor);
    return nuevo_elemento;
}

void InsertarAlFinal(Lista* lista, token* token) {
    Nodo* nodo = CrearNodo(token);
    if (lista->cabeza == NULL) {
        //Lista vacia
        lista->cabeza = nodo;
    } else {
        //Lista llena
        Nodo* puntero = lista->cabeza;
        while (puntero->siguiente) {
            puntero = puntero->siguiente;
        }
        puntero->siguiente = nodo;
    }
}

token pop(Lista* lista) {
    if (lista->cabeza == NULL) {
        //puts(".......");
    } else {
        Nodo* eliminado = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente;
        return eliminado->token;
        //DestruirNodo(eliminado);
    }
}

void push(char tokenDato[sizeChar], char valor[sizeChar]) {
    token *tok = CrearToken(tokenDato, valor);
    InsertarAlFinal(listaDeTokens, tok);
}

void imprimir(Lista* lista) {
    Nodo* puntero = lista->cabeza;
    puts("====================IMPRIMIENDO==============");
    while (puntero) {

        printf("Tipo-> %s|Valor-> %s\n", puntero->token.tipo, puntero->token.valor);
        puntero = puntero->siguiente;
    }

}

/**************************************************************
 **ANALIZADOR LEXICO                                         ** 
 **************************************************************/
//Lexico
bool esElUltimo(int i);

void q0(int i);
void q1(int i, char *uniendo, int indice);
void q2(int i, char *uniendo, int indice);
void q3(int i, char *uniendo, int indice);
void q4(int i, char *uniendo, int indice);
void q5(int i, char *uniendo, int indice);
void q11(int i, char *uniendo, int indice);
void q12(int i, char *uniendo, int indice);

int LONG_MAX_LINEA = 200;
char linea[200];

void consola() {
    //printf("saludar");
    /*
        char  str1[sizeChar]= "HoLA";
        char str2[sizeChar]="hola";
        if (strcmpi(str2, str1)) {
            puts("Iguales");

        }else{
            puts("diferentes");
        }
     */

    /*
        printf("el restultado es %i\n",num);
     */
    ocurrioError = false;
    analizadorLexico();

    analizadorSintactico();
}

void analizadorLexico() {

    printf(">>>");

    /*Capturando los caracteres*/
    if (fgets(linea, LONG_MAX_LINEA, stdin) == NULL)
        return EXIT_FAILURE;

    listaDeTokens = (Lista*) malloc(sizeof (Lista));
    listaDeTokens->cabeza = NULL;
    q0(0);
    /*
        imprimir(listaDeTokens);
     */
    /*
        token tokn=pop(listaDeTokens);
        imprimir(listaDeTokens);
        printf("Tipo = %s",pop(listaDeTokens).tipo);
        imprimir(listaDeTokens);
     */

}

/*
 * Estados
 */
void q0(int i) {//estado inicial

    char uniendo[sizeChar] = ""; //
    int indice = 0;

    char letra = linea[i];
    if (!(esElUltimo(i))) {
        if (isalpha(letra)) {//vienen letras
            uniendo[indice] = letra;
            q2(i + 1, uniendo, indice + 1);
        } else if (isalnum(letra)) {//Si viene un nuemero
            uniendo[indice] = letra;
            q4(i + 1, uniendo, indice + 1);
        } else if ('-' == letra) {//Si viene un negativo
            uniendo[indice] = letra;
            q1(i + 1, uniendo, indice + 1);
        } else {
            switch (letra) {
                case '=':
                    push("igual", "=");
                    /*
                                        puts("Viene un igual");
                     */
                    if (!esElUltimo(i))
                        q0(i + 1);
                    break;
                case '$':
                    push("dolar", "$");
                    /*
                                        puts("Viene un dolar");
                     */
                    if (!esElUltimo(i))
                        q0(i + 1);
                    break;
                case '>':
                    push("mayor", ">");
                    /*
                                        puts("viene un mayor");
                     */
                    if (!esElUltimo(i))
                        q0(i + 1);
                    break;
                case '\"':

                    if (!esElUltimo(i))
                        //uniendo[indice] = letra;

                        q3(i + 1, uniendo, indice); //lo tengo que mandar al estado de las cadenas
                    break;
                case '”':
                    if (!esElUltimo(i))
                        uniendo[indice] = letra;
                    q3(i + 1, uniendo, indice); //lo tengo que mandar al estado de las cadenas
                    break;
                case '@':
                    push("arrroba", "@");
                    /*
                                        puts("viene una arroba");
                     */
                    if (!esElUltimo(i))
                        q0(i + 1); //lo tengo que mandar al estado de las cadenas
                    break;
                default:
                    /*
                                        puts("....");
                     */
                    //if(esElUltimo(i))
                    q0(i + 1);
                    break;
            }
        }
    }
}

void q2(int i, char uniendo[sizeChar], int indice) {//reconoce letras


    char letra = linea[i];
    if (esElUltimo(i)) {//aqui acepto las letras
        uniendo[indice] = letra;
        push("letras", uniendo);
        /*
                printf("Acepte una cadena de solo letras %s \n", uniendo);
         */
    } else {
        if (isalpha(letra)) {//viene una letra
            //char pe[sizeChar];
            uniendo[indice] = letra;
            q2(i + 1, uniendo, indice + 1);

        } else if (isalnum(letra)) {//viene un id
            uniendo[indice] = letra;
            q12(i + 1, uniendo, indice + 1); //es un id
        } else if (letra == '.') {//viene un nombre
            uniendo[indice] = letra;
            q11(i + 1, uniendo, indice + 1); //es un nombre de un disco
        } else if (letra == '_') {//viene un id
            uniendo[indice] = letra;
            q12(i + 1, uniendo, indice + 1); //es un nombre de un id
        } else {//aquí acpeto que es una letra
            push("letras", uniendo);
            /*
                        printf("Acepte una cadena de solo letras %s \n", uniendo);
             */
            q0(i); //lo mando al estado inicial
        }
    }
}

void q11(int i, char uniendo[sizeChar], int indice) {//disco.dsk
    char letra = linea[i];
    if (esElUltimo(i)) {//aqui acepto el numero
        push("disco", uniendo);
        /*
                puts("Acepte  el nombre de un disco");
         */
    } else {
        if (isalpha(letra)) {//indica que viene letra
            uniendo[indice] = letra;
            q11(i + 1, uniendo, indice + 1);
        } else {//aquí acpeto el disk
            push("disco", uniendo);
            /*
                        puts("Acepte  un el nombre de un disco");
             */
            q0(i); //lo mando al estado inicial
        }
    }
}

void q12(int i, char uniendo[sizeChar], int indice) {//id asd_, ad3849
    char letra = linea[i];
    if (esElUltimo(i)) {//aqui acepto el id 
        push("id", uniendo);
        /*
                puts("Acepte  un id");
         */
    } else {
        if (isalpha(letra)) {//indica que viene letra
            uniendo[indice] = letra;
            q12(i + 1, uniendo, indice + 1);
        } else if (isalnum(letra)) {//indica que viene un numero
            uniendo[indice] = letra;
            q12(i + 1, uniendo, indice + 1);
        } else if (letra == '_') {
            uniendo[indice] = letra;
            q12(i + 1, uniendo, indice + 1);
        } else if (letra == '.') {//viene un nombre de disco
            uniendo[indice] = letra;
            q11(i + 1, uniendo, indice + 1);
        } else {//aquí acpeto el id
            push("id", uniendo);
            /*
                        puts("Acepte  un id");
             */
            q0(i); //lo mando al estado inicial
        }
    }
}

void q3(int i, char uniendo[sizeChar], int indice) {//Cadenas
    char letra = linea[i];
    if (esElUltimo(i)) {//aqui acepto las letras

        /*
                uniendo[indice] = letra;
                push("letras", uniendo);
         */
        /*
                printf("Acepte una cadena de solo letras %s \n", uniendo);
         */
    } else if (letra == '\"') {//aqui termino es donde acepto la cadena
        push("cadena", uniendo);
        /*
                puts("Acepte una cadena con\"");
         */
        /*
                printf("Viene el caracter %c",linea[i]);
         */
        q0(i + 1);
    } else if (letra == '”') {
        push("cadena", uniendo);
        /*
                puts("Acepte una cadena");
         */
        q0(i + 1);
    } else {//aqui viene lo que sea
        if (!esElUltimo(i)) {
            uniendo[indice] = letra;
            q3(i + 1, uniendo, indice + 1);
        } else {
            push("cadena", uniendo);
        }


    }
}

void q1(int i, char uniendo[sizeChar], int indice) {//Numeros
    char letra = linea[i];
    if (esElUltimo(i)) {//aqui acepto el numero
        uniendo[indice] = letra;
        push("negativo", uniendo);
        /*
                puts("Acepte un numero ");
         */
    } else {
        if (isalnum(letra)) {//indica que viene otro numero
            uniendo[indice] = letra;
            q1(i + 1, uniendo, indice + 1);
        } else if (letra == '.') {//viene un decimal
            uniendo[indice] = letra;
            q5(i + 1, uniendo, indice + 1);
        } else {//aquí acpeto el numero
            push("negativo", uniendo);
            /*
                        puts("Acepte un numero");
             */
            q0(i); //lo mando al estado inicial
        }
    }

}

void q4(int i, char uniendo[sizeChar], int indice) {//Numeros
    char letra = linea[i];

    if (esElUltimo(i)) {//aqui acepto el numero
        uniendo[indice] = letra;
        push("numero", uniendo);
        /*
                puts("Acepte un numero ");
         */
    } else {
        if (isalnum(letra)) {//indica que viene otro numero
            uniendo[indice] = letra;
            q4(i + 1, uniendo, indice + 1);
        } else if (letra == '.') {//viene un decimal
            uniendo[indice] = letra;
            q5(i + 1, uniendo, indice + 1);
        } else {//aquí acpeto el numero
            push("numero", uniendo);
            /*
                        puts("Acepte un numero");
             */
            q0(i); //lo mando al estado inicial
        }
    }

}

void q5(int i, char uniendo[sizeChar], int indice) {//decimales
    char letra = linea[i];

    if (esElUltimo(i)) {//aqui acepto el decimal
        push("decimal", uniendo);
        /*
                puts("Acepte un decimal");
         */
    } else {
        if (isalnum(letra)) {//indica que viene otro numero
            uniendo[indice] = letra;
            q5(i + 1, uniendo, indice + 1);
        } else {//aqui acepto el decimal
            push("decimal", uniendo);
            /*
                        puts("Acepte un decimal");
             */
            q0(i); //lo mando al estado inicial
        }
    }
}

bool esElUltimo(int i) {
    char letra = linea[i + 1];
    bool retorno = false;
    if (letra == "\0") {
        retorno = true;
    } else if (10 == (int) letra) {
        retorno = true;
    } else {
        retorno = false;
    }
    //printf("retornando %i", retorno);
    return retorno;
}

/**************************************************************
 **ANALIZADOR SINTACTICO                                      ** 
 **************************************************************/
void S();
void MKD(int tama, char ruta[sizeChar], char disco[sizeChar]);
void MKD2(int tama, char ruta[sizeChar], char disco[sizeChar]);
void errorSintactico();
void errorSemantico(char mensaje[sizeChar * 3]);
void varMkDisk(int tama, char ruta[sizeChar], char disco[sizeChar]);

void inicializarVariablesFdisk();
void inicializarVariablesMnt();

void RKD();
void FKD();
void FKD2();
void MNT();
void MKFIL();
void MKDIR();

///MKDisk
int tamaS = 0;
char rutaS[sizeChar] = "";
char discoS[sizeChar] = "";

//rmkdisk
char rmkdisk_ruta[sizeChar] = "";


//fdisk
int fdisk_size = 0;
char fdisk_path[sizeChar] = "";
char fdisk_name[sizeChar] = "";
char fdisk_unit = ' ';
char fdisk_type = ' ';
char fdisk_fit[sizeChar] = "";
char fdisk_delete[sizeChar] = "";
int fdisk_add = 0;


//mount
char mnt_nombre[sizeChar] = "";
char mnt_ruta[sizeChar] = "";
char mnt_id[sizeChar] = "";


//mkfs
char mkfs_id[sizeChar] = "";
char mkfs_type[sizeChar] = "";
int mkfs_add = 0;
char mkfs_unit = ' ';

//mkfile
char mkfile_id[sizeChar] = "";
char mkfile_ruta[sizeChar] = "";
char mkfile_p = ' ';
int mkfile_size = 0;
char mkfile_cont[sizeChar] = "";

//mkdir
char mkdir_id[sizeChar] = "";
char mkdir_ruta[sizeChar] = "";
char mkdir_p = ' ';



void inicializarVariablesMkdir() {
    
    strcpy(mkdir_id, ""); 
    memset(mkdir_id, 0, sizeChar);
    strcpy(mkdir_ruta, "");  
    memset(mkdir_ruta, 0, sizeChar);
    mkdir_p=' ';
}

void inicializarVariablesMkfile() {
    
    strcpy(mkfile_id, "");
    memset(mkfile_id, 0, sizeChar);
    
    strcpy(mkfile_ruta, ""); 
    memset(mkfile_ruta, 0, sizeChar);
    
    strcpy(mkfile_cont, ""); 
    memset(mkfile_cont, 0, sizeChar);
    mkfile_p=' ';
    mkfile_size=0;
}

void inicializarVariablesFdisk() {
    fdisk_size = 0;
    strcpy(fdisk_path, ""); //fdisk_path = "";
    memset(fdisk_path, 0, sizeChar);
    
    strcpy(fdisk_name, ""); //fdisk_name = "";
    memset(fdisk_name, 0, sizeChar);
    
    fdisk_unit = ' ';
    fdisk_type = ' ';
    strcpy(fdisk_fit, ""); //fdisk_fit = "";
    memset(fdisk_fit, 0, sizeChar);
    
    strcpy(fdisk_delete, ""); //fdisk_delete = "";
    memset(fdisk_delete, 0, sizeChar);
    fdisk_add = 0;
}

void inicializarVariablesMkfs() {
    mkfs_add = 0;
    mkfs_unit = ' ';
    strcpy(mkfs_id, "");
     memset(mkfs_id, 0, sizeChar);
    strcpy(mkfs_type, "");
     memset(mkfs_type, 0, sizeChar);

}

void inicializarVariablesMnt() {
    strcpy(mnt_nombre, "");
    memset(mnt_nombre, 0, sizeChar);
    strcpy(mnt_ruta, "");
    memset(mnt_ruta, 0, sizeChar);
    strcpy(mnt_id, "");
    memset(mnt_id, 0, sizeChar);

}

void analizadorSintactico() {
    S();

}

void parea(char tipo[sizeChar], char viene[]) {


}

void S() {
    token tok = pop(listaDeTokens);
    if (strcmpi("letras", tok.tipo)) {
        if (strcmpi("mkdisk", tok.valor)) {
            printf("\t.......................Mkdisk.......................\n");

            int tama;
            char ruta[sizeChar];
            char disco[sizeChar];
            MKD(tama, ruta, disco);

            if (!ocurrioError) {

                mkdisk(tamaS, rutaS, discoS);

            }
        } else if (strcmpi("rmdisk", tok.valor)) {
            //puts(".......................Rmkdisk.......................");

            RKD();

            //printf("Ruta: %s\n", rmkdisk_ruta);
            rmkdisk(rmkdisk_ruta);
        } else if (strcmpi("fdisk", tok.valor)) {

            printf("\t.........................Fdisk........................\n");
            FKD();
            if (!ocurrioError) {

                fdisk(fdisk_size, fdisk_path, fdisk_name, fdisk_unit, fdisk_type, fdisk_fit, fdisk_delete, fdisk_add);

            }
            inicializarVariablesFdisk();
        } else if (strcmpi("mount", tok.valor)) {


            MNT();

            if (!ocurrioError) {
                int retorno = strncmp("", mnt_nombre, sizeChar);
                if (retorno == 0) {//solo viene mount
                    imprimirListaDeParticionesMontadas();
                } else {
                    printf("\t.............>[Montando Partición]<....................\n");

                    mountn(mnt_ruta, mnt_nombre);

                    printf("\t.......................................................\n");
                }
            }



            inicializarVariablesMnt();

        } else if (strcmpi("mkfs", tok.valor)) {
            inicializarVariablesMkfs();
            printf("\t....................>[Formatear]<..........................\n");
            MKFS();

            if (!ocurrioError) {
                mkfs(mkfs_id, mkfs_unit, mkfs_type, mkfs_add);
            }

            printf("\t.......................................................\n");

        } else if (strcmpi("mkfile", tok.valor)) {
            inicializarVariablesMkfile();
            printf("\t.................>[Creando archivos]<........................\n");
            MKFIL();

            if (!ocurrioError) {
                mkfile(mkfile_id,mkfile_ruta,mkfile_p,mkfile_size,mkfile_cont);
            }

            printf("\t..............................................................\n");

        }else if (strcmpi("mkdir", tok.valor)) {
            inicializarVariablesMkdir();
            printf("\t..................>[Creando carpeta]<..........................\n");
            MKDIR();

            if (!ocurrioError) {
                mkdir(mkdir_id,mkdir_ruta,mkdir_p);
            }

            printf("\t................................................................\n");

        }else if (strcmpi("exit", tok.valor)) {
            abort();
        } else if (strcmpi("reportembr", tok.valor)) {
            token tok = pop(listaDeTokens);
            //reporteMBR(tok.valor);
            reporteMBR(tok.valor);
        } else if (strcmpi("reporteebr", tok.valor)) {
            token tok = pop(listaDeTokens);
            //reporteMBR(tok.valor);
            reporteEBR(tok.valor);
        } else if (strcmpi("reporteext", tok.valor)) {
            token tok = pop(listaDeTokens);
            //reporteMBR(tok.valor);
            reporteEXT(tok.valor);
        } else {
            errorSintactico();
        }
        //////////agregar un salir
    } else {
        errorSintactico();
    }
}

void varMkDisk(int tama, char ruta[sizeChar], char disco[sizeChar]) {
    tamaS = tama;
    strcpy(rutaS, ruta);
}

/**************************************************************
 **No TERMINALES                                             ** 
 **************************************************************/
void MKDIR() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);

    if (strcmpi("id", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(mkdir_id, cad.valor);
            MKDIR();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(mkdir_id, cad.valor);
            MKDIR();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(mkdir_ruta, cad.valor);
            MKDIR();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("p", tok.valor)) {
        mkdir_p='p';
        MKDIR();
        return;
    }
}


void MKFIL() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);

    if (strcmpi("id", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(mkfile_id, cad.valor);
            MKFIL();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(mkfile_id, cad.valor);
            MKFIL();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(mkfile_ruta, cad.valor);
            MKFIL();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("p", tok.valor)) {
        mkfile_p='p';
        MKFIL();
        return;
    }else if (strcmpi("size", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            if (numero > 0) {
                mkfile_size = numero;
                MKFIL();
                return;
            } else {
                errorSemantico("Size debe de ser mayor a cero");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("cont", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(mkfile_cont, cad.valor);
            MKFIL();
            return;
        } else {
            errorSintactico();
        }
    }
}

void MKFS() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);

    if (strcmpi("id", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(mkfs_id, cad.valor);
            MKFS();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(mkfs_id, cad.valor);
            MKFS();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("letras", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(mkfs_id, cad.valor);
            MKFS();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("type", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {
            strcpy(mkfs_type, cad.valor);
            MKFS();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("add", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("negativo", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            mkfs_add = numero;
            MKFS();
            return;
        } else if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            mkfs_add = numero;
            MKFS();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("unit", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {

            char aux = cad.valor[0]; //el primer elemento
            aux = tolower(aux);
            //strncmp(string1, string2, sizeChar);
            if (aux == 'b') {
                mkfs_unit = aux;
                MKFS();
                return;
            } else if (aux == 'k') {
                mkfs_unit = aux;
                MKFS();
                return;
            } else if (aux == 'm') {
                mkfs_unit = aux;
                MKFS();
                return;
            } else {
                errorSemantico("Ingrese una unidad correcta");
            }
        } else {
            errorSintactico();
        }
    }
}

void MNT() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);
    if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(mnt_ruta, cad.valor);
            MNT();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("name", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(mnt_nombre, cad.valor);
            MNT();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(mnt_nombre, cad.valor);
            MNT();
            return;
        } else {
            errorSintactico();
        }
    }
}

void FKD() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);
    if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(fdisk_path, cad.valor);
            FKD2();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("size", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            if (numero > 0) {

                fdisk_size = numero;
                FKD2();
                return;
            } else {
                errorSemantico("Size debe de ser mayor a cero");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("name", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD2();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD2();
            return;
        } else if (strcmpi("cadena", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD2();
            return;
        } else {
            errorSintactico();
        }

    } else if (strcmpi("unit", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {

            char aux = cad.valor[0]; //el primer elemento
            aux = tolower(aux);
            //strncmp(string1, string2, sizeChar);
            if (aux == 'b') {
                fdisk_unit = aux;
                FKD2();
                return;
            } else if (aux == 'k') {
                fdisk_unit = aux;
                FKD2();
                return;
            } else if (aux == 'm') {
                fdisk_unit = aux;
                FKD2();
                return;
            } else {
                errorSemantico("Ingrese una unidad correcta");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("type", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {
            char aux = cad.valor[0]; //el primer elemento
            aux = tolower(aux);
            //strncmp(string1, string2, sizeChar);
            if (aux == 'p') {
                fdisk_type = aux;
                FKD2();
                return;
            } else if (aux == 'e') {
                fdisk_type = aux;
                FKD2();
                return;
            } else if (aux == 'l') {
                fdisk_type = aux;
                FKD2();
                return;
            } else {
                errorSemantico("Ingrese un tipo de partición correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("fit", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {

            if (strcmpi("bf", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD2();
                return;
            } else if (strcmpi("ff", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD2();
                return;
            } else if (strcmpi("wf", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD2();
                return;
            } else {
                errorSemantico("Ingrese un ajuste correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("delete", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {
            if (strcmpi("fast", cad.valor)) {
                strcpy(fdisk_delete, cad.valor);
                FKD2();
                return;
            } else if (strcmpi("full", cad.valor)) {
                strcpy(fdisk_delete, cad.valor);
                FKD2();
                return;
            } else {
                errorSemantico("Ingrese un parámetro correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("add", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("negativo", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            fdisk_add = numero;
            FKD2();
            return;
        } else if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            fdisk_add = numero;
            FKD2();
            return;
        } else {
            errorSintactico();
        }
    } else {
        //aquí se acepta
    }
}

void FKD2() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);
    if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(fdisk_path, cad.valor);
            FKD();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("size", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            if (numero > 0) {

                fdisk_size = numero;
                FKD();
                return;
            } else {
                errorSemantico("Size debe de ser mayor a cero");
            }
            strcpy(rmkdisk_ruta, cad.valor);
        } else {
            errorSintactico();
        }
    } else if (strcmpi("name", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("id", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD();
            return;
        } else if (strcmpi("letras", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD();
            return;
        } else if (strcmpi("cadena", cad.tipo)) {
            strcpy(fdisk_name, cad.valor);
            FKD();
            return;
        } else {
            errorSintactico();
        }
    } else if (strcmpi("unit", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {

            char aux = cad.valor[0]; //el primer elemento
            aux = tolower(aux);
            //strncmp(string1, string2, sizeChar);
            if (aux == 'b') {
                fdisk_unit = aux;
                FKD();
                return;
            } else if (aux == 'k') {
                fdisk_unit = aux;
                FKD();
                return;
            } else if (aux == 'm') {
                fdisk_unit = aux;
                FKD();
                return;
            } else {
                errorSemantico("Ingrese una unidad correcta");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("type", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {
            char aux = cad.valor[0]; //el primer elemento
            aux = tolower(aux);
            //strncmp(string1, string2, sizeChar);
            if (aux == 'p') {
                fdisk_type = aux;
                FKD();
                return;
            } else if (aux == 'e') {
                fdisk_type = aux;
                FKD();
                return;
            } else if (aux == 'l') {
                fdisk_type = aux;
                FKD();
                return;
            } else {
                errorSemantico("Ingrese un tipo de partición correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("fit", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {

            if (strcmpi("bf", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD();
                return;
            } else if (strcmpi("ff", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD();
                return;
            } else if (strcmpi("wf", cad.valor)) {
                strcpy(fdisk_fit, cad.valor);
                FKD();
                return;
            } else {
                errorSemantico("Ingrese un ajuste correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("delete", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("letras", cad.tipo)) {
            if (strcmpi("fast", cad.valor)) {
                strcpy(fdisk_delete, cad.valor);
                FKD();
                return;
            } else if (strcmpi("full", cad.valor)) {
                strcpy(fdisk_delete, cad.valor);
                FKD();
                return;
            } else {
                errorSemantico("Ingrese un parámetro correcto");
            }
        } else {
            errorSintactico();
        }
    } else if (strcmpi("add", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("negativo", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            fdisk_add = numero;
            FKD();
            return;
        } else if (strcmpi("numero", cad.tipo)) {
            int numero;
            sscanf(cad.valor, "%i", &numero);
            fdisk_add = numero;
            FKD();
            return;
        } else {
            errorSintactico();
        }
    } else {
        //aquí se acepta
    }
}

void MKD(int tama, char ruta[sizeChar], char disco[sizeChar]) {
    token to = pop(listaDeTokens);
    if (strcmpi("dolar", to.tipo)) {
        token tok = pop(listaDeTokens);
        if (strcmpi("size", tok.valor)) {
            token tok1 = pop(listaDeTokens);
            if (strcmpi("igual", tok1.tipo)) {
                token tok2 = pop(listaDeTokens);
                if (strcmpi("mayor", tok2.tipo)) {
                    token tok3 = pop(listaDeTokens);
                    if (strcmpi("numero", tok3.tipo)) {//hay que validar que sea multiplo de 8 //mayor a cero
                        int numero;
                        sscanf(tok3.valor, "%i", &numero);

                        if ((numero % 8) == 0) {//si es multiplo de 8
                            //printf("El numero aceptado %i\n", numero);
                            tama = numero;
                            tamaS = numero;
                            MKD2(tama, ruta, disco);
                            return;

                        } else {
                            errorSemantico("No es multiplo de 8");
                        }

                    } else {
                        errorSintactico();
                    }
                } else {
                    errorSintactico();
                }
            } else {
                errorSintactico();
            }

        } else if (strcmpi("path", tok.valor)) {
            pop(listaDeTokens); //saco el igual
            pop(listaDeTokens); //saco el mayor
            token tok2 = pop(listaDeTokens);
            if (strcmpi("cadena", tok2.tipo)) {
                //printf("La cadena es  %s\n", tok2.valor);
                ruta = tok2.valor;
                strcpy(rutaS, tok2.valor);
                //rutaS=tok2.valor;

                MKD2(tama, ruta, disco);
                return;
            } else {
                errorSintactico();
            }
        } else if (strcmpi("name", tok.valor)) {
            pop(listaDeTokens); //saco el igual
            pop(listaDeTokens); //saco el mayor
            token tok2 = pop(listaDeTokens);
            if (strcmpi("disco", tok2.tipo)) {
                //printf("El disco es  %s\n", tok2.valor);
                disco = tok2.valor;
                //discoS=tok2.valor;
                strcpy(discoS, tok2.valor);
                MKD2(tama, ruta, disco); //aqui, despues me paso a la linea 550
                return;
            } else {
                errorSintactico();
            }
        } else {
            errorSintactico();
        }
    } else {
        //MKD2(tama,ruta,disco);   
    }
}

void MKD2(int tama, char ruta[sizeChar], char disco[sizeChar]) {
    token to = pop(listaDeTokens);
    if (strcmpi("dolar", to.tipo)) {
        token tok = pop(listaDeTokens);
        if (strcmpi("size", tok.valor)) {
            token tok1 = pop(listaDeTokens);
            if (strcmpi("igual", tok1.tipo)) {
                token tok2 = pop(listaDeTokens);
                if (strcmpi("mayor", tok2.tipo)) {
                    token tok3 = pop(listaDeTokens);
                    if (strcmpi("numero", tok3.tipo)) {//hay que validar que sea multiplo de 8 //mayor a cero
                        int numero;
                        sscanf(tok3.valor, "%i", &numero);

                        if ((numero % 8) == 0) {//si es multiplo de 8
                            //printf("El numero aceptado %i\n", numero);
                            tama = numero;
                            tamaS = numero;
                            MKD(tama, ruta, disco);
                            return;
                        } else {
                            errorSemantico("No es multiplo de 8");
                        }

                    } else {
                        errorSintactico();
                    }
                } else {
                    errorSintactico();
                }
            } else {
                errorSintactico();
            }

        } else if (strcmpi("path", tok.valor)) {
            pop(listaDeTokens); //saco el igual
            pop(listaDeTokens); //saco el mayor
            token tok2 = pop(listaDeTokens);
            if (strcmpi("cadena", tok2.tipo)) {
                //printf("La cadena es  %s\n", tok2.valor);
                ruta = tok2.valor;
                strcpy(rutaS, tok2.valor);
                //rutaS=tok2.valor;
                MKD(tama, ruta, disco);
                return;
            } else {
                errorSintactico();
            }
        } else if (strcmpi("name", tok.valor)) {
            pop(listaDeTokens); //saco el igual
            pop(listaDeTokens); //saco el mayor
            token tok2 = pop(listaDeTokens);
            if (strcmpi("disco", tok2.tipo)) {
                //printf("El disco es  %s\n", tok2.valor);
                disco = tok2.valor;
                //discoS=tok2.valor;
                strcpy(discoS, tok2.valor);
                MKD(tama, ruta, disco);
                return;
            } else {
                errorSintactico();
            }
        } else {
            errorSintactico();
        }
    } else {
        //aquí acepto la cadena
        //puts("Cadena aceptada");
    }

}

void RKD() {
    pop(listaDeTokens); //$
    token tok = pop(listaDeTokens); //path
    toMinusc(tok.valor);
    if (strcmpi("path", tok.valor)) {
        pop(listaDeTokens); //=
        pop(listaDeTokens); //>
        token cad = pop(listaDeTokens);
        if (strcmpi("cadena", cad.tipo)) {
            strcpy(rmkdisk_ruta, cad.valor);

        } else {
            errorSintactico();
        }
    } else {
        errorSintactico();
    }
}

void errorSintactico() {
    puts("\t[Error Sintactico]");
    ocurrioError = true;
}

void errorSemantico(char mensaje[sizeChar * 3]) {
    printf("\t[Error Semantico]%s\n", mensaje);
    ocurrioError = true;
}
//hola

/**************************************************************
 **FUNCIONES CADENA                                           ** 
 **************************************************************/


void quitar_espacios(char *linea) {
    int i;
    int j;

    for (i = j = 0; linea[i] != '\0'; i++)
        if (linea[i] != ' ')
            linea[j++] = linea[i];
    linea[j] = '\0';
}

bool strcmpi(char string1[sizeChar], char string2[sizeChar]) {
    int retorno = 1;
    toMinusc(string2);
    //printf("een minusculas %s\n",str1);
    retorno = strncmp(string1, string2, sizeChar);
    if (retorno == 0) {
        return true;
    } else {
        return false;
    }
}

void toMinusc(char cadena[50]) {

    int i;


    /* Pasamos todos los caracteres a minuscula con tolower */
    for (i = 0; cadena[i] != '\0'; i++)
        cadena[i] = tolower(cadena[i]);

    /*
        printf("Linea en minusculas: %s", linea);
        printf("Numero de espacios: %i\n", espacios);
     */
}
