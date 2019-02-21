/******************************************************************************/
/* Configuration pour l'archivage des diff�rents �l�ments du fichier source   */
/******************************************************************************/
// !CONFIG!=/L/* /R/* /W"* Nom : "
// D�finition du syst�me       !CONFIG!=/V1!EMULATEUR CPC!
// D�finition du sous syst�me  !CONFIG!=/V2!WIN-CPC!
// D�finition du sous ensemble !CONFIG!=/V3!Chips!
// D�finition du module        !CONFIG!=/V4!VGA!
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
*  Fichier     : VGA.C                 | Version : 0.1p
* ------------------------------------------------------------------------------
*  Date        : 05/11/2002            | Auteur  : L.DEPLANQUE
* ------------------------------------------------------------------------------
*  Description : Emulation du circuit VGA
*
* ------------------------------------------------------------------------------
*  Historique  :
*           Date           |         Auteur          |       Description
* ------------------------------------------------------------------------------
*  05/11/2002              | L.DEPLANQUE             | creation
* ------------------------------------------------------------------------------
********************************************************** !END! **************/

#include "nds.h"

#include  "types.h"
#include  "config.h"
#ifdef USE_LOG
#include  "log.h"
#endif
#include  "vga.h"


#include "z80.h"

#include "plateform.h"

extern int CntHSync;

u8 *TabPOKE[4];
u8 *TabPEEK[4];

/********************************************************* !NAME! **************
* Nom : MemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : M�moire du CPC
*
********************************************************** !0! ****************/
UBYTE *MemCPC;

// FAST RAM

u8 TabCoul[ 32 ];  // Couleurs R,V,B


/********************************************************* !NAME! **************
* Nom : RamSelect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Num�ro de ram s�lectionn�e (128K)
*
********************************************************** !0! ****************/
int RamSelect;


/********************************************************* !NAME! **************
* Nom : lastMode
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mode �cran s�lectionn�
*
********************************************************** !0! ****************/
int lastMode;


/********************************************************* !NAME! **************
* Nom : DecodeurAdresse
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : ROMs activ�es/d�sactiv�es
*
********************************************************** !0! ****************/
int DecodeurAdresse;


/********************************************************* !NAME! **************
* Nom : RomExt
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Num�ro de rom sup�rieure s�lectionn�e
*
********************************************************** !0! ****************/
int RomExt;


/********************************************************* !NAME! **************
* Nom : PenSelect
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Num�ro de stylot s�lectionn�
*
********************************************************** !0! ****************/
int PenSelect=0;


/********************************************************* !NAME! **************
* Nom : ROMINF
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Rom inf�rieure
*
********************************************************** !0! ****************/
// static u8 *ROMINF;
static UBYTE ROMINF[ 0x4000 ];


/********************************************************* !NAME! **************
* Nom : ROMSUP
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Rom sup�rieure
*
********************************************************** !0! ****************/
// static u8 *ROMSUP;
static UBYTE ROMSUP[ 0x4000 ];


/********************************************************* !NAME! **************
* Nom : ROMDISC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Rom disque (Amsdos)
*
********************************************************** !0! ****************/
// static u8 *ROMDISC;
static UBYTE ROMDISC[ 0x4000 ];



/********************************************************* !NAME! **************
* Nom : AdjRam
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Configuration de la ram en fonction du registre de s�lection
*
********************************************************** !0! ****************/
static int AdjRam[4][8] =
    {
        {0x00000, 0x00000, 0x10000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000},
        {0x04000, 0x04000, 0x14000, 0x0C000, 0x10000, 0x14000, 0x18000, 0x1C000},
        {0x08000, 0x08000, 0x18000, 0x08000, 0x08000, 0x08000, 0x08000, 0x08000},
        {0x0C000, 0x1C000, 0x1C000, 0x1C000, 0x0C000, 0x0C000, 0x0C000, 0x0C000}
    };


