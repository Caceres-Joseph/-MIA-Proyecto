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
#include "fMkfile.h"
void mkfile(char id[sizeChar],char ruta[sizeChar], char p, int size,char count[sizeChar]){
    printf("\tId=%s \n", id);
    printf("\tRuta= %s\n", ruta);
    printf("\tP= %c\n", p);
    printf("\tTamaño= %i\n", size);
    printf("\tCount= %s\n", count);
    
}
