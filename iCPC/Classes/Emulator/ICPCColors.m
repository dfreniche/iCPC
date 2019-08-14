//
//  ICPCColors.m
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#import "ICPCColors.h"
#import "ICPCTypes.h"

@implementation ICPCColors

int RGBCPCdef[ 32 ] =  {
    0x7F7F7F,                 // Blanc            (13)
    0x7F7F7F,                 // Blanc            (13)
    0x00FF7F,                 // Vert Marin       (19)
    0xFFFF7F,                 // Jaune Pastel     (25)
    0x00007F,                 // Bleu              (1)
    0xFF007F,                 // Pourpre           (7)
    0x007F7F,                 // Turquoise        (10)
    0xFF7F7F,                 // Rose             (16)
    0xFF007F,                 // Pourpre           (7)
    0xFFFF00,                 // Jaune vif        (24)
    0xFFFF00,                 // Jaune vif        (24)
    0xFFFFFF,                 // Blanc Brillant   (26)
    0xFF0000,                 // Rouge vif         (6)
    0xFF00FF,                 // Magenta vif       (8)
    0xFF7F00,                 // Orange           (15)
    0xFF7FFF,                 // Magenta pastel   (17)
    0x00007F,                 // Bleu              (1)
    0x00FF7F,                 // Vert Marin       (19)
    0x00FF00,                 // Vert vif         (18)
    0x00FFFF,                 // Turquoise vif    (20)
    0x000000,                 // Noir              (0)
    0x0000FF,                 // Bleu vif          (2)
    0x007F00,                 // Vert              (9)
    0x007FFF,                 // Bleu ciel        (11)
    0x7F007F,                 // Magenta           (4)
    0x7FFF7F,                 // Vert pastel      (22)
    0x7FFF00,                 // Vert citron      (21)
    0x7FFFFF,                 // Turquoise pastel (23)
    0x7F0000,                 // Rouge             (3)
    0x7F00FF,                 // Mauve             (5)
    0x7F7F00,                 // Jaune            (12)
    0x7F7FFF                  // Bleu pastel      (14)
};

- (int)cpcColor:(int)color {
    
    if (color >=0 && color <= 31) {
        return RGBCPCdef[color];
    }
    
    return RGBCPCdef[0];
}

u16 CPC_BG_PALETTE[32];

- (void) setPalette:(int)color {
    
    static int lastcolour=1;
    int i;
    
    if ( (color==0) || (color==1) ) {
        lastcolour=color;
        return;
    }
    
    if (color==-1) {
        color=lastcolour;
    }
    
    int (^colorPaletteBlock)(int, int, int) = ^int(int r, int g, int b) {
        
        return RGB15(r,g,b);
    };
    
    int (^greenPaletteBlock)(int, int, int) = ^int(int r, int g, int b) {
        
        g=(r+g+b)/3;
        b=0;
        r=0;
        
        return RGB15(r,g,b);
    };
    
    int (^invertedPaletteBlock)(int, int, int) = ^int(int r, int g, int b) {
        
        int z = (r+g+b)/3;
        
        return RGB15(z,z,z);
    };
    
    for ( i = 0; i < 32; i++ ) {
        int r = ( RGBCPCdef[ i ] >> 16 ) & 0xFF;
        int g = ( RGBCPCdef[ i ] >> 8 ) & 0xFF;
        int b = ( RGBCPCdef[ i ] >> 0 ) & 0xFF;
        
        if (color==1) {
            CPC_BG_PALETTE[i] = colorPaletteBlock(r,g,b);
        } else if (color == 0) {
            CPC_BG_PALETTE[i] = greenPaletteBlock(r,g,b);
        } else if (color == 3) {
            CPC_BG_PALETTE[i] = invertedPaletteBlock(r,g,b);
        }
    }
    
    if (color==1) {
        lastcolour=color;
    }
    
    if (color==0) {
        lastcolour=color;
    }
}


@end
