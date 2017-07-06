/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: joseph
 *
 * Created on 6 de junio de 2017, 01:06 PM

 *
 *       */


#include <stdio.h>
#include <stdlib.h>
#include "fFdisk.h"
#include "fMount.h"

/*
 * 
 */
int main(int argc, char** argv) {
    inicializarListaMount();
    inicializarVarFdisk();
    while (1) {
        consola();
    }


    return (EXIT_SUCCESS);
}

