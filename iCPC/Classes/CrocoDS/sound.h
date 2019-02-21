/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Accessoires!
// D�finition du module        !CONFIG!=/V4!Gestion sons!
/********************************************************* !NAME! **************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*  Fichier     :                       | Version : V0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : D�finitions pour le module SNDWIN.C
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#ifndef SOUND_H
#define SOUND_H


#ifdef __cplusplus
extern "C" {
#endif
    
    u8 PlaySound( void );
void PauseSound( void );
void FifoOut(u8 cmd, u8 reg, u8 val);

void Reset8912( void );
    void Write8912( u32 r, u32 val );       
int Read8912( int r );

    void Loop_Sound(int samplerate);


    
#ifdef __cplusplus
}
#endif

#endif