/********************************************************* !NAME! **************
* Nom : SetMemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Mapping de la m�moire du CPC en fonction de la s�lection des
*               roms et rams
*
* R�sultat    : /
*
* Variables globales modifi�es : TabPOKE, TabPEEK
*
********************************************************** !0! ****************/
static void SetMemCPC( void )
{
    /*
    static int n=0;
    myprintf("SetMemCPC %d", n);
    n++;
    */

    TabPOKE[ 0 ] = &MemCPC[ AdjRam[ 0 ][ RamSelect ] ];
    TabPEEK[ 0 ] = ( DecodeurAdresse & ROMINF_OFF ) ? &MemCPC[ AdjRam[ 0 ][ RamSelect ] ] : ROMINF;

    TabPOKE[ 1 ] =
    TabPEEK[ 1 ] = &MemCPC[ AdjRam[ 1 ][ RamSelect ] ];

    TabPOKE[ 2 ] =
    TabPEEK[ 2 ] = &MemCPC[ AdjRam[ 2 ][ RamSelect ] ];

    TabPOKE[ 3 ] = &MemCPC[ AdjRam[ 3 ][ RamSelect ] ];
    
    TabPEEK[ 3 ] = ( DecodeurAdresse & ROMSUP_OFF ) ? &MemCPC[ AdjRam[ 3 ][ RamSelect ] ] : ( RomExt == 7 ) ?  ROMDISC : ROMSUP;
    
//    memcpy(z80Bank + 0*16384, &MemCPC[ AdjRam[ 0 ][ RamSelect ] ]
}


/********************************************************* !NAME! **************
* Nom : WriteVGA
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un registre du VGA
*
* R�sultat    : /
*
* Variables globales modifi�es : PenSelect, TabCoul, TabInk, DecodeurAdresse,
*                                lastMode, CntHSync, RamSelect
*
********************************************************** !0! ****************/
void WriteVGA( u8 val )
{
    u8 newVal = val & 0x1F;

//    myprintf("VGA: %d %d", val&0x1F, val >> 6);

    switch( val >> 6 )
        {
        case 0 :
            if ((newVal&0x10)==0) {
                PenSelect = newVal & 0x0f;
            } else {
                PenSelect = 0x10;
            }
            break;

        case 1 :
            if (TabCoul[ PenSelect ] != newVal) {
                TabCoul[ PenSelect ] = newVal;
                UpdateInk=1;
                }
            break;

        case 2 :
            DecodeurAdresse = val;
            lastMode = val & 3;
            SetMemCPC();
            if ( val & 16 ) {
                CntHSync = 0;
                SetIRQZ80(0);
                }
            UpdateInk=1;
            break;

        case 3 :
            RamSelect = val & 7;
            SetMemCPC();
            break;
        }
}


/********************************************************* !NAME! **************
* Nom : WriteROM
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : S�lection du num�ro de rom sup�rieure
*
* R�sultat    : /
*
* Variables globales modifi�es : RomExt
*
********************************************************** !0! ****************/
void WriteROM( int val )
{
    RomExt = val;
    SetMemCPC();
}


/********************************************************* !NAME! **************
* Nom : InitMemCPC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Initialisation : lecture des roms du cpc
*
* R�sultat    : TRUE si ok, FALSE sinon
*
* Variables globales modifi�es : ROMINF, ROMSUP, ROMDISC
*
********************************************************** !0! ****************/
BOOL InitMemCPC(const char *cpc6128_bin, const char *romdisc_bin)
{
    MemCPC = MyAlloc(0x20000, "Memory CPC"); // 128Ko
    
    // memcpy(ROMINF, rominf_bin, sizeof(ROMINF));
    // memcpy(ROMSUP, romsup_bin, sizeof( ROMSUP ));
    /* 
    ROMINF = (u8*)cpc6128_bin;
    ROMSUP = ((u8*)cpc6128_bin) + 16384;
    ROMDISC =  (u8*)romdisc_bin;
    */
    
    
    memcpy(ROMINF, cpc6128_bin, sizeof(ROMINF));
    memcpy(ROMSUP, cpc6128_bin + 0x4000, sizeof(ROMSUP));
    memcpy(ROMDISC, romdisc_bin, sizeof(ROMDISC));

   // emulator_patch_ROM(ROMINF); // Patch de la langue (entre autres)

    WriteVGA(0x89);
    WriteVGA(0xC0);

    return( TRUE );
}
