/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Noyau!
// D�finition du module        !CONFIG!=/V4!Types!
/********************************************************* !NAME! **************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : V0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions de types pr�d�finis et de constantes de compilation
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


typedef unsigned short          USHORT;
typedef signed short            SHORT;
typedef unsigned char           UBYTE;
typedef unsigned long           ULONG;
// typedef signed char        BOOL; 

#ifndef FALSE
enum { FALSE, TRUE };
#endif

#define CPC_VISIBLE_SCR_WIDTH 256
#define CPC_VISIBLE_SCR_HEIGHT 240


#define     USE_DEBUG
#undef     USE_TAPE
#undef     USE_LOG
#undef     USE_PRINTER
#undef     USE_MULTIFACE
#define     USE_SOUND
#undef     USE_SOUND_CAS
#define    USE_SNAPSHOT
#define     USE_Z80_ORIG
#undef     USE_Z80_ASM

#define    USE_SAVESNAP

#define USE_CONSOLE
#undef USE_ALTERSCREEN

#endif
