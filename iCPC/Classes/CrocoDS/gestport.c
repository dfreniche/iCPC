/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!Ports!
/******************************************************************************/

/********************************************************* !NAME! **************
* !./FLE!
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\Fichiers
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | WIN-CPC                 | Chips
* ------------------------------------------------------------------------------
*  Fichier     : GESTPORT.C            | Version : 0.1x
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Gestion des ports d'entr�e/sortie
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
*  09/01/2003              | L.DEPLANQUE             | Version 0.1u : Test
*                          |                         | nbre de ports �crits
*                          |                         | simultan�ments (proto)
* ------------------------------------------------------------------------------
*  22/01/2004              | L.DEPLANQUE             | Version 0.1x : Ajout
*                          |                         | adresse du PC dans les
*                          |                         | traces du fichier .LOG
* ------------------------------------------------------------------------------
********************************************************** !END! **************/


#include  "nds.h"

#include  "types.h"
#include  "plateform.h"
#include  "config.h"
#include  "ppi.h"
#include  "crtc.h"
#include  "upd.h"
#include  "vga.h"
#include "gestport.h"

/********************************************************* !NAME! **************
* Nom : ReadPort
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un port d'entr�e
*
* R�sultat    : La valeur du port selectionn�
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
u8 ReadPort( u16 port )
{
    if ( ! ( port & 0x0480 ) )
        return( ReadUPD( port ) );

    if ( ! ( port & 0x0800 ) )
        return( ReadPPI( port ) );

    // myprintf("Acc�s lecture port %04X", port);
    return( 0xFF );
}


/********************************************************* !NAME! **************
* Nom : WritePort
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un port de sortie
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
void WritePort( u16 port, u8 val )
{
    //char chaine[256];
    //sprintf(chaine, "P%04X %d", port, val);
    if ( ( port & 0xC000 ) == 0x04000 ) {
        // strcat(chaine, " VGA");
        WriteVGA( val );
        }

    if ( ! ( port & 0x4000 ) ) {
        // strcat(chaine, " CRC");
        WriteCRTC( port, val );
        }

    if ( ! ( port & 0x2000 ) ) {
        // strcat(chaine, " ROM");
        WriteROM( val );
        }

    if ( ! ( port & 0x1000 ) ) {
        // strcat(chaine, " PRN");
    //    PrintVal( val );
        }

    if ( ! ( port & 0x0800 ) ) {
        // strcat(chaine, " PPI");
        WritePPI( port, val );
        }

    if ( ! ( port & 0x0480 ) ) {
        // strcat(chaine, " UPD");
        WriteUPD( port, val );
        }

    // myprintf(chaine);
}

