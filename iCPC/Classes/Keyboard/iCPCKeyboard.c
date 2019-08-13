//
//  iCPCKeyboard.c
//  iCPC
//
//  Created by Diego Freniche Brito on 12/08/2019.
//

#include "iCPCKeyboard.h"
#include "ppi.h"

int cpckeypressed[NBCPCKEY];


u8 bit_values[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

void Dispkey(CPC_KEY n, int status);
void DispScanCode(CPC_SCANCODE n, int status);


u16 *kbdBuffer;
int shifted=0;
int ctrled=0;
int copyed=0;

typedef struct {
    int normal;
} CPC_MAP;

CPC_SCANCODE keyown[11];

CPC_MAP keymap[NBCPCKEY] = {
    { CPC_FDOT },       // 0
    { CPC_F1 },
    { CPC_F2 },
    { CPC_F3 },
    { CPC_F4 },
    { CPC_F5 },
    { CPC_F6 },
    { CPC_F7 },
    { CPC_F8 },
    { CPC_F9 },     // 9
    { CPC_F0 },
    { CPC_CURSOR_UP },    // 11
    { CPC_CURSOR_LEFT },
    { CPC_CURSOR_DOWN },
    { CPC_CURSOR_RIGHT },
    
    { CPC_ESC },
    { CPC_1 },//16
    { CPC_2 },
    { CPC_3 },
    { CPC_4 },
    { CPC_5 }, // 20
    { CPC_6 },
    { CPC_7 },
    { CPC_8 },
    { CPC_9 },
    { CPC_ZERO },
    { CPC_MINUS },
    { CPC_HAT },
    { CPC_CLR },
    { CPC_DEL },
    
    { CPC_TAB },       // 30
    { CPC_Q },
    { CPC_W },
    { CPC_E },//33
    { CPC_R },
    { CPC_T },
    { CPC_Y },
    { CPC_U },
    { CPC_I },
    { CPC_O },
    { CPC_P },   // 40
    { CPC_AT },
    { CPC_OPEN_SQUARE_BRACKET },
    { CPC_RETURN },
    
    { CPC_CAPS_LOCK },   // 44
    { CPC_A },
    { CPC_S },
    { CPC_D },
    { CPC_F },
    { CPC_G },
    { CPC_H },     // 50
    { CPC_J },
    { CPC_K },
    { CPC_L },
    { CPC_COLON },
    { CPC_SEMICOLON },
    { CPC_CLOSE_SQUARE_BRACKET },   // 56
    
    { CPC_SHIFT },   // 57
    { CPC_Z },
    { CPC_X },
    { CPC_C },   // 60
    { CPC_V },
    { CPC_B },
    { CPC_N },
    { CPC_M },
    { CPC_COMMA },   // 65
    { CPC_DOT },
    { CPC_FORWARD_SLASH },
    { CPC_BACKSLASH },
    { CPC_SHIFT },
    
    
    { CPC_CONTROL },   // 70
    { CPC_COPY },
    { CPC_SPACE },
    { CPC_SMALL_ENTER }
};




// DIFB: positions on screen of each key
RECT keypos[NBCPCKEY] = {
    {0,51,14,72}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {15,51,33,72}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {34,51,52,72}, // 17    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {53,51,70,72}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {71,51,87,72}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {88,51,104,72}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {105,51,121,72}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {122,51,138,72}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {139,51,155,72}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {156,51,172,72}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {173,51,189,72}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {190,51,206,72}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {207,51,223,72}, //    0x50 | MOD_CPC_SHIFT,   // CPC_LEFT
    {224,51,240,72},   //    0x41 | MOD_CPC_SHIFT,   // CPC_UP
    {241,51,255,72},  // CP_RIGHT
    
    {0,73,14,94},    // (0)
    {15,73,33,94},   //    0x80 | MOD_CPC_SHIFT,   // CPC_1
    {34,73,52,94},   //    0x81 | MOD_CPC_SHIFT,   // CPC_2
    {53,73,70,94}, //    0x71 | MOD_CPC_SHIFT,   // CPC_3
    {71,73,87,94}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {88,73,104,94}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    {105,73,121,94}, //    0x60 | MOD_CPC_SHIFT,   // CPC_6
    {122,73,138,94}, //    0x51 | MOD_CPC_SHIFT,   // CPC_7
    {139,73,155,94}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {156,73,172,94}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {173,73,189,94}, // (10)  0x40 | MOD_CPC_SHIFT,   // CPC_0
    {190,73,206,94}, //    0x70 | MOD_CPC_SHIFT,   // CPC_=
    {207,73,223,94}, //    0x61 | MOD_CPC_SHIFT,   // CPC_LAMBDA
    {224,73,240,94}, //    0x60 | MOD_CPC_SHIFT,   // CPC_CLR
    {241,73,256,94}, //    0x51 | MOD_CPC_SHIFT,   // CPC_DEL
    
    {0,95,19,116},
    {20,95,38,116}, //    0x83,                   // CPC_a
    {39,95,57,116}, //    0x73,                   // CPC_z
    {58,95,76,116}, //    0x71 | MOD_CPC_SHIFT,   // CPC_3
    {77,95,95,116}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {96,95,114,116}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    {115,95,133,116}, //    0x60 | MOD_CPC_SHIFT,   // CPC_6
    {134,95,152,116}, //    0x51 | MOD_CPC_SHIFT,   // CPC_7
    {153,95,171,116}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {172,95,190,116}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {191,95,207,116}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {208,95,224,116}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {225,95,241,116}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    {242,95,256,138},//    0x22,                   // CPC_RETURN
    
    {0,117,21,138},
    {22,117,40,138}, //    0x83,                   // CPC_A
    {41,117,59,138}, //    0x73,                   // CPC_S
    {60,117,78,138}, //    0x71 | MOD_CPC_SHIFT,   // CPC_D
    {79,117,97,138}, //    0x70 | MOD_CPC_SHIFT,   // CPC_F
    {98,117,116,138}, //    0x61 | MOD_CPC_SHIFT,   // CPC_G
    {117,117,135,138}, //    0x60 | MOD_CPC_SHIFT,   // CPC_H
    {136,117,154,138}, //    0x51 | MOD_CPC_SHIFT,   // CPC_J
    {155,117,173,138}, //    0x50 | MOD_CPC_SHIFT,   // CPC_K
    {174,117,190,138}, //    0x41 | MOD_CPC_SHIFT,   // CPC_L
    {191,117,207,138}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {208,117,224,138}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {225,117,241,138}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    
    {0,139,28,160},  // SHIFT
    {29,139,47,160}, //    0x81 | MOD_CPC_SHIFT,   // CPC_2
    {48,139,66,160}, //    0x71 | MOD_CPC_SHIFT,   // CPC_3
    {67,139,85,160}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {86,139,104,160}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    {105,139,123,160}, //    0x60 | MOD_CPC_SHIFT,   // CPC_6
    {124,139,142,160}, //    0x51 | MOD_CPC_SHIFT,   // CPC_7
    {143,139,161,160}, //    0x50 | MOD_CPC_SHIFT,   // CPC_8
    {162,139,178,160}, //    0x41 | MOD_CPC_SHIFT,   // CPC_9
    {179,139,195,160}, //    0x40 | MOD_CPC_SHIFT,   // CPC_0
    {196,139,212,160}, //    0x70 | MOD_CPC_SHIFT,   // CPC_4
    {213,139,229,160}, //    0x61 | MOD_CPC_SHIFT,   // CPC_5
    {230,139,256,160}, //    0x60 | MOD_CPC_SHIFT,   // CPC_6
    
    {0,161,57,182}, //    0x55,                   // CPC_j
    {58,161,95,182}, //    0x55,                   // CPC_j
    {96,161,207,182}, //    0x55,                   // CPC_j
    {208,161,256,182} //    0x55,                   // CPC_j
};



void DispScanCode(CPC_SCANCODE scancode, int status)
{
    int n;
    
    for(n=0;n<NBCPCKEY;n++) {
        if (keymap[n].normal == scancode)  {
            Dispkey(n, status);
        }
    }
}

void PressKey(CPC_KEY n)
{
    CPC_SCANCODE cpc_scancode;
    cpc_scancode=keymap[n].normal;
    
    Dispkey(n, 1);
    
    if (shifted) {
        DispScanCode(CPC_SHIFT, 0 | 16);
        shifted=0;
        clav[0x25 >> 4] &= ~bit_values[0x25 & 7]; // key needs to be SHIFTed
    }
    if (ctrled) {
        DispScanCode(CPC_CONTROL, 0);
        ctrled=0;
        clav[0x27 >> 4] &= ~bit_values[0x27 & 7]; // CONTROL key is held down
    }
    if (copyed) {
        DispScanCode(CPC_COPY, 0);
        copyed=0;
    }
    
    clav[(u8)cpc_scancode >> 3] &= ~bit_values[(u8)cpc_scancode & 7];
    
    switch(cpc_scancode) {
        case CPC_SHIFT:
            if (shifted) {
                DispScanCode(cpc_scancode, 0 | 16);
                shifted=0;
            } else {
                DispScanCode(cpc_scancode, 1 | 16);
                shifted=1;
            }
            break;
        case CPC_CONTROL:
            if (ctrled) {
                DispScanCode(cpc_scancode, 0 | 16);
                ctrled=0;
            } else {
                DispScanCode(cpc_scancode, 1 | 16);
                ctrled=1;
            }
            break;
        case CPC_COPY:
            if (copyed) {
                DispScanCode(cpc_scancode, 0 | 16);
                copyed=0;
            } else {
                DispScanCode(cpc_scancode, 1 | 16);
                copyed=1;
            }
            break;
        default:
            break;
    }
}


// 1: active
// 2: on
// 0: off

void Dispkey(CPC_KEY n, int status)
{
#ifndef USE_CONSOLE
    int x,y;
    u16 color;
    
    
    if ((status&16)!=16) {
        if ((keymap[n].normal==CPC_SHIFT) || (keymap[n].normal==CPC_CONTROL) || (keymap[n].normal==CPC_COPY)) {
            return;
        }
    }
    
    switch(status) {
        case 0:
        case 16:
            for(y=keypos[n].top;y<keypos[n].bottom;y++) {
                for(x=keypos[n].left;x<keypos[n].right;x++) {
                    backBuffer[x+y*256]=kbdBuffer[x+y*256];
                }
            }
            break;
        case 17:
        case 1:
            color=RGB15(15,0,0);
            for(y=keypos[n].top;y<keypos[n].bottom;y++) {
                for(x=keypos[n].left;x<keypos[n].right;x++) {
                    backBuffer[x+y*256]=AlphaBlendFast(kbdBuffer[x+y*256], color);
                }
            }
            cpckeypressed[n]=2;
            break;
        case 2:
        case 18:
            color=RGB15(0,15,0);
            for(y=keypos[n].top;y<keypos[n].bottom;y++) {
                for(x=keypos[n].left;x<keypos[n].right;x++) {
                    backBuffer[x+y*256]=~kbdBuffer[x+y*256]|0x8000;
                    // backBuffer[x+y*256]=AlphaBlendFast(kbdBuffer[x+y*256], color);
                }
            }
            break;
    }
#endif
}


void CPC_SetScanCode(CPC_SCANCODE cpc_scancode)
{
    clav[(u8)cpc_scancode >> 3] &= ~bit_values[(u8)cpc_scancode & 7];
    DispScanCode(cpc_scancode, 1);
}

void CPC_ClearScanCode(CPC_SCANCODE cpc_scancode)
{
    clav[(u8)cpc_scancode >> 3] |= bit_values[(u8)cpc_scancode & 7];
    DispScanCode(cpc_scancode, 0);
}
