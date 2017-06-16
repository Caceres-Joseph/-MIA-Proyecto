/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mbr.h
 * Author: joseph
 *
 * Created on 16 de junio de 2017, 10:54 AM
 */

#ifndef MBR_H
#define MBR_H
#include "estructuras.h"

bloqueMBR leerMBR(char ruta[sizeChar]);
void actualizarMBR(bloqueMBR mbr, char ruta[sizeChar]);
bloqueMBR crearMBR(int tamanioDeDisco);


#endif /* MBR_H */

