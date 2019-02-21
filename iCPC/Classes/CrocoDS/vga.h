/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!VGA!
/********************************************************* !NAME! **************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : V0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions pour le module VGA.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef VGA_H
#define VGA_H


#define   ROMINF_OFF        0x04
#define   ROMSUP_OFF        0x08


#ifdef __cplusplus
extern "C" {
#endif


extern u8 *MemCPC;

extern int adrEcr, DecodeurAdresse, RomExt, lastMode;

extern u8 TabCoul[ 32 ];

extern int RamSelect, PenSelect;

extern u16 RgbCPC[ 32 ];

extern u8 *TabPOKE[4];
extern u8 *TabPEEK[4];




void WriteVGA( u8 val );

void WriteROM( int val );

BOOL InitMemCPC(const char *cpc6128_bin, const char *romdisc_bin);
    
    
#ifdef __cplusplus
    }
#endif


#endif
