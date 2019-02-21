/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Accessoires!
// D�finition du module        !CONFIG!=/V4!Gestion Configuration!
/********************************************************* !NAME! **************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : V0.1v
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions pour le module CONFIG.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  06/01/2003              | L.DEPLANQUE             | Version 0.1t : Gestion
*                          |                         | nouvelles variables :
*                          |                         | localisation des roms
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u : Gestion
*                          |                         | nouvelles variables :
*                          |                         | touches joystick
* ------------------------------------------------------------------------------
*  03/02/2003              | L.DEPLANQUE             | Version 0.1v : Gestion
*                          |                         | nouvelles variables :
*                          |                         | localisation fichiers
*                          |                         | DSK, SNA, TAP
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef CONFIG_H
#define CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif
    
extern int Sound;

extern int Multiface;

extern int LogLevel;

extern char LocRomInf[];

extern char LocRomSup[];

extern char LocRomDisc[];

extern char LocRomMulti[];

extern int JoyLeft;

extern int JoyRight;

extern int JoyUp;

extern int JoyDown;

extern int JoyFire1;

extern int JoyFire2;

extern int DureeVbl;

extern char DirSnap[];

extern char DirTape[];

extern char DirDisc[];

extern char DirEmuDisc[];


u8 ReadConfig( void );
    

    
#ifdef __cplusplus
    }
#endif

#endif
