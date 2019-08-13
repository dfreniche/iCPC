//
//  iCPCKeyboard.h
//  iCPC
//
//  Created by Diego Freniche Brito on 12/08/2019.
//

#ifndef iCPCKeyboard_h
#define iCPCKeyboard_h

#include <stdio.h>
#import "iCPCTypes.h"
#import "iCPCColors.h"

extern u8 bit_values[8];
extern int cpckeypressed[];

typedef struct {
    int left,top,right,bottom;
} RECT;


typedef enum {
    /* line 0, bit 0..bit 7 */
    CPC_CURSOR_UP, // = 0
    CPC_CURSOR_RIGHT,
    CPC_CURSOR_DOWN,
    CPC_F9,
    CPC_F6,
    CPC_F3,
    CPC_SMALL_ENTER,
    CPC_FDOT,
    /* line 1, bit 0..bit 7 */
    CPC_CURSOR_LEFT,
    CPC_COPY,
    CPC_F7,
    CPC_F8,
    CPC_F5,
    CPC_F1,
    CPC_F2,
    CPC_F0,
    /* line 2, bit 0..bit 7 */
    CPC_CLR,
    CPC_OPEN_SQUARE_BRACKET,
    CPC_RETURN,
    CPC_CLOSE_SQUARE_BRACKET,
    CPC_F4,
    CPC_SHIFT,
    CPC_FORWARD_SLASH,
    CPC_CONTROL,
    /* line 3, bit 0.. bit 7 */
    CPC_HAT,
    CPC_MINUS,
    CPC_AT,
    CPC_P,
    CPC_SEMICOLON,
    CPC_COLON,
    CPC_BACKSLASH,
    CPC_DOT,
    /* line 4, bit 0..bit 7 */
    CPC_ZERO,
    CPC_9,
    CPC_O,
    CPC_I,
    CPC_L,
    CPC_K,
    CPC_M,
    CPC_COMMA,
    /* line 5, bit 0..bit 7 */
    CPC_8,
    CPC_7,
    CPC_U,
    CPC_Y,
    CPC_H,
    CPC_J,
    CPC_N,
    CPC_SPACE,
    /* line 6, bit 0..bit 7 */
    CPC_6,
    CPC_5,
    CPC_R,
    CPC_T,
    CPC_G,
    CPC_F,
    CPC_B,
    CPC_V,
    /* line 7, bit 0.. bit 7 */
    CPC_4,
    CPC_3,
    CPC_E,
    CPC_W,
    CPC_S,
    CPC_D,
    CPC_C,
    CPC_X,
    /* line 8, bit 0.. bit 7 */
    CPC_1,
    CPC_2,
    CPC_ESC,
    CPC_Q,
    CPC_TAB,
    CPC_A,
    CPC_CAPS_LOCK,
    CPC_Z,
    /* line 9, bit 7..bit 0 */
    CPC_JOY_UP,
    CPC_JOY_DOWN,
    CPC_JOY_LEFT,
    CPC_JOY_RIGHT,
    CPC_JOY_FIRE1,
    CPC_JOY_FIRE2,
    CPC_SPARE,
    CPC_DEL,
    
    /* no key press */
    CPC_NIL
} CPC_SCANCODE;

extern CPC_SCANCODE keyown[11];


#define NBCPCKEY 74
typedef int CPC_KEY;

void CPC_SetScanCode(CPC_SCANCODE cpc_scancode);
void CPC_ClearScanCode(CPC_SCANCODE cpc_scancode);
void PressKey(CPC_KEY n);
void DispScanCode(CPC_SCANCODE scancode, int status);
void Dispkey(CPC_KEY n, int status);



#endif /* iCPCKeyboard_h */
