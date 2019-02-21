/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!PPI 8255!
/********************************************************* !NAME! **************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : V0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions pour le module PPI.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  07/11/2002              | L.DEPLANQUE             | Ajout gestion lecture et
*                          |                         | �criture cassette.:
*                          |                         | fonctions WriteCas() et
*                          |                         | ReadCas().
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef PPI_H
#define PPI_H

#ifdef __cplusplus
extern "C" {
#endif

extern u8 clav[ 16 ];

extern int RegsPPI[ 4 ];

extern int RegPSGSel;



void WritePPI( int adr, int val );

int ReadPPI( int adr );

void WriteCas( void );

void ReadCas( void );

void OpenTapWrite( char * Nom );

void OpenTapRead( char * Nom );

void CloseTap( void );

BOOL Keyboard_HasBeenScanned(void);
void Keyboard_Reset(void);
    
#ifdef __cplusplus
}
#endif


#endif


