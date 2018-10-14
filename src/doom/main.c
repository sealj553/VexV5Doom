/*
 * main.c
 *
 *  Created on: 13.05.2014
 *      Author: Florian
 */

/*---------------------------------------------------------------------*
 *  include files                                                      *
 *---------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

extern void D_DoomMain (void);

int mainfunc(void){
    //init stuff


	printf("\n\033[1;31m\r\nSTM32Doom\033[0m\n");

	D_DoomMain();

	while(1){}
}
