/*
********************************************************** !0! *****************
* ------------------------------------------------------------------------------
*          SYSTEME         |      SOUS SYSTEME       |      SOUS ENSEMBLE
* ------------------------------------------------------------------------------
*  EMULATEUR CPC           | PC-CPC                  | Chips
* ------------------------------------------------------------------------------
*  Fichier     : Z80.C                 | Version : 0.1am
* ------------------------------------------------------------------------------
* ------------------------------------------------------------------------------
*  Description : Emulation des instructions du Z80
*/


#include  "nds.h"

#include  <stdio.h>

#include  "types.h"
#include  "z80.h"
#include  "vga.h"
#include  "crtc.h"
#include  "gestport.h"
#include  "config.h"
#include "plateform.h"

#define HACK_IR
#undef HACK_LDIR
#define HACK_IRQ

#define MASK_14BIT      0x3FFF

extern int CntHSync;

#ifdef HACK_IRQ
int halted=0;
#endif

void VerifyIRQ(void);
int IRQ;

/********************************************************* !NAME! **************
* Nom : Z80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Registres Z80
*
********************************************************** !0! ****************/
SRegs Z80;
 
// register u32 peekreg asm("r7");
// register u32 pokereg asm("r8");

extern pfct tabCB[ 256 ];
extern pfct tabED[ 256 ];
extern pfct tabIX[ 256 ];
extern pfct tabIY[ 256 ];


/********************************************************* !NAME! **************
* Nom : TabDAA
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Utilis� pour �muler l'instruction DAA
*
********************************************************** !0! ****************/
static USHORT TabDAA[ 2048 ] =
    {
    0x0044, 0x0100, 0x0200, 0x0304, 0x0400, 0x0504, 0x0604, 0x0700,
    0x0808, 0x090C, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510,
    0x1000, 0x1104, 0x1204, 0x1300, 0x1404, 0x1500, 0x1600, 0x1704,
    0x180C, 0x1908, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530,
    0x2020, 0x2124, 0x2224, 0x2320, 0x2424, 0x2520, 0x2620, 0x2724,
    0x282C, 0x2928, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534,
    0x3024, 0x3120, 0x3220, 0x3324, 0x3420, 0x3524, 0x3624, 0x3720,
    0x3828, 0x392C, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510,
    0x4000, 0x4104, 0x4204, 0x4300, 0x4404, 0x4500, 0x4600, 0x4704,
    0x480C, 0x4908, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514,
    0x5004, 0x5100, 0x5200, 0x5304, 0x5400, 0x5504, 0x5604, 0x5700,
    0x5808, 0x590C, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534,
    0x6024, 0x6120, 0x6220, 0x6324, 0x6420, 0x6524, 0x6624, 0x6720,
    0x6828, 0x692C, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530,
    0x7020, 0x7124, 0x7224, 0x7320, 0x7424, 0x7520, 0x7620, 0x7724,
    0x782C, 0x7928, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590,
    0x8080, 0x8184, 0x8284, 0x8380, 0x8484, 0x8580, 0x8680, 0x8784,
    0x888C, 0x8988, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594,
    0x9084, 0x9180, 0x9280, 0x9384, 0x9480, 0x9584, 0x9684, 0x9780,
    0x9888, 0x998C, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701,
    0x0809, 0x090D, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705,
    0x180D, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725,
    0x282D, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721,
    0x3829, 0x392D, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705,
    0x480D, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701,
    0x5809, 0x590D, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x6025, 0x6121, 0x6221, 0x6325, 0x6421, 0x6525, 0x6625, 0x6721,
    0x6829, 0x692D, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531,
    0x7021, 0x7125, 0x7225, 0x7321, 0x7425, 0x7521, 0x7621, 0x7725,
    0x782D, 0x7929, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591,
    0x8081, 0x8185, 0x8285, 0x8381, 0x8485, 0x8581, 0x8681, 0x8785,
    0x888D, 0x8989, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595,
    0x9085, 0x9181, 0x9281, 0x9385, 0x9481, 0x9585, 0x9685, 0x9781,
    0x9889, 0x998D, 0xA0B5, 0xA1B1, 0xA2B1, 0xA3B5, 0xA4B1, 0xA5B5,
    0xA0A5, 0xA1A1, 0xA2A1, 0xA3A5, 0xA4A1, 0xA5A5, 0xA6A5, 0xA7A1,
    0xA8A9, 0xA9AD, 0xB0B1, 0xB1B5, 0xB2B5, 0xB3B1, 0xB4B5, 0xB5B1,
    0xB0A1, 0xB1A5, 0xB2A5, 0xB3A1, 0xB4A5, 0xB5A1, 0xB6A1, 0xB7A5,
    0xB8AD, 0xB9A9, 0xC095, 0xC191, 0xC291, 0xC395, 0xC491, 0xC595,
    0xC085, 0xC181, 0xC281, 0xC385, 0xC481, 0xC585, 0xC685, 0xC781,
    0xC889, 0xC98D, 0xD091, 0xD195, 0xD295, 0xD391, 0xD495, 0xD591,
    0xD081, 0xD185, 0xD285, 0xD381, 0xD485, 0xD581, 0xD681, 0xD785,
    0xD88D, 0xD989, 0xE0B1, 0xE1B5, 0xE2B5, 0xE3B1, 0xE4B5, 0xE5B1,
    0xE0A1, 0xE1A5, 0xE2A5, 0xE3A1, 0xE4A5, 0xE5A1, 0xE6A1, 0xE7A5,
    0xE8AD, 0xE9A9, 0xF0B5, 0xF1B1, 0xF2B1, 0xF3B5, 0xF4B1, 0xF5B5,
    0xF0A5, 0xF1A1, 0xF2A1, 0xF3A5, 0xF4A1, 0xF5A5, 0xF6A5, 0xF7A1,
    0xF8A9, 0xF9AD, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701,
    0x0809, 0x090D, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705,
    0x180D, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725,
    0x282D, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721,
    0x3829, 0x392D, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705,
    0x480D, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701,
    0x5809, 0x590D, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x0046, 0x0102, 0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702,
    0x080A, 0x090E, 0x0402, 0x0506, 0x0606, 0x0702, 0x080A, 0x090E,
    0x1002, 0x1106, 0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706,
    0x180E, 0x190A, 0x1406, 0x1502, 0x1602, 0x1706, 0x180E, 0x190A,
    0x2022, 0x2126, 0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726,
    0x282E, 0x292A, 0x2426, 0x2522, 0x2622, 0x2726, 0x282E, 0x292A,
    0x3026, 0x3122, 0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722,
    0x382A, 0x392E, 0x3422, 0x3526, 0x3626, 0x3722, 0x382A, 0x392E,
    0x4002, 0x4106, 0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706,
    0x480E, 0x490A, 0x4406, 0x4502, 0x4602, 0x4706, 0x480E, 0x490A,
    0x5006, 0x5102, 0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702,
    0x580A, 0x590E, 0x5402, 0x5506, 0x5606, 0x5702, 0x580A, 0x590E,
    0x6026, 0x6122, 0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722,
    0x682A, 0x692E, 0x6422, 0x6526, 0x6626, 0x6722, 0x682A, 0x692E,
    0x7022, 0x7126, 0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726,
    0x782E, 0x792A, 0x7426, 0x7522, 0x7622, 0x7726, 0x782E, 0x792A,
    0x8082, 0x8186, 0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786,
    0x888E, 0x898A, 0x8486, 0x8582, 0x8682, 0x8786, 0x888E, 0x898A,
    0x9086, 0x9182, 0x9282, 0x9386, 0x9482, 0x9586, 0x9686, 0x9782,
    0x988A, 0x998E, 0x3423, 0x3527, 0x3627, 0x3723, 0x382B, 0x392F,
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707,
    0x480F, 0x490B, 0x4407, 0x4503, 0x4603, 0x4707, 0x480F, 0x490B,
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703,
    0x580B, 0x590F, 0x5403, 0x5507, 0x5607, 0x5703, 0x580B, 0x590F,
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723,
    0x682B, 0x692F, 0x6423, 0x6527, 0x6627, 0x6723, 0x682B, 0x692F,
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727,
    0x782F, 0x792B, 0x7427, 0x7523, 0x7623, 0x7727, 0x782F, 0x792B,
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787,
    0x888F, 0x898B, 0x8487, 0x8583, 0x8683, 0x8787, 0x888F, 0x898B,
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783,
    0x988B, 0x998F, 0x9483, 0x9587, 0x9687, 0x9783, 0x988B, 0x998F,
    0xA0A7, 0xA1A3, 0xA2A3, 0xA3A7, 0xA4A3, 0xA5A7, 0xA6A7, 0xA7A3,
    0xA8AB, 0xA9AF, 0xA4A3, 0xA5A7, 0xA6A7, 0xA7A3, 0xA8AB, 0xA9AF,
    0xB0A3, 0xB1A7, 0xB2A7, 0xB3A3, 0xB4A7, 0xB5A3, 0xB6A3, 0xB7A7,
    0xB8AF, 0xB9AB, 0xB4A7, 0xB5A3, 0xB6A3, 0xB7A7, 0xB8AF, 0xB9AB,
    0xC087, 0xC183, 0xC283, 0xC387, 0xC483, 0xC587, 0xC687, 0xC783,
    0xC88B, 0xC98F, 0xC483, 0xC587, 0xC687, 0xC783, 0xC88B, 0xC98F,
    0xD083, 0xD187, 0xD287, 0xD383, 0xD487, 0xD583, 0xD683, 0xD787,
    0xD88F, 0xD98B, 0xD487, 0xD583, 0xD683, 0xD787, 0xD88F, 0xD98B,
    0xE0A3, 0xE1A7, 0xE2A7, 0xE3A3, 0xE4A7, 0xE5A3, 0xE6A3, 0xE7A7,
    0xE8AF, 0xE9AB, 0xE4A7, 0xE5A3, 0xE6A3, 0xE7A7, 0xE8AF, 0xE9AB,
    0xF0A7, 0xF1A3, 0xF2A3, 0xF3A7, 0xF4A3, 0xF5A7, 0xF6A7, 0xF7A3,
    0xF8AB, 0xF9AF, 0xF4A3, 0xF5A7, 0xF6A7, 0xF7A3, 0xF8AB, 0xF9AF,
    0x0047, 0x0103, 0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703,
    0x080B, 0x090F, 0x0403, 0x0507, 0x0607, 0x0703, 0x080B, 0x090F,
    0x1003, 0x1107, 0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707,
    0x180F, 0x190B, 0x1407, 0x1503, 0x1603, 0x1707, 0x180F, 0x190B,
    0x2023, 0x2127, 0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727,
    0x282F, 0x292B, 0x2427, 0x2523, 0x2623, 0x2727, 0x282F, 0x292B,
    0x3027, 0x3123, 0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723,
    0x382B, 0x392F, 0x3423, 0x3527, 0x3627, 0x3723, 0x382B, 0x392F,
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707,
    0x480F, 0x490B, 0x4407, 0x4503, 0x4603, 0x4707, 0x480F, 0x490B,
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703,
    0x580B, 0x590F, 0x5403, 0x5507, 0x5607, 0x5703, 0x580B, 0x590F,
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723,
    0x682B, 0x692F, 0x6423, 0x6527, 0x6627, 0x6723, 0x682B, 0x692F,
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727,
    0x782F, 0x792B, 0x7427, 0x7523, 0x7623, 0x7727, 0x782F, 0x792B,
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787,
    0x888F, 0x898B, 0x8487, 0x8583, 0x8683, 0x8787, 0x888F, 0x898B,
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783,
    0x988B, 0x998F, 0x9483, 0x9587, 0x9687, 0x9783, 0x988B, 0x998F,
    0x0604, 0x0700, 0x0808, 0x090C, 0x0A0C, 0x0B08, 0x0C0C, 0x0D08,
    0x0E08, 0x0F0C, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510,
    0x1600, 0x1704, 0x180C, 0x1908, 0x1A08, 0x1B0C, 0x1C08, 0x1D0C,
    0x1E0C, 0x1F08, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530,
    0x2620, 0x2724, 0x282C, 0x2928, 0x2A28, 0x2B2C, 0x2C28, 0x2D2C,
    0x2E2C, 0x2F28, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534,
    0x3624, 0x3720, 0x3828, 0x392C, 0x3A2C, 0x3B28, 0x3C2C, 0x3D28,
    0x3E28, 0x3F2C, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510,
    0x4600, 0x4704, 0x480C, 0x4908, 0x4A08, 0x4B0C, 0x4C08, 0x4D0C,
    0x4E0C, 0x4F08, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514,
    0x5604, 0x5700, 0x5808, 0x590C, 0x5A0C, 0x5B08, 0x5C0C, 0x5D08,
    0x5E08, 0x5F0C, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534,
    0x6624, 0x6720, 0x6828, 0x692C, 0x6A2C, 0x6B28, 0x6C2C, 0x6D28,
    0x6E28, 0x6F2C, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530,
    0x7620, 0x7724, 0x782C, 0x7928, 0x7A28, 0x7B2C, 0x7C28, 0x7D2C,
    0x7E2C, 0x7F28, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590,
    0x8680, 0x8784, 0x888C, 0x8988, 0x8A88, 0x8B8C, 0x8C88, 0x8D8C,
    0x8E8C, 0x8F88, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594,
    0x9684, 0x9780, 0x9888, 0x998C, 0x9A8C, 0x9B88, 0x9C8C, 0x9D88,
    0x9E88, 0x9F8C, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0605, 0x0701, 0x0809, 0x090D, 0x0A0D, 0x0B09, 0x0C0D, 0x0D09,
    0x0E09, 0x0F0D, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1601, 0x1705, 0x180D, 0x1909, 0x1A09, 0x1B0D, 0x1C09, 0x1D0D,
    0x1E0D, 0x1F09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2621, 0x2725, 0x282D, 0x2929, 0x2A29, 0x2B2D, 0x2C29, 0x2D2D,
    0x2E2D, 0x2F29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3625, 0x3721, 0x3829, 0x392D, 0x3A2D, 0x3B29, 0x3C2D, 0x3D29,
    0x3E29, 0x3F2D, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4601, 0x4705, 0x480D, 0x4909, 0x4A09, 0x4B0D, 0x4C09, 0x4D0D,
    0x4E0D, 0x4F09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5605, 0x5701, 0x5809, 0x590D, 0x5A0D, 0x5B09, 0x5C0D, 0x5D09,
    0x5E09, 0x5F0D, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x6625, 0x6721, 0x6829, 0x692D, 0x6A2D, 0x6B29, 0x6C2D, 0x6D29,
    0x6E29, 0x6F2D, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531,
    0x7621, 0x7725, 0x782D, 0x7929, 0x7A29, 0x7B2D, 0x7C29, 0x7D2D,
    0x7E2D, 0x7F29, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591,
    0x8681, 0x8785, 0x888D, 0x8989, 0x8A89, 0x8B8D, 0x8C89, 0x8D8D,
    0x8E8D, 0x8F89, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595,
    0x9685, 0x9781, 0x9889, 0x998D, 0x9A8D, 0x9B89, 0x9C8D, 0x9D89,
    0x9E89, 0x9F8D, 0xA0B5, 0xA1B1, 0xA2B1, 0xA3B5, 0xA4B1, 0xA5B5,
    0xA6A5, 0xA7A1, 0xA8A9, 0xA9AD, 0xAAAD, 0xABA9, 0xACAD, 0xADA9,
    0xAEA9, 0xAFAD, 0xB0B1, 0xB1B5, 0xB2B5, 0xB3B1, 0xB4B5, 0xB5B1,
    0xB6A1, 0xB7A5, 0xB8AD, 0xB9A9, 0xBAA9, 0xBBAD, 0xBCA9, 0xBDAD,
    0xBEAD, 0xBFA9, 0xC095, 0xC191, 0xC291, 0xC395, 0xC491, 0xC595,
    0xC685, 0xC781, 0xC889, 0xC98D, 0xCA8D, 0xCB89, 0xCC8D, 0xCD89,
    0xCE89, 0xCF8D, 0xD091, 0xD195, 0xD295, 0xD391, 0xD495, 0xD591,
    0xD681, 0xD785, 0xD88D, 0xD989, 0xDA89, 0xDB8D, 0xDC89, 0xDD8D,
    0xDE8D, 0xDF89, 0xE0B1, 0xE1B5, 0xE2B5, 0xE3B1, 0xE4B5, 0xE5B1,
    0xE6A1, 0xE7A5, 0xE8AD, 0xE9A9, 0xEAA9, 0xEBAD, 0xECA9, 0xEDAD,
    0xEEAD, 0xEFA9, 0xF0B5, 0xF1B1, 0xF2B1, 0xF3B5, 0xF4B1, 0xF5B5,
    0xF6A5, 0xF7A1, 0xF8A9, 0xF9AD, 0xFAAD, 0xFBA9, 0xFCAD, 0xFDA9,
    0xFEA9, 0xFFAD, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0605, 0x0701, 0x0809, 0x090D, 0x0A0D, 0x0B09, 0x0C0D, 0x0D09,
    0x0E09, 0x0F0D, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1601, 0x1705, 0x180D, 0x1909, 0x1A09, 0x1B0D, 0x1C09, 0x1D0D,
    0x1E0D, 0x1F09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2621, 0x2725, 0x282D, 0x2929, 0x2A29, 0x2B2D, 0x2C29, 0x2D2D,
    0x2E2D, 0x2F29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3625, 0x3721, 0x3829, 0x392D, 0x3A2D, 0x3B29, 0x3C2D, 0x3D29,
    0x3E29, 0x3F2D, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4601, 0x4705, 0x480D, 0x4909, 0x4A09, 0x4B0D, 0x4C09, 0x4D0D,
    0x4E0D, 0x4F09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5605, 0x5701, 0x5809, 0x590D, 0x5A0D, 0x5B09, 0x5C0D, 0x5D09,
    0x5E09, 0x5F0D, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0xFABE, 0xFBBA, 0xFCBE, 0xFDBA, 0xFEBA, 0xFFBE, 0x0046, 0x0102,
    0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702, 0x080A, 0x090E,
    0x0A1E, 0x0B1A, 0x0C1E, 0x0D1A, 0x0E1A, 0x0F1E, 0x1002, 0x1106,
    0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706, 0x180E, 0x190A,
    0x1A1A, 0x1B1E, 0x1C1A, 0x1D1E, 0x1E1E, 0x1F1A, 0x2022, 0x2126,
    0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726, 0x282E, 0x292A,
    0x2A3A, 0x2B3E, 0x2C3A, 0x2D3E, 0x2E3E, 0x2F3A, 0x3026, 0x3122,
    0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722, 0x382A, 0x392E,
    0x3A3E, 0x3B3A, 0x3C3E, 0x3D3A, 0x3E3A, 0x3F3E, 0x4002, 0x4106,
    0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706, 0x480E, 0x490A,
    0x4A1A, 0x4B1E, 0x4C1A, 0x4D1E, 0x4E1E, 0x4F1A, 0x5006, 0x5102,
    0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702, 0x580A, 0x590E,
    0x5A1E, 0x5B1A, 0x5C1E, 0x5D1A, 0x5E1A, 0x5F1E, 0x6026, 0x6122,
    0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722, 0x682A, 0x692E,
    0x6A3E, 0x6B3A, 0x6C3E, 0x6D3A, 0x6E3A, 0x6F3E, 0x7022, 0x7126,
    0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726, 0x782E, 0x792A,
    0x7A3A, 0x7B3E, 0x7C3A, 0x7D3E, 0x7E3E, 0x7F3A, 0x8082, 0x8186,
    0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786, 0x888E, 0x898A,
    0x8A9A, 0x8B9E, 0x8C9A, 0x8D9E, 0x8E9E, 0x8F9A, 0x9086, 0x9182,
    0x9282, 0x9386, 0x3423, 0x3527, 0x3627, 0x3723, 0x382B, 0x392F,
    0x3A3F, 0x3B3B, 0x3C3F, 0x3D3B, 0x3E3B, 0x3F3F, 0x4003, 0x4107,
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480F, 0x490B,
    0x4A1B, 0x4B1F, 0x4C1B, 0x4D1F, 0x4E1F, 0x4F1B, 0x5007, 0x5103,
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580B, 0x590F,
    0x5A1F, 0x5B1B, 0x5C1F, 0x5D1B, 0x5E1B, 0x5F1F, 0x6027, 0x6123,
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682B, 0x692F,
    0x6A3F, 0x6B3B, 0x6C3F, 0x6D3B, 0x6E3B, 0x6F3F, 0x7023, 0x7127,
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782F, 0x792B,
    0x7A3B, 0x7B3F, 0x7C3B, 0x7D3F, 0x7E3F, 0x7F3B, 0x8083, 0x8187,
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888F, 0x898B,
    0x8A9B, 0x8B9F, 0x8C9B, 0x8D9F, 0x8E9F, 0x8F9B, 0x9087, 0x9183,
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988B, 0x998F,
    0x9A9F, 0x9B9B, 0x9C9F, 0x9D9B, 0x9E9B, 0x9F9F, 0xA0A7, 0xA1A3,
    0xA2A3, 0xA3A7, 0xA4A3, 0xA5A7, 0xA6A7, 0xA7A3, 0xA8AB, 0xA9AF,
    0xAABF, 0xABBB, 0xACBF, 0xADBB, 0xAEBB, 0xAFBF, 0xB0A3, 0xB1A7,
    0xB2A7, 0xB3A3, 0xB4A7, 0xB5A3, 0xB6A3, 0xB7A7, 0xB8AF, 0xB9AB,
    0xBABB, 0xBBBF, 0xBCBB, 0xBDBF, 0xBEBF, 0xBFBB, 0xC087, 0xC183,
    0xC283, 0xC387, 0xC483, 0xC587, 0xC687, 0xC783, 0xC88B, 0xC98F,
    0xCA9F, 0xCB9B, 0xCC9F, 0xCD9B, 0xCE9B, 0xCF9F, 0xD083, 0xD187,
    0xD287, 0xD383, 0xD487, 0xD583, 0xD683, 0xD787, 0xD88F, 0xD98B,
    0xDA9B, 0xDB9F, 0xDC9B, 0xDD9F, 0xDE9F, 0xDF9B, 0xE0A3, 0xE1A7,
    0xE2A7, 0xE3A3, 0xE4A7, 0xE5A3, 0xE6A3, 0xE7A7, 0xE8AF, 0xE9AB,
    0xEABB, 0xEBBF, 0xECBB, 0xEDBF, 0xEEBF, 0xEFBB, 0xF0A7, 0xF1A3,
    0xF2A3, 0xF3A7, 0xF4A3, 0xF5A7, 0xF6A7, 0xF7A3, 0xF8AB, 0xF9AF,
    0xFABF, 0xFBBB, 0xFCBF, 0xFDBB, 0xFEBB, 0xFFBF, 0x0047, 0x0103,
    0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703, 0x080B, 0x090F,
    0x0A1F, 0x0B1B, 0x0C1F, 0x0D1B, 0x0E1B, 0x0F1F, 0x1003, 0x1107,
    0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707, 0x180F, 0x190B,
    0x1A1B, 0x1B1F, 0x1C1B, 0x1D1F, 0x1E1F, 0x1F1B, 0x2023, 0x2127,
    0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727, 0x282F, 0x292B,
    0x2A3B, 0x2B3F, 0x2C3B, 0x2D3F, 0x2E3F, 0x2F3B, 0x3027, 0x3123,
    0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723, 0x382B, 0x392F,
    0x3A3F, 0x3B3B, 0x3C3F, 0x3D3B, 0x3E3B, 0x3F3F, 0x4003, 0x4107,
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480F, 0x490B,
    0x4A1B, 0x4B1F, 0x4C1B, 0x4D1F, 0x4E1F, 0x4F1B, 0x5007, 0x5103,
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580B, 0x590F,
    0x5A1F, 0x5B1B, 0x5C1F, 0x5D1B, 0x5E1B, 0x5F1F, 0x6027, 0x6123,
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682B, 0x692F,
    0x6A3F, 0x6B3B, 0x6C3F, 0x6D3B, 0x6E3B, 0x6F3F, 0x7023, 0x7127,
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782F, 0x792B,
    0x7A3B, 0x7B3F, 0x7C3B, 0x7D3F, 0x7E3F, 0x7F3B, 0x8083, 0x8187,
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888F, 0x898B,
    0x8A9B, 0x8B9F, 0x8C9B, 0x8D9F, 0x8E9F, 0x8F9B, 0x9087, 0x9183,
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988B, 0x998F
    };


/********************************************************* !NAME! **************
* Nom : Parite
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Calcul du flag P, V, S, Z d'un registre 8 bits
*
********************************************************** !0! ****************/
static UBYTE Parite[ 256 ] =
    {
    0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84
    };


/********************************************************* !NAME! **************
* Nom : CBIndex
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Variables Globales
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Indique si l'instruction CB est pr�c�d�e d'une instruction DD
*               ou d'une instruction FD
*
********************************************************** !0! ****************/
static int CBIndex = 0;


/********************************************************* !NAME! **************
* Nom : Peek8Ext
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un octet (8 bits) depuis la m�moire du CPC (utilis�e
*               depuis l'ext�rieur du module)
*
* R�sultat    : La valeur de l'octet � l'adresse d�sir�e
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
UBYTE Peek8Ext( USHORT adr )
{
    UBYTE val = TabPEEK[ adr >> 14 ][ adr & MASK_14BIT ];
    return( val );
}


/********************************************************* !NAME! **************
* Nom : Poke8Ext
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un octet (8 bits) dans la m�moire du CPC (utilis�e
*               depuis l'ext�rieur du module)
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
void Poke8Ext( USHORT adr, UBYTE val )
{
    TabPOKE[ adr >> 14 ][ adr & MASK_14BIT ] = val;
}


/********************************************************* !NAME! **************
* Nom : PEEK8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un octet (8 bits) depuis la m�moire du CPC
*
* R�sultat    : La valeur de l'octet � l'adresse d�sir�e
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
static u8 PEEK8( u16 adr )
{
    // myprintf("R8: %d", adr);
    return( TabPEEK[ adr >> 14 ][ adr & MASK_14BIT ] );
}


/********************************************************* !NAME! **************
* Nom : POKE8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un octet (8 bits) dans la m�moire du CPC
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
static void POKE8( u16 adr, u8 val )
{
    TabPOKE[ adr >> 14 ][ adr & MASK_14BIT ] = val;
}

static void POKEPEEK8( u16 adr, u16 adr2)
{
    TabPOKE[ adr >> 14 ][ adr & MASK_14BIT ] = TabPEEK[ adr2 >> 14 ][ adr2 & MASK_14BIT ];
}

/********************************************************* !NAME! **************
* Nom : PEEK16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Lecture d'un mot (16 bits) depuis la m�moire du CPC
*
* R�sultat    : La valeur du mot � l'adresse d�sir�e
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
static u16 PEEK16( u16 adr )
{
    // myprintf("R16: %d", adr);
    u16 p = adr & MASK_14BIT;
    u16 q = adr >> 14;
    return( ( USHORT )( TabPEEK[ q ][ p ] | ( TabPEEK[ q ][ p + 1 ] << 8 ) ) );
}


/********************************************************* !NAME! **************
* Nom : POKE16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ecriture d'un mot (16 bits) dans la m�moire du CPC
*
* R�sultat    : /
*
* Variables globales modifi�es : /
*
********************************************************** !0! ****************/
static void POKE16( u16 adr, u16 val )
{
    u16 p = adr & MASK_14BIT;
    u16 q = adr >> 14;
    TabPOKE[ q ][ p ] = ( UBYTE )val;
    TabPOKE[ q ][ p + 1 ] = ( UBYTE )( val >> 8 );
}


/********************************************************* !NAME! **************
* Nom : ADD_R8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une addition 8 Bits
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void ADD_R8( int v )
{
    int t = RegA + v;

    FLAGS = ( UBYTE )( ( ~( RegA ^ v ) & ( v ^ t ) & 0x80 ? FLAG_V : 0 )
                     | ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ v ^ t ) & FLAG_H )
                     );
    RegA = ( UBYTE )t;
}


/********************************************************* !NAME! **************
* Nom : SUB_R8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une soustration 8 bits
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void SUB_R8( int v )
{
    int t = RegA - v;

    FLAGS = ( UBYTE )( ( ( RegA ^ v ) & ( RegA ^ t ) & 0x80 ? FLAG_V : 0 )
                     | FLAG_N
                     | - ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ v ^ t ) & FLAG_H )
                   );
    RegA = ( UBYTE )t;
}


/********************************************************* !NAME! **************
* Nom : ADC_R8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une addition 8 bits avec carry
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void ADC_R8( int v )
{
    int t = RegA + v + ( FLAGS & FLAG_C );

    FLAGS = ( UBYTE )( ( ~( RegA ^ v ) & ( v ^ t ) & 0x80 ? FLAG_V : 0 )
                     | ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ v ^ t ) & FLAG_H )
                     );
    RegA = ( UBYTE )t;
}


/********************************************************* !NAME! **************
* Nom : SBC_R8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une soustraction 8 bits avec carry
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void SBC_R8( int v )
{
    int t = RegA - v - ( FLAGS & FLAG_C );

    FLAGS = ( UBYTE )( ( ( RegA ^ v ) & ( RegA ^ t ) & 0x80 ? FLAG_V : 0 )
                     | FLAG_N
                     | - ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ v ^ t ) & FLAG_H )
                     );
    RegA = ( UBYTE )t;
}


/********************************************************* !NAME! **************
* Nom : CP_R8
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une comparaision 8 bits
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void CP_R8( int v )
{
    int t = RegA - v;

    FLAGS = ( UBYTE )( ( ( RegA ^ v ) & ( RegA ^ t ) & 0x80 ? FLAG_V : 0 )
                     | FLAG_N
                     | - ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ v ^ t ) & FLAG_H )
                     );
}


/********************************************************* !NAME! **************
* Nom : FLAG_INC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affecte les flags apr�s une instruction d'incr�mentation
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void FLAG_INC( int reg )
{
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C )
                     | ( reg & FLAG_S )
                     | ( reg == 0x80 ? FLAG_V : 0 )
                     | ( ( reg & 0x0F ) == 0 ? FLAG_H : 0 )
                     | ( reg ? 0 : FLAG_Z )
                     );
}


/********************************************************* !NAME! **************
* Nom : FLAG_DEC
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Affecte les flags apr�s une instruction de d�cr�mentation
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void FLAG_DEC( int reg )
{
    FLAGS = ( UBYTE )( FLAG_N
                     | ( FLAGS & FLAG_C )
                     | ( reg == 0x7F ? FLAG_V : 0 )
                     | ( ( reg & 0x0F ) == 0x0F ? FLAG_H : 0 )
                     | ( reg & FLAG_S )
                     | ( reg ? 0 : FLAG_Z )
                     );
}


/********************************************************* !NAME! **************
* Nom : ADD_R16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une addition 16 bits
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void ADD_R16( USHORT * Reg, int v )
{
    int tmp = * Reg;
    * Reg = ( USHORT )( * Reg + v );

    FLAGS = ( UBYTE )( FLAGS & ( FLAG_S | FLAG_Z | FLAG_V ) );
    if ( tmp > * Reg )
        FLAGS |= FLAG_C;

    if ( ( tmp ^ v ^ * Reg ) & 0x1000 )
        FLAGS |= FLAG_H;
}


/********************************************************* !NAME! **************
* Nom : ADC_R16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une addition 16 bits avec carry
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void ADC_R16( int v )
{
    int t = RegHL + v + ( FLAGS & FLAG_C );

    FLAGS = ( UBYTE )( ( t & 0x10000 ? FLAG_C : 0 )
                       | ( ~ ( RegHL ^ v ) & ( v ^ t ) & 0x8000 ? FLAG_V : 0 )
                       | ( ( RegHL ^ v ^ t ) & 0x1000 ? FLAG_H : 0 )
                       | ( t & 0xFFFF ? 0 : FLAG_Z )
                       | ( ( t >> 8 ) & FLAG_S )
                     );
    RegHL = ( USHORT )t;
}


/********************************************************* !NAME! **************
* Nom : SBC_R16
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Effectue une soustraction 16 bits avec carry
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void SBC_R16( int v )
{
    int t = RegHL - v - ( FLAGS & FLAG_C );

    FLAGS = ( UBYTE )( FLAG_N
                    | ( t & 0x10000 ? FLAG_C : 0 )
                    | ( ( RegHL ^ v ) & ( RegHL ^ t ) & 0x8000 ? FLAG_V : 0 )
                    | ( ( RegHL ^ v ^ t ) & 0x1000 ? FLAG_H : 0 )
                    | ( t & 0xFFFF ? 0 : FLAG_Z )
                    | ( (  t >> 8  ) & FLAG_S )
                    );
    RegHL = ( USHORT )t;
}


/********************************************************* !NAME! **************
* Nom : Bit
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Teste un bit d'un registre
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
static void Bit( UBYTE r, UBYTE Bit )
{
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | FLAG_H | Parite[ r & Bit ] );
}


/************
* OPCODE CB *
************/


static int CB_00( void )   /* RLC B */
{
    FLAGS = ( UBYTE )( RegB >> 7 );
    RegB = ( UBYTE )( ( RegB << 1 ) | FLAGS );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_01( void )   /* RLC C */
{
    FLAGS = ( UBYTE )( RegC >> 7 );
    RegC = ( UBYTE )( ( RegC << 1 ) | FLAGS );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_02( void )   /* RLC D */
{
    FLAGS = ( UBYTE )( RegD >> 7 );
    RegD = ( UBYTE )( ( RegD << 1 ) | FLAGS );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_03( void )   /* RLC E */
{
    FLAGS = ( UBYTE )( RegE >> 7 );
    RegE = ( UBYTE )( ( RegE << 1 ) | FLAGS );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_04( void )   /* RLC H */
{
    FLAGS = ( UBYTE )( RegH >> 7 );
    RegH = ( UBYTE )( ( RegH << 1 ) | FLAGS );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_05( void )   /* RLC L */
{
    FLAGS = ( UBYTE )( RegL >> 7 );
    RegL = ( UBYTE )( ( RegL << 1 ) | FLAGS );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );
    
    return( 2 /* 2 NOPs */ );
}


static int CB_06( void )   /* RLC (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl >> 7 );
    POKE8( RegHL , ( UBYTE )( hl = ( ( hl << 1 ) | FLAGS ) ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_07( void )   /* RLC A */
{
    FLAGS = ( UBYTE )( RegA >> 7 );
    RegA = ( UBYTE )( ( RegA << 1 ) | FLAGS );
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_08( void )   /* RRC B */
{
    FLAGS = ( UBYTE )( RegB & FLAG_C );
    RegB = ( UBYTE )( ( RegB >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_09( void )   /* RRC C */
{
    FLAGS = ( UBYTE )( RegC & FLAG_C );
    RegC = ( UBYTE )( ( RegC >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_0A( void )   /* RRC D */
{
    FLAGS = ( UBYTE )( RegD & FLAG_C );
    RegD = ( UBYTE )( ( RegD >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_0B( void )   /* RRC E */
{
    FLAGS = ( UBYTE )( RegE & FLAG_C );
    RegE = ( UBYTE )( ( RegE >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_0C( void )   /* RRC H */
{
    FLAGS = ( UBYTE )( RegH & FLAG_C );
    RegH = ( UBYTE )( ( RegH >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_0D( void )   /* RRC L */
{
    FLAGS = ( UBYTE )( RegL & FLAG_C );
    RegL = ( UBYTE )( ( RegL >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_0E( void )   /* RRC (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl & FLAG_C );
    POKE8( RegHL, ( UBYTE )( hl = ( hl >> 1 ) | ( FLAGS << 7 ) ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_0F( void )   /* RRC A */
{
    FLAGS = ( UBYTE )( RegA & FLAG_C );
    RegA = ( UBYTE )( ( RegA >> 1 ) | ( FLAGS << 7 ) );
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_10( void )   /* RL B */
{
    int i = RegB << 1;
    RegB = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegB ] );
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_11( void )   /* RL C */
{
    int i = RegC << 1;
    RegC = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegC ] );
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_12( void )   /* RL D */
{
    int i = RegD << 1;
    RegD = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegD ] );
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_13( void )   /* RL E */
{
    int i = RegE << 1;
    RegE = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegE ] );
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_14( void )   /* RL H */
{
    int i = RegH << 1;
    RegH = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) |Parite[ RegH ] );
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_15( void )   /* RL L */
{
    int i = RegL << 1;
    RegL = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegL ] );
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_16( void )   /* RL (HL) */
{
    int hl = PEEK8( RegHL );
    int i = hl << 1;
    POKE8( RegHL, ( UBYTE )( hl = i | ( FLAGS & FLAG_C ) ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ hl & 0xFF ] );
    return( 4 /* 4 NOPs */ );
}


static int CB_17( void )   /* RL A */
{
    int i = RegA << 1;
    RegA = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( i >> 8 ) | Parite[ RegA ] );
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_18( void )   /* RR B */
{
    UBYTE i = ( UBYTE )( ( RegB >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegB & FLAG_C ) | Parite[ i ] );
    RegB = i;
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_19( void )   /* RR C */
{
    UBYTE i = ( UBYTE )( ( RegC >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegC & FLAG_C ) | Parite[ i ] );
    RegC = i;
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_1A( void )   /* RR D */
{
    UBYTE i = ( UBYTE )( ( RegD >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegD & FLAG_C ) | Parite[ i ] );
    RegD = i;
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_1B( void )   /* RR E */
{
    UBYTE i = ( UBYTE )( ( RegE >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegE & FLAG_C ) | Parite[ i ] );
    RegE = i;
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_1C( void )   /* RR H */
{
    UBYTE i = ( UBYTE )( ( RegH >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegH & FLAG_C ) | Parite[ i ] );
    RegH = i;
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_1D( void )   /* RR L */
{
    UBYTE i = ( UBYTE )( ( RegL >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegL & FLAG_C ) | Parite[ i ] );
    RegL = i;
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_1E( void )   /* RR (HL) */
{
    int hl = PEEK8( RegHL );
    UBYTE i = ( UBYTE )( ( hl >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( hl & FLAG_C ) | Parite[ i ] );
    POKE8( RegHL, ( UBYTE )i );
    return( 4 /* 4 NOPs */ );
}


static int CB_1F( void )   /* RR A */
{
    UBYTE i = ( UBYTE )( ( RegA >> 1 ) | ( FLAGS << 7 ) );
    FLAGS = ( UBYTE )( ( RegA & FLAG_C ) | Parite[ i ] );
    RegA = i;
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_20( void )   /* SLA B */
{
    FLAGS = ( UBYTE )( RegB >> 7 );
    RegB = ( UBYTE )( RegB << 1 );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_21( void )   /* SLA C */
{
    FLAGS = ( UBYTE )( RegC >> 7 );
    RegC = ( UBYTE )( RegC << 1 );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_22( void )   /* SLA D */
{
    FLAGS = ( UBYTE )( RegD >> 7 );
    RegD = ( UBYTE )( RegD << 1 );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_23( void )   /* SLA E */
{
    FLAGS = ( UBYTE )( RegE >> 7 );
    RegE = ( UBYTE )( RegE << 1 );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_24( void )   /* SLA H */
{
    FLAGS = ( UBYTE )( RegH >> 7 );
    RegH = ( UBYTE )( RegH << 1 );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_25( void )   /* SLA L */
{
    FLAGS = ( UBYTE )( RegL >> 7 );
    RegL = ( UBYTE )( RegL << 1 );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_26( void )   /* SLA (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl >> 7 );
    POKE8( RegHL, ( UBYTE )( hl = hl << 1 ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_27( void )   /* SLA A */
{
    FLAGS = ( UBYTE )( RegA >> 7 );
    RegA = ( UBYTE )( RegA << 1 );
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_28( void )   /* SRA B */
{
    FLAGS = ( UBYTE )( RegB & FLAG_C );
    RegB = ( UBYTE )( ( RegB >> 1 ) | ( RegB & FLAG_S ) );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_29( void )   /* SRA C */
{
    FLAGS = ( UBYTE )( RegC & FLAG_C );
    RegC = ( UBYTE )( ( RegC >> 1 ) | ( RegC & FLAG_S ) );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_2A( void )   /* SRA D */
{
    FLAGS = ( UBYTE )( RegD & FLAG_C );
    RegD = ( UBYTE )( ( RegD >> 1 ) | ( RegD & FLAG_S ) );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_2B( void )   /* SRA E */
{
    FLAGS = ( UBYTE )( RegE & FLAG_C );
    RegE = ( UBYTE )( ( RegE >> 1 ) | ( RegE & FLAG_S ) );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_2C( void )   /* SRA H */
{
    FLAGS = ( UBYTE )( RegH & FLAG_C );
    RegH = ( UBYTE )( ( RegH >> 1 ) | ( RegH & FLAG_S ) );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_2D( void )   /* SRA L */
{
    FLAGS = ( UBYTE )( RegL & FLAG_C );
    RegL = ( UBYTE )( ( RegL >> 1 ) | ( RegL & FLAG_S ) );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_2E( void )   /* SRA (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl & FLAG_C );
    POKE8( RegHL, ( UBYTE )( hl = ( hl >> 1 ) | ( hl & FLAG_S ) ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_2F( void )   /* SRA A */
{
    FLAGS = ( UBYTE )( RegA & FLAG_C );
    RegA = ( UBYTE )( ( RegA >> 1 ) | ( RegA & FLAG_S ) );
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_30( void )   /* SLL B */
{
    FLAGS = ( UBYTE )( RegB >> 7 );
    RegB = ( UBYTE )( ( RegB << 1 ) | 1 );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_31( void )   /* SLL C */
{
    FLAGS = ( UBYTE )( RegC >> 7 );
    RegC = ( UBYTE )( ( RegC << 1 ) | 1 );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_32( void )   /* SLL D */
{
    FLAGS = ( UBYTE )( RegD >> 7 );
    RegD = ( UBYTE )( ( RegD << 1 ) | 1 );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_33( void )   /* SLL E */
{
    FLAGS = ( UBYTE )( RegE >> 7 );
    RegE = ( UBYTE )( ( RegE << 1 ) | 1 );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_34( void )   /* SLL H */
{
    FLAGS = ( UBYTE )( RegH >> 7 );
    RegH = ( UBYTE )( ( RegH << 1 ) | 1 );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_35( void )   /* SLL L */
{
    FLAGS = ( UBYTE )( RegL >> 7 );
    RegL = ( UBYTE )( ( RegL << 1 ) | 1 );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_36( void )   /* SLL (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl >> 7 );
    POKE8( RegHL, ( UBYTE )( ( hl = hl << 1 ) | 1 ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_37( void )   /* SLL A */
{
    FLAGS = ( UBYTE )( RegA >> 7 );
    RegA = ( UBYTE )( ( RegA << 1 ) | 1);
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_38( void )   /* SRL B */
{
    FLAGS = ( UBYTE )( RegB & FLAG_C );
    RegB = ( UBYTE )( RegB >> 1 );
    FLAGS |= Parite[ RegB ];
    if ( CBIndex )
        POKE8( RegHL, RegB );

    return( 2 /* 2 NOPs */ );
}


static int CB_39( void )   /* SRL C */
{
    FLAGS = ( UBYTE )( RegC & FLAG_C );
    RegC = ( UBYTE )( RegC >> 1 );
    FLAGS |= Parite[ RegC ];
    if ( CBIndex )
        POKE8( RegHL, RegC );

    return( 2 /* 2 NOPs */ );
}


static int CB_3A( void )   /* SRL D */
{
    FLAGS = ( UBYTE )( RegD & FLAG_C );
    RegD = ( UBYTE )( RegD >> 1 );
    FLAGS |= Parite[ RegD ];
    if ( CBIndex )
        POKE8( RegHL, RegD );

    return( 2 /* 2 NOPs */ );
}


static int CB_3B( void )   /* SRL E */
{
    FLAGS = ( UBYTE )( RegE & FLAG_C );
    RegE = ( UBYTE )( RegE >> 1 );
    FLAGS |= Parite[ RegE ];
    if ( CBIndex )
        POKE8( RegHL, RegE );

    return( 2 /* 2 NOPs */ );
}


static int CB_3C( void )   /* SRL H */
{
    FLAGS = ( UBYTE )( RegH & FLAG_C );
    RegH = ( UBYTE )( RegH >> 1 );
    FLAGS |= Parite[ RegH ];
    if ( CBIndex )
        POKE8( RegHL, RegH );

    return( 2 /* 2 NOPs */ );
}


static int CB_3D( void )   /* SRL L */
{
    FLAGS = ( UBYTE )( RegL & FLAG_C );
    RegL = ( UBYTE )( RegL >> 1 );
    FLAGS |= Parite[ RegL ];
    if ( CBIndex )
        POKE8( RegHL, RegL );

    return( 2 /* 2 NOPs */ );
}


static int CB_3E( void )   /* SRL (HL) */
{
    int hl = PEEK8( RegHL );
    FLAGS = ( UBYTE )( hl & FLAG_C );
    POKE8( RegHL, ( UBYTE )( hl = hl >> 1 ) );
    FLAGS |= Parite[ hl ];
    return( 4 /* 4 NOPs */ );
}


static int CB_3F( void )   /* SRL A */
{
    FLAGS = ( UBYTE )( RegA & FLAG_C );
    RegA = ( UBYTE )( RegA >> 1 );
    FLAGS |= Parite[ RegA ];
    if ( CBIndex )
        POKE8( RegHL, RegA );

    return( 2 /* 2 NOPs */ );
}


static int CB_40( void )      /* BIT 0, B */
{
    Bit( RegB, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_41( void )      /* BIT 0, C */
{
    Bit( RegC, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_42( void )      /* BIT 0, D */
{
    Bit( RegD, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_43( void )      /* BIT 0, E */
{
    Bit( RegE, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_44( void )      /* BIT 0, H */
{
    Bit( RegH, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_45( void )      /* BIT 0, L */
{
    Bit( RegL, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_46( void )      /* BIT 0, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT0 );
    return( 3 /* 3 NOPs */ );
}


static int CB_47( void )      /* BIT 0, A */
{
    Bit( RegA, BIT0 );
    return( 2 /* 2 NOPs */ );
}


static int CB_48( void )      /* BIT 1, B */
{
    Bit( RegB, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_49( void )      /* BIT 1, C */
{
    Bit( RegC, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_4A( void )      /* BIT 1, D */
{
    Bit( RegD, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_4B( void )      /* BIT 1, E */
{
    Bit( RegE, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_4C( void )      /* BIT 1, H */
{
    Bit( RegH, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_4D( void )      /* BIT 1, L */
{
    Bit( RegL, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_4E( void )      /* BIT 1, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT1 );
    return( 3 /* 3 NOPs */ );
}


static int CB_4F( void )      /* BIT 1, A */
{
    Bit( RegA, BIT1 );
    return( 2 /* 2 NOPs */ );
}


static int CB_50( void )      /* BIT 2, B */
{
    Bit( RegB, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_51( void )      /* BIT 2, C */
{
    Bit( RegC, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_52( void )      /* BIT 2, D */
{
    Bit( RegD, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_53( void )      /* BIT 2, E */
{
    Bit( RegE, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_54( void )      /* BIT 2, H */
{
    Bit( RegH, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_55( void )      /* BIT 2, L */
{
    Bit( RegL, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_56( void )      /* BIT 2, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT2 );
    return( 3 /* 3 NOPs */ );
}


static int CB_57( void )      /* BIT 2, A */
{
    Bit( RegA, BIT2 );
    return( 2 /* 2 NOPs */ );
}


static int CB_58( void )      /* BIT 3, B */
{
    Bit( RegB, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_59( void )      /* BIT 3, C */
{
    Bit( RegC, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_5A( void )      /* BIT 3, D */
{
    Bit( RegD, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_5B( void )      /* BIT 3, E */
{
    Bit( RegE, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_5C( void )      /* BIT 3, H */
{
    Bit( RegH, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_5D( void )      /* BIT 3, L */
{
    Bit( RegL, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_5E( void )      /* BIT 3, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT3 );
    return( 3 /* 3 NOPs */ );
}


static int CB_5F( void )      /* BIT 3, A */
{
    Bit( RegA, BIT3 );
    return( 2 /* 2 NOPs */ );
}


static int CB_60( void )      /* BIT 4, B */
{
    Bit( RegB, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_61( void )      /* BIT 4, C */
{
    Bit( RegC, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_62( void )      /* BIT 4, D */
{
    Bit( RegD, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_63( void )      /* BIT 4, E */
{
    Bit( RegE, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_64( void )      /* BIT 4, H */
{
    Bit( RegH, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_65( void )      /* BIT 4, L */
{
    Bit( RegL, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_66( void )      /* BIT 4, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT4 );
    return( 3 /* 3 NOPs */ );
}


static int CB_67( void )      /* BIT 4, A */
{
    Bit( RegA, BIT4 );
    return( 2 /* 2 NOPs */ );
}


static int CB_68( void )      /* BIT 5, B */
{
    Bit( RegB, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_69( void )      /* BIT 5, C */
{
    Bit( RegC, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_6A( void )      /* BIT 5, D */
{
    Bit( RegD, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_6B( void )      /* BIT 5, E */
{
    Bit( RegE, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_6C( void )      /* BIT 5, H */
{
    Bit( RegH, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_6D( void )      /* BIT 5, L */
{
    Bit( RegL, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_6E( void )      /* BIT 5, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT5 );
    return( 3 /* 3 NOPs */ );
}


static int CB_6F( void )      /* BIT 5, A */
{
    Bit( RegA, BIT5 );
    return( 2 /* 2 NOPs */ );
}


static int CB_70( void )      /* BIT 6, B */
{
    Bit( RegB, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_71( void )      /* BIT 6, C */
{
    Bit( RegC, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_72( void )      /* BIT 6, D */
{
    Bit( RegD, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_73( void )      /* BIT 6, E */
{
    Bit( RegE, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_74( void )      /* BIT 6, H */
{
    Bit( RegH, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_75( void )      /* BIT 6, L */
{
    Bit( RegL, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_76( void )      /* BIT 6, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT6 );
    return( 3 /* 3 NOPs */ );
}


static int CB_77( void )      /* BIT 6, A */
{
    Bit( RegA, BIT6 );
    return( 2 /* 2 NOPs */ );
}


static int CB_78( void )      /* BIT 7, B */
{
    Bit( RegB, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_79( void )      /* BIT 7, C */
{
    Bit( RegC, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_7A( void )      /* BIT 7, D */
{
    Bit( RegD, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_7B( void )      /* BIT 7, E */
{
    Bit( RegE, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_7C( void )      /* BIT 7, H */
{
    Bit( RegH, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_7D( void )      /* BIT 7, L */
{
    Bit( RegL, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_7E( void )      /* BIT 7, (HL) */
{
    Bit( ( UBYTE )PEEK8( RegHL ), BIT7 );
    return( 3 /* 3 NOPs */ );
}


static int CB_7F( void )      /* BIT 7, A */
{
    Bit( RegA, BIT7 );
    return( 2 /* 2 NOPs */ );
}


static int CB_80( void )      /* RES 0, B */
{
    RegB &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_81( void )      /* RES 0, C */
{
    RegC &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_82( void )      /* RES 0, D */
{
    RegD &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_83( void )      /* RES 0, E */
{
    RegE &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_84( void )      /* RES 0, H */
{
    RegH &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_85( void )      /* RES 0, L */
{
    RegL &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_86( void )      /* RES 0, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT0 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_87( void )      /* RES 0, A */
{
    RegA &= ~BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_88( void )      /* RES 1, B */
{
    RegB &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_89( void )      /* RES 1, C */
{
    RegC &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_8A( void )      /* RES 1, D */
{
    RegD &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_8B( void )      /* RES 1, E */
{
    RegE &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_8C( void )      /* RES 1, H */
{
    RegH &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_8D( void )      /* RES 1, L */
{
    RegL &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_8E( void )      /* RES 1, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT1 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_8F( void )      /* RES 1, A */
{
    RegA &= ~BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_90( void )      /* RES 2, B */
{
    RegB &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_91( void )      /* RES 2, C */
{
    RegC &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_92( void )      /* RES 2, D */
{
    RegD &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_93( void )      /* RES 2, E */
{
    RegE &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_94( void )      /* RES 2, H */
{
    RegH &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_95( void )      /* RES 2, L */
{
    RegL &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_96( void )      /* RES 2, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT2 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_97( void )      /* RES 2, A */
{
    RegA &= ~BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_98( void )      /* RES 3, B */
{
    RegB &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_99( void )      /* RES 3, C */
{
    RegC &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_9A( void )      /* RES 3, D */
{
    RegD &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_9B( void )      /* RES 3, E */
{
    RegE &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_9C( void )      /* RES 3, H */
{
    RegH &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_9D( void )      /* RES 3, L */
{
    RegL &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_9E( void )      /* RES 3, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT3 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_9F( void )      /* RES 3, A */
{
    RegA &= ~BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_A0( void )      /* RES 4, B */
{
    RegB &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A1( void )      /* RES 4, C */
{
    RegC &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A2( void )      /* RES 4, D */
{
    RegD &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A3( void )      /* RES 4, E */
{
    RegE &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A4( void )      /* RES 4, H */
{
    RegH &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A5( void )      /* RES 4, L */
{
    RegL &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A6( void )      /* RES 4, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT4 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_A7( void )      /* RES 4, A */
{
    RegA &= ~BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_A8( void )      /* RES 5, B */
{
    RegB &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_A9( void )      /* RES 5, C */
{
    RegC &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_AA( void )      /* RES 5, D */
{
    RegD &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_AB( void )      /* RES 5, E */
{
    RegE &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_AC( void )      /* RES 5, H */
{
    RegH &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_AD( void )      /* RES 5, L */
{
    RegL &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_AE( void )      /* RES 5, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT5 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_AF( void )      /* RES 5, A */
{
    RegA &= ~BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_B0( void )      /* RES 6, B */
{
    RegB &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B1( void )      /* RES 6, C */
{
    RegC &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B2( void )      /* RES 6, D */
{
    RegD &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B3( void )      /* RES 6, E */
{
    RegE &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B4( void )      /* RES 6, H */
{
    RegH &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B5( void )      /* RES 6, L */
{
    RegL &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B6( void )      /* RES 6, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT6 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_B7( void )      /* RES 6, A */
{
    RegA &= ~BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_B8( void )      /* RES 7, B */
{
    RegB &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_B9( void )      /* RES 7, C */
{
    RegC &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_BA( void )      /* RES 7, D */
{
    RegD &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_BB( void )      /* RES 7, E */
{
    RegE &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_BC( void )      /* RES 7, H */
{
    RegH &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_BD( void )      /* RES 7, L */
{
    RegL &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_BE( void )      /* RES 7, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) & ~BIT7 ) );
    return( 4 /* 4 NOPs */ );
}


static int CB_BF( void )      /* RES 7, A */
{
    RegA &= ~BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_C0( void )      /* SET 0, B */
{
    RegB |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C1( void )      /* SET 0, C */
{
    RegC |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C2( void )      /* SET 0, D */
{
    RegD |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C3( void )      /* SET 0, E */
{
    RegE |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C4( void )      /* SET 0, H */
{
    RegH |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C5( void )      /* SET 0, L */
{
    RegL |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C6( void )      /* SET 0, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT0 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_C7( void )      /* SET 0, A */
{
    RegA |= BIT0;
    return( 2 /* 2 NOPs */ );
}


static int CB_C8( void )      /* SET 1, B */
{
    RegB |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_C9( void )      /* SET 1, C */
{
    RegC |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_CA( void )      /* SET 1, D */
{
    RegD |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_CB( void )      /* SET 1, E */
{
    RegE |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_CC( void )      /* SET 1, H */
{
    RegH |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_CD( void )      /* SET 1, L */
{
    RegL |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_CE( void )      /* SET 1, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT1 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_CF( void )      /* SET 1, A */
{
    RegA |= BIT1;
    return( 2 /* 2 NOPs */ );
}


static int CB_D0( void )      /* SET 2, B */
{
    RegB |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D1( void )      /* SET 2, C */
{
    RegC |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D2( void )      /* SET 2, D */
{
    RegD |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D3( void )      /* SET 2, E */
{
    RegE |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D4( void )      /* SET 2, H */
{
    RegH |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D5( void )      /* SET 2, L */
{
    RegL |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D6( void )      /* SET 2, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT2 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_D7( void )      /* SET 2, A */
{
    RegA |= BIT2;
    return( 2 /* 2 NOPs */ );
}


static int CB_D8( void )      /* SET 3, B */
{
    RegB |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_D9( void )      /* SET 3, C */
{
    RegC |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_DA( void )      /* SET 3, D */
{
    RegD |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_DB( void )      /* SET 3, E */
{
    RegE |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_DC( void )      /* SET 3, H */
{
    RegH |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_DD( void )      /* SET 3, L */
{
    RegL |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_DE( void )      /* SET 3, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT3 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_DF( void )      /* SET 3, A */
{
    RegA |= BIT3;
    return( 2 /* 2 NOPs */ );
}


static int CB_E0( void )      /* SET 4, B */
{
    RegB |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E1( void )      /* SET 4, C */
{
    RegC |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E2( void )      /* SET 4, D */
{
    RegD |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E3( void )      /* SET 4, E */
{
    RegE |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E4( void )      /* SET 4, H */
{
    RegH |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E5( void )      /* SET 4, L */
{
    RegL |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E6( void )      /* SET 4, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT4 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_E7( void )      /* SET 4, A */
{
    RegA |= BIT4;
    return( 2 /* 2 NOPs */ );
}


static int CB_E8( void )      /* SET 5, B */
{
    RegB |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_E9( void )      /* SET 5, C */
{
    RegC |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_EA( void )      /* SET 5, D */
{
    RegD |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_EB( void )      /* SET 5, E */
{
    RegE |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_EC( void )      /* SET 5, H */
{
    RegH |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_ED( void )      /* SET 5, L */
{
    RegL |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_EE( void )      /* SET 5, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT5 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_EF( void )      /* SET 5, A */
{
    RegA |= BIT5;
    return( 2 /* 2 NOPs */ );
}


static int CB_F0( void )      /* SET 6, B */
{
    RegB |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F1( void )      /* SET 6, C */
{
    RegC |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F2( void )      /* SET 6, D */
{
    RegD |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F3( void )      /* SET 6, E */
{
    RegE |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F4( void )      /* SET 6, H */
{
    RegH |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F5( void )      /* SET 6, L */
{
    RegL |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F6( void )      /* SET 6, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT6 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_F7( void )      /* SET 6, A */
{
    RegA |= BIT6;
    return( 2 /* 2 NOPs */ );
}


static int CB_F8( void )      /* SET 7, B */
{
    RegB |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_F9( void )      /* SET 7, C */
{
    RegC |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_FA( void )      /* SET 7, D */
{
    RegD |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_FB( void )      /* SET 7, E */
{
    RegE |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_FC( void )      /* SET 7, H */
{
    RegH |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_FD( void )      /* SET 7, L */
{
    RegL |= BIT7;
    return( 2 /* 2 NOPs */ );
}


static int CB_FE( void )      /* SET 7, ( HL ) */
{
    POKE8( RegHL, ( UBYTE )( PEEK8( RegHL ) | BIT7 ) );
    return( 3 /* 3 NOPs */ );
}


static int CB_FF( void )      /* SET 7, A */
{
    RegA |= BIT7;
    return( 2 /* 2 NOPs */ );
}


/************
* OPCODE ED *
************/


/********************************************************* !NAME! **************
* Nom : ed___
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ex�cut�e sur rencontre d'une instruction ED XX inconnue
*
* R�sultat    : 0
*
********************************************************** !0! ****************/
static int ed___( void )
{
#ifdef USE_LOG
    sprintf( MsgLog
           , "Instruction ED%02X � l'adresse %04X rencontr�e."
           , PEEK8( ( USHORT )( RegPC - 1 ) )
           , RegPC - 2
           );
    Log( MsgLog, LOG_WARNING );
#endif

    return( 2 );
}


static int ED_40( void ) /* IN B, ( C ) */
{
    
    RegB = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegB ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_41( void ) /* OUT ( C ), B */
{
    WritePort( RegBC, RegB );
    return( 4 /* 4 NOPs */ );
}


static int ED_42( void ) /* SBC HL, BC */
{
    SBC_R16( RegBC );
    return( 4 /* 4 NOPs */ );
}


static int ED_43( void ) /* LD ( nnnn ), BC */
{
    POKE16( PEEK16( RegPC ), RegBC );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_44( void ) /* NEG */
{
    int t = - RegA;

    FLAGS = ( UBYTE )( ( RegA & t & 0x80 ? FLAG_V : 0 )
                     | FLAG_N
                     | - ( t >> 8 )
                     | ( t & FLAG_S )
                     | ( ( t & 0xFF ) ? 0 : FLAG_Z )
                     | ( ( RegA ^ t ) & FLAG_H )
                   );
    RegA = ( UBYTE )t;
    return( 2 /* 2 NOPs */ );
}


static int ED_45( void ) /* RETN */
{
    Z80.IFF1 = Z80.IFF2;
#ifdef HACK_IRQ
    VerifyIRQ();
#endif
    
    RegPC = PEEK16( RegSP );
    RegSP += 2;
    return( 4 /* 4 NOPs */ );
}


static int ED_46( void ) /* IM 0 */
{
    Z80.InterruptMode = 0;
    ed___();
    return( 2 /* 2 NOPs */ );
}


static int ED_47( void ) /* LD I, A */
{
    RegI = RegA;
    return( 3 /* 3 NOPs */ );
}


static int ED_48( void ) /* IN C, ( C ) */
{
    RegC = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegC ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_49( void ) /* OUT ( C ), C */
{
    WritePort( RegBC, RegC );
    return( 4 /* 4 NOPs */ );
}


static int ED_4A( void ) /* ADC HL, BC */
{
    ADC_R16( RegBC );
    return( 5 /* 5 NOPs */ );
}


static int ED_4B( void ) /* LD BC, ( nnnn ) */
{
    RegBC = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_4D( void ) /* RETI */
{
    Z80.IFF1 = Z80.IFF2;
#ifdef HACK_IRQ
    VerifyIRQ();
#endif    
    RegPC = PEEK16( RegSP );
    RegSP += 2;
    return( 4 /* 4 NOPs */ );
}


static int ED_4F( void ) /* LD R, A */
{
    RegR = RegA;
    return( 3 /* 3 NOPs */ );
}


static int ED_50( void ) /* IN D, ( C ) */
{
    RegD = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegD ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_51( void ) /* OUT ( C ), D */
{
    WritePort( RegBC, RegD );
    return( 4 /* 4 NOPs */ );
}


static int ED_52( void ) /* SBC HL, DE */
{
    SBC_R16( RegDE );
    return( 4 /* 4 NOPs */ );
}


static int ED_53( void ) /* LD ( nnnn ), DE */
{
    POKE16( PEEK16( RegPC ), RegDE );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_56( void ) /* IM 1 */
{
    Z80.InterruptMode = 1;
    return( 2 /* 2 NOPs */ );
}


static int ED_57( void ) /* LD A, I */
{
    RegA = RegI;
    FLAGS =  (UBYTE)((FLAGS & FLAG_C)
               | (RegA ? (RegA & FLAG_S) : FLAG_Z)
               | (RegA & (BIT5|BIT3))
               | (Z80.IFF2 & 1 ? FLAG_V : 0)
               ); 

    return( 3 /* 3 NOPs */ );
}


static int ED_58( void ) /* IN E, ( C ) */
{
    RegE = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegE ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_59( void ) /* OUT ( C ), E */
{
    WritePort( RegBC, RegE );
    return( 4 /* 4 NOPs */ );
}


static int ED_5A( void ) /* ADC HL, DE */
{
    ADC_R16( RegDE );
    return( 4 /* 4 NOPs */ );
}


static int ED_5B( void ) /* LD DE, ( nnnn ) */
{
    RegDE = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_5E( void ) /* IM 2 */
{
    Z80.InterruptMode = 2;
    return( 2 /* 2 NOPs */ );
}


static int ED_5F( void ) /* LD A, R */
{
    RegA = RegR;
    FLAGS =  (UBYTE)((FLAGS & FLAG_C)
               | (RegA ? (RegA & FLAG_S) : FLAG_Z)
               | (RegA & (BIT5|BIT3))
               | (Z80.IFF2 & 1 ? FLAG_V : 0)
               ); 

    return( 3 /* 3 NOPs */ );
}


static int ED_60( void ) /* IN H, ( C ) */
{
    RegH = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegH ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_61( void ) /* OUT ( C ), H */
{
    WritePort( RegBC, RegH );
    return( 4 /* 4 NOPs */ );
}


static int ED_62( void ) /* SBC HL, HL */
{
    SBC_R16( RegHL );
    return( 4 /* 4 NOPs */ );
}


static int ED_63( void ) /* LD ( nnnn ), HL */
{
    POKE16( PEEK16( RegPC ), RegHL );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_67( void ) /* RRD */
{
    int a = RegA;
    int hl = PEEK8( RegHL );

    RegA = ( UBYTE )( ( a & 0xF0 ) | ( hl & 0xF ) );
    POKE8( RegHL, ( UBYTE )( ( hl >> 4 ) | ( a << 4 ) ) );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegA ] );
    return( 5 /* 5 NOPs */ );
}


static int ED_68( void ) /* IN L, ( C ) */
{
    RegL = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegL ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_69( void ) /* OUT ( C ), L */
{
    WritePort( RegBC, RegL );
    return( 4 /* 4 NOPs */ );
}


static int ED_6A( void ) /* ADC HL, HL */
{
    ADC_R16( RegHL );
    return( 4 /* 4 NOPs */ );
}


static int ED_6B( void ) /* LD HL, ( nnnn ) */
{
    RegHL = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_6F( void ) /* RLD */
{
    int a = RegA;
    int hl = PEEK8( RegHL );

    RegA = ( UBYTE )( ( a & 0xF0 ) | ( hl >> 4 ) );
    POKE8( RegHL, ( UBYTE )( ( hl << 4 ) | ( a & 0xF ) ) );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegA ] );
    return( 5 /* 5 NOPs */ );
}


static int ED_70( void ) /* IN F, ( C ) */
{
    ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegL ] );
    ed___();
    return( 4 /* 4 NOPs */ );
}


static int ED_71( void ) /* OUT ( C ), 0 */
{
    WritePort( RegBC, 0 );
    return( 4 /* 4 NOPs */ );
}


static int ED_72( void ) /* SBC HL, SP */
{
    SBC_R16( RegSP );
    return( 4 /* 4 NOPs */ );
}


static int ED_73( void ) /* LD ( nnnn ), SP */
{
    POKE16( PEEK16( RegPC ), RegSP );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_78( void ) /* IN A, ( C ) */
{
    RegA = ( UBYTE )ReadPort( RegBC );
    FLAGS = ( UBYTE )( ( FLAGS & FLAG_C ) | Parite[ RegA ] );
    return( 4 /* 4 NOPs */ );
}


static int ED_79( void ) /* OUT ( C ), A */
{
    WritePort( RegBC, RegA );
    return( 4 /* 4 NOPs */ );
}


static int ED_7A( void ) /* ADC HL, SP */
{
    ADC_R16( RegSP );
    return( 4 /* 4 NOPs */ );
}


static int ED_7B( void ) /* LD SP, ( nnnn ) */
{
    RegSP = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int ED_A0( void ) /* LDI */
{
    POKE8( RegDE++, ( UBYTE )PEEK8( RegHL++ ) );
    FLAGS &= ~FLAG_H & ~FLAG_V & ~FLAG_N;
    if ( --RegBC )
        FLAGS |= FLAG_V;

    return( 5 /* 5 NOPs */ );
}


static int ED_A1( void ) /* CPI */
{
    FLAGS &= ~FLAG_Z & ~FLAG_V;

    if ( RegA == PEEK8( RegHL++ ) )
        FLAGS |= FLAG_Z;

    if ( --RegBC )
        FLAGS |= FLAG_V;

    FLAGS |= FLAG_N;
    return( 5 /* 5 NOPs */ );
}


static int ED_A2( void ) /* INI */
{
    POKE8( RegHL++, ( UBYTE )ReadPort( RegBC ) );
/* #### A v�rifier : flags #### */
    FLAGS = FLAG_N;
    if ( --RegB )
        FLAGS &= ~FLAG_Z;
    else
        FLAGS |= FLAG_Z;

    return( 5 /* 5 NOPs */ );
}


static int ED_A3( void ) /* OUTI */
{
    FLAGS = FLAG_N;
/* #### A v�rifier : flags #### */
    if ( --RegB )
        FLAGS &= ~FLAG_Z;
    else
        FLAGS |= FLAG_Z;

    WritePort( RegBC, PEEK8( RegHL++ ) );
    return( 5 /* 5 NOPs */ );
}


static int ED_A8( void ) /* LDD */
{
    POKE8( RegDE--, ( UBYTE )PEEK8( RegHL-- ) );
    FLAGS &= ~FLAG_H & ~FLAG_V & ~FLAG_N;
    if ( --RegBC )
        FLAGS |= FLAG_V;

    return( 5 /* 5 NOPs */ );
}


static int ED_A9( void ) /* CPD */
{
    FLAGS &= ~FLAG_Z & ~FLAG_V;

    if ( RegA == PEEK8( RegHL-- ) )
        FLAGS |= FLAG_Z;

    if ( --RegBC )
        FLAGS |= FLAG_V;

    FLAGS |= FLAG_N;
    return( 5 /* 5 NOPs */ );
}


static int ED_AA( void ) /* IND */
{
    FLAGS = FLAG_N;
    POKE8( RegHL--, ( UBYTE )ReadPort( RegBC ) );
/* #### A v�rifier : flags #### */
    if ( --RegB )
        FLAGS &= ~FLAG_Z;
    else
        FLAGS |= FLAG_Z;

    return( 5 /* 5 NOPs */ );
}


static int ED_AB( void ) /* OUTD */
{
/* #### A v�rifier : flags #### */
    FLAGS = FLAG_N;
    if ( --RegB )
        FLAGS &= ~FLAG_Z;
    else
        FLAGS |= FLAG_Z;

    WritePort( RegBC, PEEK8( RegHL-- ) );
    return( 5 /* 5 NOPs */ );
}


#ifdef HACK_LDIR
static int ED_B0( void ) /* LDIR */
{
    int r = 5;
    r+=RegBC;

    do {
        POKEPEEK8( RegDE++, RegHL++ );
        FLAGS &= ~FLAG_H & ~FLAG_V & ~FLAG_N;
        RegBC--;
    } while (RegBC!=0);
    
    return( r );
}
#else
static int ED_B0( void ) /* LDIR */
{
    int r = 5;

    POKE8( RegDE++, ( UBYTE )PEEK8( RegHL++ ) );
    FLAGS &= ~FLAG_H & ~FLAG_V & ~FLAG_N;
    if (--RegBC) {
        RegPC -= 2;
        FLAGS |= FLAG_V;
        r++;
    }
    
    return( r );
}
#endif


static int ED_B1( void ) /* CPIR */
{
    int r = 5;
    UBYTE i = ( UBYTE )PEEK8( RegHL++ );
    UBYTE tmp = ( UBYTE )( RegA - i );
    RegBC--;
    FLAGS = ( UBYTE )( FLAG_N
                     | ( FLAGS & FLAG_C )
                     | ( tmp & FLAG_S )
                     | ( tmp ? 0 : FLAG_Z )
                     | ( ( RegA ^ i ^ tmp ) &FLAG_H )
                     | ( RegBC ? FLAG_V : 0 )
                     );
    if ( RegBC && tmp )
        {
        r++;
        RegPC -= 2;

        }
    return( r );
}


static int ED_B2( void ) /* INIR */
{
    return( ed___() );
}


static int ED_B3( void ) /* OTIR */
{
    return( ed___() );
}


static int ED_B8( void ) /* LDDR */
{
    int r = 5;
    POKE8( RegDE--, ( UBYTE )PEEK8( RegHL-- ) );
    FLAGS &= ~FLAG_H & ~FLAG_V & ~FLAG_N;
    if ( --RegBC )
        {
        FLAGS |= FLAG_V;
        r++;
        RegPC -= 2;
        }
    return( r );
}


static int ED_B9( void ) /* CPDR */
{
    int r = 5;
    UBYTE i = ( UBYTE )PEEK8( RegHL-- );
    UBYTE tmp = ( UBYTE )( RegA - i );
    RegBC--;
    FLAGS = ( UBYTE )( FLAG_N
                     | ( FLAGS & FLAG_C )
                     | ( tmp & FLAG_S )
                     | ( tmp ? 0 : FLAG_Z )
                     | ( ( RegA ^ i ^ tmp ) &FLAG_H )
                     | ( RegBC ? FLAG_V : 0 )
                     );
    if ( RegBC && tmp )
        {
        r++;
        RegPC -= 2;

        }
    return( r );
}


static int ED_BA( void ) /* INDR */
{
    return( ed___() );
}


static int ED_BB( void ) /* OTDR */
{
    return( ed___() );
}


/************
* OPCODE DD *
************/


/********************************************************* !NAME! **************
* Nom : dd___
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ex�cut�e sur rencontre d'une instruction DD XX inconnue
*
* R�sultat    : 0
*
********************************************************** !0! ****************/
static int dd___( void )
{
#ifdef USE_LOG
    sprintf( MsgLog
           , "Instruction DD%02X � l'adresse %04X rencontr�e."
           , PEEK8( ( USHORT )( RegPC - 1 ) )
           , RegPC - 2
           );
    Log( MsgLog, LOG_WARNING );
#endif

    RegPC++;
    return( 2 );
}


static USHORT GetIXdd( void )
{
    return( USHORT )( RegIX + ( signed char )PEEK8( RegPC++ ) );
}


static int DD_09( void ) /* ADD IX, BC */
{
    ADD_R16( &RegIX, RegBC );
    return( 4 /* 4 NOPs */ );
}


static int DD_19( void ) /* ADD IX, DE */
{
    ADD_R16( &RegIX, RegDE );
    return( 4 /* 4 NOPs */ );
}


static int DD_21( void ) /* LD IX, nnnn */
{
    RegIX = PEEK16( RegPC );
    RegPC += 2;
    return( 4 /* 4 NOPs */ );
}


static int DD_22( void ) /* LD ( nnnn ), IX */
{
    POKE16( PEEK16( RegPC ), RegIX );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int DD_23( void ) /* INC IX */
{
    ++RegIX;
    return( 3 /* 3 NOPs */ );
}


static int DD_24( void ) /* INC IXh */
{
    FLAG_INC( ++RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_25( void ) /* DEC IXh */
{
    FLAG_DEC( --RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_26( void ) /* LD IXh, n */
{
    RegIXH = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int DD_29( void ) /* ADD IX, IX */
{
    ADD_R16( &RegIX, RegIX );
    return( 4 /* 4 NOPs */ );
}


static int DD_2A( void ) /* LD IX, ( nnnn ) */
{
    RegIX = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int DD_2B( void ) /* DEC IX */
{
    --RegIX;
    return( 3 /* 3 NOPs */ );
}


static int DD_2C( void ) /* INC IXl */
{
    FLAG_INC( ++RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_2D( void ) /* DEC IXl */
{
    FLAG_DEC( --RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_2E( void ) /* LD IXl, n */
{
    RegIXL = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int DD_34( void ) /* INC (IX+n) */
{
    USHORT ofs = GetIXdd();
    UBYTE r = ( UBYTE )PEEK8( ofs );
    FLAG_INC( ++r );
    POKE8( ofs, r );
    return( 6 /* 6 NOPs */ );
}


static int DD_35( void ) /* DEC (IX+n) */
{
    USHORT ofs = GetIXdd();
    UBYTE r = ( UBYTE )PEEK8( ofs );
    FLAG_DEC( --r );
    POKE8( ofs, r );
    return( 6 /* 6 NOPs */ );
}


static int DD_36( void ) /* LD (IX+d), n */
{
    USHORT ofs = GetIXdd();
    POKE8( ofs, ( UBYTE )PEEK8( RegPC ) );
    RegPC++;
    return( 6 /* 6 NOPs */ );
}


static int DD_39( void ) /* ADD IX, SP */
{
    ADD_R16( &RegIX, RegSP );
    return( 4 /* 4 NOPs */ );
}


static int DD_44( void ) /* LD B, IXh */
{
    RegB = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_45( void ) /* LD B, IXl */
{
    RegB = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_46( void ) /* LD B, (IX+d) */
{
    RegB = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_4C( void ) /* LD C, IXh */
{
    RegC = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_4D( void ) /* LD C, IXl */
{
    RegC = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_4E( void ) /* LD C, (IX+d) */
{
    RegC = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_54( void ) /* LD D, IXh */
{
    RegD = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_55( void ) /* LD D, IXl */
{
    RegD = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_56( void ) /* LD D, (IX+d) */
{
    RegD = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_5C( void ) /* LD E, IXh */
{
    RegE = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_5D( void ) /* LD E, IXl */
{
    RegE = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_5E( void ) /* LD E, (IX+d) */
{
    RegE = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_60( void ) /* LD IXh, B */
{
    RegIXH = RegB;
    return( 2 /* 2 NOPs */ );
}


static int DD_61( void ) /* LD IXh, C */
{
    RegIXH = RegC;
    return( 2 /* 2 NOPs */ );
}


static int DD_62( void ) /* LD IXh, D */
{
    RegIXH = RegD;
    return( 2 /* 2 NOPs */ );
}


static int DD_63( void ) /* LD IXh, E */
{
    RegIXH = RegE;
    return( 2 /* 2 NOPs */ );
}


static int DD_65( void ) /* LD IXh, IXl */
{
    RegIXH = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_66( void ) /* LD H, (IX+d) */
{
    RegH = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_67( void ) /* LD IXh, A */
{
    RegIXH = RegA;
    return( 2 /* 2 NOPs */ );
}


static int DD_68( void ) /* LD IXl, B */
{
    RegIXL = RegB;
    return( 2 /* 2 NOPs */ );
}


static int DD_69( void ) /* LD IXl, C */
{
    RegIXL = RegC;
    return( 2 /* 2 NOPs */ );
}


static int DD_6A( void ) /* LD IXl, D */
{
    RegIXL = RegD;
    return( 2 /* 2 NOPs */ );
}


static int DD_6B( void ) /* LD IXl, E */
{
    RegIXL = RegE;
    return( 2 /* 2 NOPs */ );
}


static int DD_6C( void ) /* LD IXl, IXH */
{
    RegIXL = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_6E( void ) /* LD L, (IX+d) */
{
    RegL = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_6F( void ) /* LD IXl, A */
{
    RegIXL = RegA;
    return( 2 /* 2 NOPs */ );
}


static int DD_70( void ) /* LD (IX+d), B */
{
    POKE8( GetIXdd(), RegB );
    return( 5 /* 5 NOPs */ );
}


static int DD_71( void ) /* LD (IX+d), C */
{
    POKE8( GetIXdd(), RegC );
    return( 5 /* 5 NOPs */ );
}


static int DD_72( void ) /* LD (IX+d), D */
{
    POKE8( GetIXdd(), RegD );
    return( 5 /* 5 NOPs */ );
}


static int DD_73( void ) /* LD (IX+d), E */
{
    POKE8( GetIXdd(), RegE );
    return( 5 /* 5 NOPs */ );
}


static int DD_74( void ) /* LD (IX+d), H */
{
    POKE8( GetIXdd(), RegH );
    return( 5 /* 5 NOPs */ );
}


static int DD_75( void ) /* LD (IX+d), L */
{
    POKE8( GetIXdd(), RegL );
    return( 5 /* 5 NOPs */ );
}


static int DD_77( void ) /* LD (IX+d), A */
{
    POKE8( GetIXdd(), RegA );
    return( 5 /* 5 NOPs */ );
}


static int DD_7C( void ) /* LD A, IXh */
{
    RegA = RegIXH;
    return( 2 /* 2 NOPs */ );
}


static int DD_7D( void ) /* LD A, IXl */
{
    RegA = RegIXL;
    return( 2 /* 2 NOPs */ );
}


static int DD_7E( void ) /* LD A, (IX+d) */
{
    RegA = ( UBYTE )PEEK8( GetIXdd() );
    return( 5 /* 5 NOPs */ );
}


static int DD_84( void ) /* ADD A, IXh */
{
    ADD_R8( RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_85( void ) /* ADD A, IXl */
{
    ADD_R8( RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_86( void ) /* ADD A, (IX+n) */
{
    ADD_R8( PEEK8( GetIXdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int DD_8C( void ) /* ADC A, IXh */
{
    ADC_R8( RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_8D( void ) /* ADC A, IXl */
{
    ADC_R8( RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_8E( void ) /* ADC A, (IX+n) */
{
    ADC_R8( PEEK8( GetIXdd()  ) );
    return( 5 /* 5 NOPs */ );
}


static int DD_94( void ) /* SUB IXh */
{
    SUB_R8( RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_95( void ) /* SUB IXl */
{
    SUB_R8( RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_96( void ) /* SUB (IX+n) */
{
    SUB_R8( PEEK8( GetIXdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int DD_9C( void ) /* SBC A, IXh */
{
    SBC_R8( RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_9D( void ) /* SBC A, IXl */
{
    SBC_R8( RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_9E( void ) /* SBC A, (IX+n) */
{
    SBC_R8( PEEK8( GetIXdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int DD_A4( void ) /* AND IXh */
{
    RegA &= RegIXH;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 2 /* 2 NOPs */ );
}


static int DD_A5( void ) /* AND IXl */
{
    RegA &= RegIXL;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 2 /* 2 NOPs */ );
}


static int DD_A6( void ) /* AND (IX+n) */
{
    RegA &= PEEK8( GetIXdd() );
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 5 /* 5 NOPs */ );
}


static int DD_AC( void ) /* XOR IXh */
{
    RegA ^= RegIXH;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int DD_AD( void ) /* XOR IXl */
{
    RegA ^= RegIXL;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int DD_AE( void ) /* XOR (IX+n) */
{
    RegA ^= PEEK8( GetIXdd() );
    FLAGS = Parite[ RegA ];
    return( 5 /* 5 NOPs */ );
}


static int DD_B4( void ) /* OR IXh */
{
    RegA |= RegIXH;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int DD_B5( void ) /* OR IXl */
{
    RegA |= RegIXL;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int DD_B6( void ) /* OR (IX+n) */
{
    RegA |= PEEK8( GetIXdd() );
    FLAGS = Parite[ RegA ];
    return( 5 /* 5 NOPs */ );
}


static int DD_BC( void ) /* CP IXh */
{
    CP_R8( RegIXH );
    return( 2 /* 2 NOPs */ );
}


static int DD_BD( void ) /* CP IXl */
{
    CP_R8( RegIXL );
    return( 2 /* 2 NOPs */ );
}


static int DD_BE( void ) /* CP (IX+n) */
{
    CP_R8( PEEK8( GetIXdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int DD_CB( void ) /* special code CB */
{
    int r, tmp = RegHL;
    RegHL = ( USHORT )GetIXdd();
    CBIndex = 1;
    r = tabCB[ PEEK8( RegPC++ ) ]();
    CBIndex = 0;
    RegHL = ( USHORT )tmp;
    return( r + 4 ); // ### a v�rifier...
}


static int DD_E1( void ) /* POP IX */
{
    RegIX = PEEK16( RegSP );
    RegSP += 2;
    return( 5 /* 5 NOPs */ );
}


static int DD_E3( void ) /* EX (SP), IX */
{
    USHORT a = PEEK16( RegSP );
    POKE16( RegSP, RegIX );
    RegIX = a;
    return( 7 /* 7 NOPs */ );
}


static int DD_E5( void ) /* PUSH IX */
{
    RegSP -= 2;
    POKE16( RegSP, RegIX );
    return( 5 /* 5 NOPs */ );
}


static int DD_E9( void ) /* JP (IX) */
{
    RegPC = RegIX;
    return( 2 /* 2 NOPs */ );
}


static int DD_F9( void ) /* LD SP, IX */
{
    RegSP = RegIX;
    return( 3 /* 3 NOPs */ );
}


static int DD_FD( void ) /* special DD_FD */
{
    // Se comporte commme un simple FD
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    return( 1 + tabIY[ PEEK8( RegPC++ ) ]() ); // ### A v�rifier
}



/************
* OPCODE FD *
************/


/********************************************************* !NAME! **************
* Nom : fd___
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Ex�cut�e sur rencontre d'une instruction FD XX inconnue
*
* R�sultat    : 0
*
********************************************************** !0! ****************/
static int fd___( void )
{
#ifdef USE_LOG
    sprintf( MsgLog
           , "Instruction FD%02X � l'adresse %04X rencontr�e."
           , PEEK8( ( USHORT )( RegPC - 1 ) )
           , RegPC - 2
           );
    Log( MsgLog, LOG_WARNING );
#endif

    RegPC++;
    return( 2 );
}


static USHORT GetIYdd( void )
{
    return( USHORT )( RegIY + ( signed char )PEEK8( RegPC++ ) );
}


static int FD_09( void ) /* ADD IY, BC */
{
    ADD_R16( &RegIY, RegBC );
    return( 4 /* 4 NOPs */ );
}


static int FD_19( void ) /* ADD IY, DE */
{
    ADD_R16( &RegIY, RegDE );
    return( 4 /* 4 NOPs */ );
}


static int FD_21( void ) /* LD IY, nnnn */
{
    RegIY = PEEK16( RegPC );
    RegPC += 2;
    return( 4 /* 4 NOPs */ );
}


static int FD_22( void ) /* LD ( nnnn ), IY */
{
    POKE16( PEEK16( RegPC ), RegIY );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int FD_23( void ) /* INC IY */
{
    ++RegIY;
    return( 3 /* 3 NOPs */ );
}


static int FD_24( void ) /* INC IYh */
{
    FLAG_INC( ++RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_25( void ) /* DEC IYh */
{
    FLAG_DEC( --RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_26( void ) /* LD IYh, n */
{
    RegIYH = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int FD_29( void ) /* ADD IY, IY */
{
    ADD_R16( &RegIY, RegIY );
    return( 4 /* 4 NOPs */ );
}


static int FD_2A( void ) /* LD IY, ( nnnn ) */
{
    RegIY = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 6 /* 6 NOPs */ );
}


static int FD_2B( void ) /* DEC IY */
{
    --RegIY;
    return( 3 /* 3 NOPs */ );
}


static int FD_2C( void ) /* INC IYl */
{
    FLAG_INC( ++RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_2D( void ) /* DEC IYl */
{
    FLAG_DEC( --RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_2E( void ) /* LD IYl, n */
{
    RegIYL = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int FD_34( void ) /* INC (IY+n) */
{
    USHORT ofs = GetIYdd();
    UBYTE r = ( UBYTE )PEEK8( ofs );
    FLAG_INC( ++r );
    POKE8( ofs, r );
    return( 6 /* 6 NOPs */ );
}


static int FD_35( void ) /* DEC (IY+n) */
{
    USHORT ofs = GetIYdd();
    UBYTE r = ( UBYTE )PEEK8( ofs );
    FLAG_DEC( --r );
    POKE8( ofs, r );
    return( 6 /* 6 NOPs */ );
}


static int FD_36( void ) /* LD (IY+d), n */
{
    USHORT ofs = GetIYdd();
    POKE8( ofs, ( UBYTE )PEEK8( RegPC ) );
    RegPC++;
    return( 6 /* 6 NOPs */ );
}


static int FD_39( void ) /* ADD IY, SP */
{
    ADD_R16( &RegIY, RegSP );
    return( 4 /* 4 NOPs */ );
}


static int FD_44( void ) /* LD B, IYh */
{
    RegB = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_45( void ) /* LD B, IYl */
{
    RegB = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_46( void ) /* LD B, (IY+d) */
{
    RegB = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_4C( void ) /* LD C, IYh */
{
    RegC = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_4D( void ) /* LD C, IYl */
{
    RegC = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_4E( void ) /* LD C, (IY+d) */
{
    RegC = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_54( void ) /* LD D, IYh */
{
    RegD = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_55( void ) /* LD D, IYl */
{
    RegD = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_56( void ) /* LD D, (IY+d) */
{
    RegD = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_5C( void ) /* LD E, IYh */
{
    RegE = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_5D( void ) /* LD E, IYl */
{
    RegE = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_5E( void ) /* LD E, (IY+d) */
{
    RegE = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_60( void ) /* LD IYh, B */
{
    RegIYH = RegB;
    return( 2 /* 2 NOPs */ );
}


static int FD_61( void ) /* LD IYh, C */
{
    RegIYH = RegC;
    return( 2 /* 2 NOPs */ );
}


static int FD_62( void ) /* LD IYh, D */
{
    RegIYH = RegD;
    return( 2 /* 2 NOPs */ );
}


static int FD_63( void ) /* LD IYh, E */
{
    RegIYH = RegE;
    return( 2 /* 2 NOPs */ );
}


static int FD_65( void ) /* LD IYh, IYl */
{
    RegIYH = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_66( void ) /* LD H, (IY+d) */
{
    RegH = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_67( void ) /* LD IYh, A */
{
    RegIYH = RegA;
    return( 2 /* 2 NOPs */ );
}


static int FD_68( void ) /* LD IYl, B */
{
    RegIYL = RegB;
    return( 2 /* 2 NOPs */ );
}


static int FD_69( void ) /* LD IYl, C */
{
    RegIYL = RegC;
    return( 2 /* 2 NOPs */ );
}


static int FD_6A( void ) /* LD IYl, D */
{
    RegIYL = RegD;
    return( 2 /* 2 NOPs */ );
}


static int FD_6B( void ) /* LD IYl, E */
{
    RegIYL = RegE;
    return( 2 /* 2 NOPs */ );
}


static int FD_6C( void ) /* LD IYl, IYH */
{
    RegIYL = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_6E( void ) /* LD L, (IY+d) */
{
    RegL = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_6F( void ) /* LD IYl, A */
{
    RegIYL = RegA;
    return( 2 /* 2 NOPs */ );
}


static int FD_70( void ) /* LD (IY+d), B */
{
    POKE8( GetIYdd(), RegB );
    return( 5 /* 5 NOPs */ );
}


static int FD_71( void ) /* LD (IY+d), C */
{
    POKE8( GetIYdd(), RegC );
    return( 5 /* 5 NOPs */ );
}


static int FD_72( void ) /* LD (IY+d), D */
{
    POKE8( GetIYdd(), RegD );
    return( 5 /* 5 NOPs */ );
}


static int FD_73( void ) /* LD (IY+d), E */
{
    POKE8( GetIYdd(), RegE );
    return( 5 /* 5 NOPs */ );
}


static int FD_74( void ) /* LD (IY+d), H */
{
    POKE8( GetIYdd(), RegH );
    return( 5 /* 5 NOPs */ );
}


static int FD_75( void ) /* LD (IY+d), L */
{
    POKE8( GetIYdd(), RegL );
    return( 5 /* 5 NOPs */ );
}


static int FD_77( void ) /* LD (IY+d), A */
{
    POKE8( GetIYdd(), RegA );
    return( 5 /* 5 NOPs */ );
}


static int FD_7C( void ) /* LD A, IYh */
{
    RegA = RegIYH;
    return( 2 /* 2 NOPs */ );
}


static int FD_7D( void ) /* LD A, IYl */
{
    RegA = RegIYL;
    return( 2 /* 2 NOPs */ );
}


static int FD_7E( void ) /* LD A, (IY+d) */
{
    RegA = ( UBYTE )PEEK8( GetIYdd() );
    return( 5 /* 5 NOPs */ );
}


static int FD_84( void ) /* ADD A, IYh */
{
    ADD_R8( RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_85( void ) /* ADD A, IYl */
{
    ADD_R8( RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_86( void ) /* ADD A, (IY+n) */
{
    ADD_R8( PEEK8( GetIYdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int FD_8C( void ) /* ADC A, IYh */
{
    ADC_R8( RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_8D( void ) /* ADC A, IYl */
{
    ADC_R8( RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_8E( void ) /* ADC A, (IY+n) */
{
    ADC_R8( PEEK8( GetIYdd()  ) );
    return( 5 /* 5 NOPs */ );
}


static int FD_94( void ) /* SUB IYh */
{
    SUB_R8( RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_95( void ) /* SUB IYl */
{
    SUB_R8( RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_96( void ) /* SUB (IY+n) */
{
    SUB_R8( PEEK8( GetIYdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int FD_9C( void ) /* SBC A, IYh */
{
    SBC_R8( RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_9D( void ) /* SBC A, IYl */
{
    SBC_R8( RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_9E( void ) /* SBC A, (IY+n) */
{
    SBC_R8( PEEK8( GetIYdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int FD_A4( void ) /* AND IYh */
{
    RegA &= RegIYH;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 2 /* 2 NOPs */ );
}


static int FD_A5( void ) /* AND IYl */
{
    RegA &= RegIYL;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 2 /* 2 NOPs */ );
}


static int FD_A6( void ) /* AND (IY+n) */
{
    RegA &= PEEK8( GetIYdd() );
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 5 /* 5 NOPs */ );
}


static int FD_AC( void ) /* XOR IYh */
{
    RegA ^= RegIYH;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int FD_AD( void ) /* XOR IYl */
{
    RegA ^= RegIYL;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int FD_AE( void ) /* XOR (IY+n) */
{
    RegA ^= PEEK8( GetIYdd() );
    FLAGS = Parite[ RegA ];
    return( 5 /* 5 NOPs */ );
}


static int FD_B4( void ) /* OR IYh */
{
    RegA |= RegIYH;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int FD_B5( void ) /* OR IYl */
{
    RegA |= RegIYL;
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int FD_B6( void ) /* OR (IY+n) */
{
    RegA |= PEEK8( GetIYdd() );
    FLAGS = Parite[ RegA ];
    return( 5 /* 5 NOPs */ );
}


static int FD_BC( void ) /* CP IYh */
{
    CP_R8( RegIYH );
    return( 2 /* 2 NOPs */ );
}


static int FD_BD( void ) /* CP IYl */
{
    CP_R8( RegIYL );
    return( 2 /* 2 NOPs */ );
}


static int FD_BE( void ) /* CP (IY+n) */
{
    CP_R8( PEEK8( GetIYdd() ) );
    return( 5 /* 5 NOPs */ );
}


static int FD_CB( void ) /* special code CB */
{
    int r, tmp = RegHL;
    RegHL = ( USHORT )GetIYdd();
    CBIndex = 1;
    r = tabCB[ PEEK8( RegPC++ ) ]();
    CBIndex = 0;
    RegHL = ( USHORT )tmp;
    return( r + 4 ); // ### a v�rifier...
}


static int FD_DD( void ) /* special FD_DD */
{
    // Se comporte comme un simple DD
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    return( 1 + tabIX[ PEEK8( RegPC++ ) ]() ); // ### A v�rifier
}


static int FD_E1( void ) /* POP IY */
{
    RegIY = PEEK16( RegSP );
    RegSP += 2;
    return( 5 /* 5 NOPs */ );
}


static int FD_E3( void ) /* EX (SP), IY */
{
    USHORT a = PEEK16( RegSP );
    POKE16( RegSP, RegIY );
    RegIY = a;
    return( 7 /* 7 NOPs */ );
}


static int FD_E5( void ) /* PUSH IY */
{
    RegSP -= 2;
    POKE16( RegSP, RegIY );
    return( 5 /* 5 NOPs */ );
}


static int FD_E9( void ) /* JP (IY) */
{
    RegPC = RegIY;
    return( 2 /* 2 NOPs */ );
}


static int FD_F9( void ) /* LD SP, IY */
{
    RegSP = RegIY;
    return( 3 /* 3 NOPs */ );
}


/*******************
* OPCODE Standards *
*******************/


static int NO_OP( void )
{
    return( 1 /* 1 NOP */ );
}


static int ___01( void ) /* LD BC, nnnn */
{
    RegBC = PEEK16( RegPC );
    RegPC += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___02( void ) /* LD ( BC ), A */
{
    POKE8( RegBC, RegA );
    return( 2 /* 2 NOPs */ );
}


static int ___03( void ) /* INC BC */
{
    RegBC++;
    return( 2 /* 2 NOPs */ );
}


static int ___04( void ) /* INC B */
{
    FLAG_INC( ++RegB );
    return( 1 /* 1 NOP */ );
}


static int ___05( void ) /* DEC B */
{
    FLAG_DEC( --RegB );
    return( 1 /* 1 NOP */ );
}


static int ___06( void ) /* LD B, n */
{
    RegB = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___07( void ) /* RLCA */
{
    FLAGS = ( UBYTE )( ( FLAGS & ( ~FLAG_C & ~FLAG_N & ~FLAG_H ) )
                     | ( RegA >> 7 )
                     );
    RegA = ( UBYTE )( ( RegA << 1 ) | ( FLAGS & FLAG_C ) );
    return( 1 /* 1 NOP */ );
}


static int ___08( void ) /* EX AF, AF' */
{
    USHORT tmp = RegAF;
    RegAF = Reg_AF;
    Reg_AF = tmp;
    return( 1 /* 1 NOP */ );
}


static int ___09( void ) /* ADD HL, BC */
{
    ADD_R16( &RegHL, RegBC );
    return( 3 /* 3 NOPs */ );
}


static int ___0A( void ) /* LD A, ( BC ) */
{
    RegA = ( UBYTE )PEEK8( RegBC );
    return( 2 /* 2 NOPs */ );
}


static int ___0B( void ) /* DEC BC */
{
    RegBC--;
    return( 2 /* 2 NOPs */ );
}


static int ___0C( void ) /* INC C */
{
    FLAG_INC( ++RegC );
    return( 1 /* 1 NOP */ );
}


static int ___0D( void ) /* DEC C */
{
    FLAG_DEC( --RegC );
    return( 1 /* 1 NOP */ );
}


static int ___0E( void ) /* LD C, n */
{
    RegC = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___0F( void ) /* RRCA */
{
    FLAGS = ( UBYTE )( ( FLAGS & ( ~FLAG_C & ~FLAG_N & ~FLAG_H ) )
                     | ( RegA & FLAG_C )
                     );
    RegA = ( UBYTE )( ( RegA >> 1 ) | ( FLAGS << 7 ) );
    return( 1 /* 1 NOP */ );
}


static int ___10( void ) /* DJNZ e */
{
    int r = 3;
    if ( --RegB )
        {
        RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
        r++;
        }
    RegPC++;
    return( r );
}


static int ___11( void ) /* LD DE, nnnn */
{
    RegDE = PEEK16( RegPC );
    RegPC += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___12( void ) /* LD ( DE ), A */
{
    POKE8( RegDE, RegA );
    return( 2 /* 2 NOPs */ );
}


static int ___13( void ) /* INC DE */
{
    RegDE++;
    return( 2 /* 2 NOPs */ );
}


static int ___14( void ) /* INC D */
{
    FLAG_INC( ++RegD );
    return( 1 /* 1 NOP */ );
}


static int ___15( void ) /* DEC D */
{
    FLAG_DEC( --RegD );
    return( 1 /* 1 NOP */ );
}


static int ___16( void ) /* LD D, n */
{
    RegD = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___17( void ) /* RLA */
{
    int i = RegA << 1;
    RegA = ( UBYTE )( i | ( FLAGS & FLAG_C ) );
    FLAGS = ( UBYTE )( ( FLAGS & ( ~FLAG_C & ~FLAG_N & ~FLAG_H ) )
                     | ( i >> 8 )
                     );
    return( 1 /* 1 NOP */ );
}


static int ___18( void ) /* JR e */
{
    RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int ___19( void ) /* ADD HL, DE */
{
    ADD_R16( &RegHL, RegDE );
    return( 3 /* 3 NOPs */ );
}


static int ___1A( void ) /* LD A, ( DE ) */
{
    RegA = ( UBYTE )PEEK8( RegDE );
    return( 2 /* 2 NOPs */ );
}


static int ___1B( void ) /* DEC DE */
{
    RegDE--;
    return( 2 /* 2 NOPs */ );
}


static int ___1C( void ) /* INC E */
{
    FLAG_INC( ++RegE );
    return( 1 /* 1 NOP */ );
}


static int ___1D( void ) /* DEC E */
{
    FLAG_DEC( --RegE );
    return( 1 /* 1 NOP */ );
}


static int ___1E( void ) /* LD E, n */
{
    RegE = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___1F( void ) /* RRA */
{
    int i = ( RegA >> 1 ) | ( ( FLAGS << 7 ) & 128 );
    FLAGS = ( UBYTE )( ( FLAGS & ( ~FLAG_C & ~FLAG_N & ~FLAG_H ) )
                     | ( RegA & FLAG_C )
                     );
    RegA = ( UBYTE )i;
    return( 1 /* 1 NOP */ );
}


static int ___20( void ) /* JR NZ, e */
{
    int r = 2;
    if ( ! ( FLAGS & FLAG_Z ) )
        {
        RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
        r++;
        }
    RegPC++;
    return( r );
}


static int ___21( void ) /* LD HL, nnnn */
{
    RegHL = PEEK16( RegPC );
    RegPC += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___22( void ) /* LD ( nnnn ), HL */
{
    POKE16( PEEK16( RegPC ), RegHL );
    RegPC += 2;
    return( 5 /* 5 NOPs */ );
}


static int ___23( void ) /* INC HL */
{
    RegHL++;
    return( 2 /* 2 NOPs */ );
}


static int ___24( void ) /* INC H */
{
    FLAG_INC( ++RegH );
    return( 1 /* 1 NOP */ );
}


static int ___25( void ) /* DEC H */
{
    FLAG_DEC( --RegH );
    return( 1 /* 1 NOP */ );
}


static int ___26( void ) /* LD H, n */
{
    RegH = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___27( void ) /* DAA */
{
    RegAF = TabDAA[ RegA
                  | ( ( FLAGS & FLAG_H ) << 6 )
                  | ( ( FLAGS & ( FLAG_N | FLAG_C ) ) << 8 )
                  ];
    return( 1 /* 1 NOP */ );
}


static int ___28( void ) /* JR Z, e */
{
    int r = 2;
    if ( FLAGS & FLAG_Z )
        {
        RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
        r++;
        }
    RegPC++;
    return( r );
}


static int ___29( void ) /* ADD HL, HL */
{
    ADD_R16( &RegHL, RegHL );
    return( 3 /* 3 NOPs */ );
}


static int ___2A( void ) /* LD HL, ( nnnn ) */
{
    RegHL = PEEK16( PEEK16( RegPC ) );
    RegPC += 2;
    return( 5 /* 5 NOPs */ );
}


static int ___2B( void ) /* DEC HL */
{
    RegHL--;
    return( 2 /* 2 NOPs */ );
}


static int ___2C( void ) /* INC L */
{
    FLAG_INC( ++RegL );
    return( 1 /* 1 NOP */ );
}


static int ___2D( void ) /* DEC L */
{
    FLAG_DEC( --RegL );
    return( 1 /* 1 NOP */ );
}


static int ___2E( void ) /* LD L, n */
{
    RegL = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___2F( void ) /* CPL */
{
    RegAF = ( USHORT )( ( RegAF ^ 0xFF00 ) | ( FLAG_H | FLAG_N ) );
    return( 1 /* 1 NOP */ );
}


static int ___30( void ) /* JR NC, e */
{
    int r = 2;
    if ( ! ( FLAGS & FLAG_C ) )
        {
        RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
        r++;
        }
    RegPC++;
    return( r );
}


static int ___31( void ) /* LD SP, nnnn */
{
    RegSP = PEEK16( RegPC );
    RegPC += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___32( void ) /* LD ( nnnn ), A */
{
    POKE8( PEEK16( RegPC ), RegA );
    RegPC += 2;
    return( 4 /* 4 NOPs */ );
}


static int ___33( void ) /* INC SP */
{
    RegSP++;
    return( 2 /* 2 NOPs */ );
}


static int ___34( void ) /* INC ( HL ) */
{
    UBYTE r = ( UBYTE )PEEK8( RegHL );
    FLAG_INC(  ++r );
    POKE8( RegHL, r );
    return( 3 /* 3 NOPs */ );
}


static int ___35( void ) /* DEC ( HL ) */
{
    UBYTE r = ( UBYTE )PEEK8( RegHL );
    FLAG_DEC( --r );
    POKE8( RegHL, r );
    return( 3 /* 3 NOPs */ );
}


static int ___36( void ) /* LD ( HL ), n */
{
    POKE8( RegHL, ( UBYTE )PEEK8( RegPC ) );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int ___37( void ) /* SCF */
{
    FLAGS = ( UBYTE )( ( FLAGS | FLAG_C ) & ( ~FLAG_N & ~FLAG_H ) );
    return( 1 /* 1 NOP */ );
}


static int ___38( void ) /* JR C, e */
{
    int r = 2;
    if ( FLAGS & FLAG_C )
        {
        RegPC = ( USHORT )( RegPC + ( signed char )PEEK8( RegPC ) );
        r++;
        }
    RegPC++;
    return( r );
}


static int ___39( void ) /* ADD HL, SP */
{
    ADD_R16( &RegHL, RegSP );
    return( 3 /* 3 NOPs */ );
}


static int ___3A( void ) /* LD A, ( nnnn ) */
{
    RegA = ( UBYTE )PEEK8( PEEK16( RegPC ) );
    RegPC += 2;
    return( 4 /* 4 NOPs */ );
}


static int ___3B( void ) /* DEC SP */
{
    RegSP--;
    return( 2 /* 2 NOPs */ );
}


static int ___3C( void ) /* INC A */
{
    FLAG_INC( ++RegA );
    return( 1 /* 1 NOP */ );
}


static int ___3D( void ) /* DEC A */
{
    FLAG_DEC( --RegA );
    return( 1 /* 1 NOP */ );
}


static int ___3E( void ) /* LD A, ee */
{
    RegA = ( UBYTE )PEEK8( RegPC );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___3F( void ) /* CCF */
{
    int tmp = ( FLAGS & FLAG_C ) << 4;
    FLAGS = ( UBYTE )( (( FLAGS ^ FLAG_C ) & ( ~FLAG_N & ~FLAG_H )) | tmp );
    return( 1 /* 1 NOP */ );
}


static int ___41( void ) /* LD B, C */
{
    RegB = RegC;
    return( 1 /* 1 NOP */ );
}


static int ___42( void ) /* LD B, D */
{
    RegB = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___43( void ) /* LD B, E */
{
    RegB = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___44( void ) /* LD B, H */
{
    RegB = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___45( void ) /* LD B, L */
{
    RegB = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___46( void ) /* LD B, ( HL ) */
{
    RegB = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___47( void ) /* LD B, A */
{
    RegB = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___48( void ) /* LD C, B */
{
    RegC = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___4A( void ) /* LD C, D */
{
    RegC = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___4B( void ) /* LD C, E */
{
    RegC = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___4C( void ) /* LD C, H */
{
    RegC = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___4D( void ) /* LD C, L */
{
    RegC = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___4E( void ) /* LD C, ( HL ) */
{
    RegC = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___4F( void ) /* LD C, A */
{
    RegC = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___50( void ) /* LD D, B */
{
    RegD = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___51( void ) /* LD D, C */
{
    RegD = RegC;
    return( 1 /* 1 NOP */ );
}


static int ___53( void ) /* LD D, E */
{
    RegD = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___54( void ) /* LD D, H */
{
    RegD = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___55( void ) /* LD D, L */
{
    RegD = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___56( void ) /* LD D, ( HL ) */
{
    RegD = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___57( void ) /* LD D, A */
{
    RegD = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___58( void ) /* LD E, B */
{
    RegE = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___59( void ) /* LD E, C */
{
    RegE = RegC;
    return( 1 /* 1 NOP */ );
}


static int ___5A( void ) /* LD E, D */
{
    RegE = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___5C( void ) /* LD E, H */
{
    RegE = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___5D( void ) /* LD E, L */
{
    RegE = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___5E( void ) /* LD E, ( HL ) */
{
    RegE = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___5F( void ) /* LD E, A */
{
    RegE = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___60( void ) /* LD H, B */
{
    RegH = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___61( void ) /* LD H, C */
{
    RegH = RegC;
    return( 1 /* 1 NOP */ );
}


static int ___62( void ) /* LD H, D */
{
    RegH = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___63( void ) /* LD H, E */
{
    RegH = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___65( void ) /* LD H, L */
{
    RegH = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___66( void ) /* LD H, ( HL ) */
{
    RegH = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___67( void ) /* LD H, A */
{
    RegH = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___68( void ) /* LD L, B */
{
    RegL = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___69( void ) /* LD L, C */
{
    RegL = RegC;
    return( 1 /* 1 NOP */ );
}


static int ___6A( void ) /* LD L, D */
{
    RegL = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___6B( void ) /* LD L, E */
{
    RegL = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___6C( void ) /* LD L, H */
{
    RegL = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___6E( void ) /* LD L, ( HL ) */
{
    RegL = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___6F( void ) /* LD L, A */
{
    RegL = RegA;
    return( 1 /* 1 NOP */ );
}


static int ___70( void ) /* LD ( HL ), B */
{
    POKE8( RegHL, RegB );
    return( 2 /* 2 NOPs */ );
}


static int ___71( void ) /* LD ( HL ), C */
{
    POKE8( RegHL, RegC );
    return( 2 /* 2 NOPs */ );
}


static int ___72( void ) /* LD ( HL ), D */
{
    POKE8( RegHL, RegD );
    return( 2 /* 2 NOPs */ );
}


static int ___73( void ) /* LD ( HL ), E */
{
    POKE8( RegHL, RegE );
    return( 2 /* 2 NOPs */ );
}


static int ___74( void ) /* LD ( HL ), H */
{
    POKE8( RegHL, RegH );
    return( 2 /* 2 NOPs */ );
}


static int ___75( void ) /* LD ( HL ), L */
{
    POKE8( RegHL, RegL );
    return( 2 /* 2 NOPs */ );
}


static int ___76( void ) /* HALT */
{
#ifndef HACK_IRQ
    if ( ! IRQ ) RegPC--;
#else
    RegPC--;
    halted=1;
#endif

    return( 1 /* 1 NOP */ );
}


static int ___77( void ) /* LD ( HL ), A */
{
    POKE8( RegHL, RegA );
    return( 2 /* 2 NOPs */ );
}


static int ___78( void ) /* LD A, B */
{
    RegA = RegB;
    return( 1 /* 1 NOP */ );
}


static int ___79( void ) /* LD A, C */
{
    RegA = RegC ;
    return( 1 /* 1 NOP */ );
}


static int ___7A( void ) /* LD A, D */
{
    RegA = RegD;
    return( 1 /* 1 NOP */ );
}


static int ___7B( void ) /* LD A, E */
{
    RegA = RegE;
    return( 1 /* 1 NOP */ );
}


static int ___7C( void ) /* LD A, H */
{
    RegA = RegH;
    return( 1 /* 1 NOP */ );
}


static int ___7D( void ) /* LD A, L */
{
    RegA = RegL;
    return( 1 /* 1 NOP */ );
}


static int ___7E( void ) /* LD A, ( HL ) */
{
    RegA = ( UBYTE )PEEK8( RegHL );
    return( 2 /* 2 NOPs */ );
}


static int ___80( void ) /* ADD A, B */
{
    ADD_R8( RegB );
    return( 1 /* 1 NOP */ );
}


static int ___81( void ) /* ADD A, C */
{
    ADD_R8( RegC );
    return( 1 /* 1 NOP */ );
}


static int ___82( void ) /* ADD A, D */
{
    ADD_R8( RegD );
    return( 1 /* 1 NOP */ );
}


static int ___83( void ) /* ADD A, E */
{
    ADD_R8( RegE );
    return( 1 /* 1 NOP */ );
}


static int ___84( void ) /* ADD A, H */
{
    ADD_R8( RegH );
    return( 1 /* 1 NOP */ );
}


static int ___85( void ) /* ADD A, L */
{
    ADD_R8( RegL );
    return( 1 /* 1 NOP */ );
}


static int ___86( void ) /* ADD A, ( HL ) */
{
    ADD_R8( PEEK8( RegHL ) );
    return( 2 /* 2 NOPs */ );
}


static int ___87( void ) /* ADD A, A */
{
    ADD_R8( RegA );
    return( 1 /* 1 NOP */ );
}


static int ___88( void ) /* ADC A, B */
{
    ADC_R8( RegB );
    return( 1 /* 1 NOP */ );
}


static int ___89( void ) /* ADC A, C */
{
    ADC_R8( RegC );
    return( 1 /* 1 NOP */ );
}


static int ___8A( void ) /* ADC A, D */
{
    ADC_R8( RegD );
    return( 1 /* 1 NOP */ );
}


static int ___8B( void ) /* ADC A, E */
{
    ADC_R8( RegE );
    return( 1 /* 1 NOP */ );
}


static int ___8C( void ) /* ADC A, H */
{
    ADC_R8( RegH );
    return( 1 /* 1 NOP */ );
}


static int ___8D( void ) /* ADC A, L */
{
    ADC_R8( RegL );
    return( 1 /* 1 NOP */ );
}


static int ___8E( void ) /* ADC A, (HL) */
{
    ADC_R8( PEEK8( RegHL ) );
    return( 2 /* 2 NOPs */ );
}


static int ___8F( void ) /* ADC A, A */
{
    ADC_R8( RegA );
    return( 1 /* 1 NOP */ );
}


static int ___90( void ) /* SUB B */
{
    SUB_R8( RegB );
    return( 1 /* 1 NOP */ );
}


static int ___91( void ) /* SUB C */
{
    SUB_R8( RegC );
    return( 1 /* 1 NOP */ );
}


static int ___92( void ) /* SUB D */
{
    SUB_R8( RegD );
    return( 1 /* 1 NOP */ );
}


static int ___93( void ) /* SUB E */
{
    SUB_R8( RegE );
    return( 1 /* 1 NOP */ );
}


static int ___94( void ) /* SUB H */
{
    SUB_R8( RegH );
    return( 1 /* 1 NOP */ );
}


static int ___95( void ) /* SUB L */
{
    SUB_R8( RegL );
    return( 1 /* 1 NOP */ );
}


static int ___96( void ) /* SUB (HL) */
{
    SUB_R8( PEEK8( RegHL ) );
    return( 2 /* 2 NOPs */ );
}


static int ___97( void ) /* SUB A */
{
    RegAF = ( USHORT )( FLAG_N | FLAG_Z );
    return( 1 /* 1 NOP */ );
}


static int ___98( void ) /* SBC A, B */
{
    SBC_R8( RegB );
    return( 1 /* 1 NOP */ );
}


static int ___99( void ) /* SBC A, C */
{
    SBC_R8( RegC );
    return( 1 /* 1 NOP */ );
}


static int ___9A( void ) /* SBC A, D */
{
    SBC_R8( RegD );
    return( 1 /* 1 NOP */ );
}


static int ___9B( void ) /* SBC A, E */
{
    SBC_R8( RegE );
    return( 1 /* 1 NOP */ );
}


static int ___9C( void ) /* SBC A, H */
{
    SBC_R8( RegH );
    return( 1 /* 1 NOP */ );
}


static int ___9D( void ) /* SBC A, L */
{
    SBC_R8( RegL );
    return( 1 /* 1 NOP */ );
}


static int ___9E( void ) /* SBC A, (HL) */
{
    SBC_R8( PEEK8( RegHL ) );
    return( 2 /* 2 NOPs */ );
}


static int ___9F( void ) /* SBC A, A */
{
    SBC_R8( RegA );
    return( 1 /* 1 NOP */ );
}


static int ___A0( void ) /* AND B */
{
    RegA &= RegB;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A1( void ) /* AND C */
{
    RegA &= RegC;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A2( void ) /* AND D */
{
    RegA &= RegD;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A3( void ) /* AND E */
{
    RegA &= RegE;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A4( void ) /* AND H */
{
    RegA &= RegH;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A5( void ) /* AND L */
{
    RegA &= RegL;
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A6( void ) /* AND (HL) */
{
    RegA &= PEEK8( RegHL );
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 2 /* 2 NOPs */ );
}


static int ___A7( void ) /* AND A */
{
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    return( 1 /* 1 NOP */ );
}


static int ___A8( void ) /* XOR B */
{
    RegA ^= RegB;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___A9( void ) /* XOR C */
{
    RegA ^= RegC;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___AA( void ) /* XOR D */
{
    RegA ^= RegD;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___AB( void ) /* XOR E */
{
    RegA ^= RegE;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___AC( void ) /* XOR H */
{
    RegA ^= RegH;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___AD( void ) /* XOR L */
{
    RegA ^= RegL;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___AE( void ) /* XOR (HL) */
{
    RegA ^= PEEK8( RegHL );
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int ___AF( void ) /* XOR A */
{
    RegAF = FLAG_Z | FLAG_V;
    return( 1 /* 1 NOP */ );
}


static int ___B0( void ) /* OR B */
{
    RegA |= RegB;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B1( void ) /* OR C */
{
    RegA |= RegC;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B2( void ) /* OR D */
{
    RegA |= RegD;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B3( void ) /* OR E */
{
    RegA |= RegE;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B4( void ) /* OR H */
{
    RegA |= RegH;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B5( void ) /* OR L */
{
    RegA |= RegL;
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B6( void ) /* OR (HL) */
{
    RegA |= PEEK8( RegHL );
    FLAGS = Parite[ RegA ];
    return( 2 /* 2 NOPs */ );
}


static int ___B7( void ) /* OR A */
{
    FLAGS = Parite[ RegA ];
    return( 1 /* 1 NOP */ );
}


static int ___B8( void ) /* CP B */
{
    CP_R8( RegB );
    return( 1 /* 1 NOP */ );
}


static int ___B9( void ) /* CP C */
{
    CP_R8( RegC );
    return( 1 /* 1 NOP */ );
}


static int ___BA( void ) /* CP D */
{
    CP_R8( RegD );
    return( 1 /* 1 NOP */ );
}


static int ___BB( void ) /* CP E */
{
    CP_R8( RegE );
    return( 1 /* 1 NOP */ );
}


static int ___BC( void ) /* CP H */
{
    CP_R8( RegH );
    return( 1 /* 1 NOP */ );
}


static int ___BD( void ) /* CP L */
{
    CP_R8( RegL );
    return( 1 /* 1 NOP */ );
}


static int ___BE( void ) /* CP (HL) */
{
    CP_R8( PEEK8( RegHL ) );
    return( 2 /* 2 NOPs */ );
}


static int ___BF( void ) /* CP A */
{
    FLAGS = FLAG_N | FLAG_Z;
    return( 1 /* 1 NOP */ );
}


static int ___C0( void ) /* RET NZ */
{
    if ( ! ( FLAGS & FLAG_Z ) )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___C1( void ) /* POP BC */
{
    RegBC = PEEK16( RegSP );
    RegSP += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___C2( void ) /* JP NZ, nnnn */
{
    if ( ! ( FLAGS & FLAG_Z ) )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___C3( void ) /* JP nnnn */
{
    RegPC = PEEK16( RegPC );
    return( 3 /* 3 NOPs */ );
}


static int ___C4( void ) /* CALL NZ, nnnn */
{
    if ( ! ( FLAGS & FLAG_Z ) )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___C5( void ) /* PUSH BC */
{
    RegSP -= 2;
    POKE16( RegSP, RegBC );
    return( 4 /* 4 NOPs */ );
}


static int ___C6( void ) /* ADD A, ee */
{
    ADD_R8( PEEK8( RegPC ) );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___C7( void ) /* RST 00 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x00;
    return( 4 /* 4 NOPs */ );
}


static int ___C8( void ) /* RET Z */
{
    if ( FLAGS & FLAG_Z )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


int ___C9( void ) /* RET */
{
    RegPC = PEEK16( RegSP );
    RegSP += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___CA( void ) /* JP Z, nnnn */
{
    if ( FLAGS & FLAG_Z )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}



static int ___CB( void ) /* Special code CB */
{
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    int r = tabCB[ PEEK8( RegPC++ ) ]();
    return( r );
}


static int ___CC( void ) /* CALL Z, nnnn */
{
    if ( FLAGS & FLAG_Z )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___CD( void ) /* CALL nnnn */
{
    RegSP -= 2;
    POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
    RegPC = PEEK16( RegPC );
    return( 5 /* 5 NOPs */ );
}


static int ___CE( void ) /* ADC A, ee */
{
    ADC_R8( PEEK8( RegPC ) );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___CF( void ) /* RST 08 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x08;
    return( 4 /* 4 NOPs */ );
}


static int ___D0( void ) /* RET NC */
{
    if ( ! ( FLAGS & FLAG_C ) )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___D1( void ) /* POP DE */
{
    RegDE = PEEK16( RegSP );
    RegSP += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___D2( void ) /* JP NC, nnnn */
{
    if ( ! ( FLAGS & FLAG_C ) )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___D3( void ) /* OUT ( n ), A */
{
    WritePort( ( RegA << 8 ) + PEEK8( RegPC ), RegA );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int ___D4( void ) /* CALL NC, nnnn */
{
    if ( ! ( FLAGS & FLAG_C ) )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___D5( void ) /* PUSH DE */
{
    RegSP -= 2;
    POKE16( RegSP, RegDE );
    return( 4 /* 4 NOPs */ );
}


static int ___D6( void ) /* SUB ee */
{
    SUB_R8( PEEK8( RegPC ) );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___D7( void ) /* RST 10 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x10;
    return( 4 /* 4 NOPs */ );
}


static int ___D8( void ) /* RET C */
{
    if ( FLAGS & FLAG_C )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___D9( void ) /* EXX */
{
    USHORT tmp = RegBC;
    RegBC = Reg_BC;
    Reg_BC = tmp;

    tmp = RegDE;
    RegDE = Reg_DE;
    Reg_DE = tmp;

    tmp = RegHL;
    RegHL = Reg_HL;
    Reg_HL = tmp;

    return( 1 /* 1 NOP */ );
}


static int ___DA( void ) /* JP C, nnnn */
{
    if ( FLAGS & FLAG_C )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___DB( void ) /* IN A, ( n ) */
{
    RegA = ( UBYTE )ReadPort( ( RegA << 8 ) + PEEK8( RegPC ) );
    RegPC++;
    return( 3 /* 3 NOPs */ );
}


static int ___DC( void ) /* CALL C, nnnn */
{
    if ( FLAGS & FLAG_C )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___DD( void ) /* Special code DD : IX */
{
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    int r = tabIX[ PEEK8( RegPC++ ) ]();
    return( r );
}


static int ___DE( void ) /* SBC A, ee */
{
    SBC_R8( PEEK8( RegPC ) );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___DF( void ) /* RST 18 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x18;
    return( 4 /* 4 NOPs */ );
}


static int ___E0( void ) /* RET PO */
{
    if ( ! ( FLAGS & FLAG_V ) )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___E1( void ) /* POP HL */
{
    RegHL = PEEK16( RegSP );
    RegSP += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___E2( void ) /* JP PO, nnnn */
{
    if ( ! ( FLAGS & FLAG_V ) )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___E3( void ) /* EX (SP), HL */
{
    USHORT a = PEEK16( RegSP );
    POKE16( RegSP, RegHL );
    RegHL = a;
    return( 6 /* 6 NOPs */ );
}


static int ___E4( void ) /* CALL PO, nnnn */
{
    if ( ! ( FLAGS & FLAG_V ) )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___E5( void ) /* PUSH HL */
{
    RegSP -= 2;
    POKE16( RegSP, RegHL );
    return( 4 /* 4 NOPs */ );
}


static int ___E6( void ) /* AND ee */
{
    RegA &= PEEK8( RegPC );
    FLAGS = ( UBYTE )( FLAG_H | Parite[ RegA ] );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___E7( void ) /* RST 20 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x20;
    return( 4 /* 4 NOPs */ );
}


static int ___E8( void ) /* RET PE */
{
    if ( FLAGS & FLAG_V )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___E9( void ) /* JP ( HL ) */
{
    RegPC = RegHL;
    return( 1 /* 1 NOP */ );
}


static int ___EA( void ) /* JP PE, nnnn */
{
    if ( FLAGS & FLAG_V )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___EB( void ) /* EX DE, HL */
{
    USHORT tmp = RegDE;
    RegDE = RegHL;
    RegHL = tmp;
    return( 1 /* 1 NOP */ );
}


static int ___EC( void ) /* CALL PE, nnnn */
{
    if ( FLAGS & FLAG_V )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___ED( void ) /* Special code ED */
{
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    int r = tabED[ PEEK8( RegPC++ ) ]();
    return( r );
}


static int ___EE( void ) /* XOR ee */
{
    RegA ^= PEEK8( RegPC );
    FLAGS = Parite[ RegA ];
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___EF( void ) /* RST 28 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x28;
    return( 4 /* 4 NOPs */ );
}


static int ___F0( void ) /* RET P */
{
    if ( ! ( FLAGS & FLAG_S ) )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___F1( void ) /* POP AF */
{
    RegAF = PEEK16( RegSP );
    RegSP += 2;
    return( 3 /* 3 NOPs */ );
}


static int ___F2( void ) /* JP P, nnnn */
{
    if ( ! ( FLAGS & FLAG_S ) )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___F3( void ) /* DI */
{
    Z80.IFF1 = Z80.IFF2 = 0;
    return( 1 /* 1 NOP */ );
}


static int ___F4( void ) /* CALL P, nnnn */
{
    if ( ! ( FLAGS & FLAG_S ) )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___F5( void ) /* PUSH AF */
{
    RegSP -= 2;
    POKE16( RegSP, RegAF );
    return( 4 /* 4 NOPs */ );
}


static int ___F6( void ) /* OR ee */
{
    RegA |= PEEK8( RegPC );
    FLAGS = Parite[ RegA ];
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___F7( void ) /* RST 30 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x30;
    return( 4 /* 4 NOPs */ );
}


static int ___F8( void ) /* RET M */
{
    if ( FLAGS & FLAG_S )
        {
        RegPC = PEEK16( RegSP );
        RegSP += 2;
        return( 4 /* 4 NOPs */ );
        }
    else
        return( 2 /* 2 NOPs */ );
}


static int ___F9( void ) /* LD SP, HL */
{
    RegSP = RegHL;
    return( 2 /* 2 NOPs */ );
}


static int ___FA( void ) /* JP M, nnnn */
{
    if ( FLAGS & FLAG_S )
        {
        RegPC = PEEK16( RegPC );
        }
    else
        RegPC += 2;

    return( 3 /* 3 NOPs */ );
}


static int ___FB( void ) /* EI */
{
    Z80.IFF1 = Z80.IFF2 = 1;
#ifdef HACK_IRQ
    VerifyIRQ();
#endif
    return( 1 /* 1 NOP */ );
}


static int ___FC( void ) /* CALL M, nnnn */
{
    if ( FLAGS & FLAG_S )
        {
        RegSP -= 2;
        POKE16( RegSP, ( USHORT )( RegPC + 2 ) );
        RegPC = PEEK16( RegPC );
        return( 5 /* 5 NOPs */ );
        }
    else
        {
        RegPC += 2;
        return( 3 /* 3 NOPs */ );
        }
}


static int ___FD( void ) /* Special code FD : IY */
{
    RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) );
    int r = tabIY[ PEEK8( RegPC++ ) ]();
    return( r );
}


static int ___FE( void ) /* CP ee */
{
    CP_R8( PEEK8( RegPC ) );
    RegPC++;
    return( 2 /* 2 NOPs */ );
}


static int ___FF( void ) /* RST 38 */
{
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x38;
    return( 4 /* 4 NOPs */ );
}

static pfct tabinstr[ 256 ] =
    {
    NO_OP, ___01, ___02, ___03, ___04, ___05, ___06, ___07, // 00
    ___08, ___09, ___0A, ___0B, ___0C, ___0D, ___0E, ___0F, // 08
    ___10, ___11, ___12, ___13, ___14, ___15, ___16, ___17, // 10
    ___18, ___19, ___1A, ___1B, ___1C, ___1D, ___1E, ___1F, // 18
    ___20, ___21, ___22, ___23, ___24, ___25, ___26, ___27, // 20
    ___28, ___29, ___2A, ___2B, ___2C, ___2D, ___2E, ___2F, // 28
    ___30, ___31, ___32, ___33, ___34, ___35, ___36, ___37, // 30
    ___38, ___39, ___3A, ___3B, ___3C, ___3D, ___3E, ___3F, // 38
    NO_OP, ___41, ___42, ___43, ___44, ___45, ___46, ___47, // 40
    ___48, NO_OP, ___4A, ___4B, ___4C, ___4D, ___4E, ___4F, // 48
    ___50, ___51, NO_OP, ___53, ___54, ___55, ___56, ___57, // 50
    ___58, ___59, ___5A, NO_OP, ___5C, ___5D, ___5E, ___5F, // 58
    ___60, ___61, ___62, ___63, NO_OP, ___65, ___66, ___67, // 60
    ___68, ___69, ___6A, ___6B, ___6C, NO_OP, ___6E, ___6F, // 68
    ___70, ___71, ___72, ___73, ___74, ___75, ___76, ___77, // 70
    ___78, ___79, ___7A, ___7B, ___7C, ___7D, ___7E, NO_OP, // 78
    ___80, ___81, ___82, ___83, ___84, ___85, ___86, ___87, // 80
    ___88, ___89, ___8A, ___8B, ___8C, ___8D, ___8E, ___8F, // 88
    ___90, ___91, ___92, ___93, ___94, ___95, ___96, ___97, // 90
    ___98, ___99, ___9A, ___9B, ___9C, ___9D, ___9E, ___9F, // 98
    ___A0, ___A1, ___A2, ___A3, ___A4, ___A5, ___A6, ___A7, // A0
    ___A8, ___A9, ___AA, ___AB, ___AC, ___AD, ___AE, ___AF, // A8
    ___B0, ___B1, ___B2, ___B3, ___B4, ___B5, ___B6, ___B7, // B0
    ___B8, ___B9, ___BA, ___BB, ___BC, ___BD, ___BE, ___BF, // B8
    ___C0, ___C1, ___C2, ___C3, ___C4, ___C5, ___C6, ___C7, // C0
    ___C8, ___C9, ___CA, ___CB, ___CC, ___CD, ___CE, ___CF, // C8
    ___D0, ___D1, ___D2, ___D3, ___D4, ___D5, ___D6, ___D7, // D0
    ___D8, ___D9, ___DA, ___DB, ___DC, ___DD, ___DE, ___DF, // D8
    ___E0, ___E1, ___E2, ___E3, ___E4, ___E5, ___E6, ___E7, // E0
    ___E8, ___E9, ___EA, ___EB, ___EC, ___ED, ___EE, ___EF, // E8
    ___F0, ___F1, ___F2, ___F3, ___F4, ___F5, ___F6, ___F7, // F0
    ___F8, ___F9, ___FA, ___FB, ___FC, ___FD, ___FE, ___FF  // F8
};


pfct tabCB[ 256 ] =
    {
    CB_00, CB_01, CB_02, CB_03, CB_04, CB_05, CB_06, CB_07, // 00
    CB_08, CB_09, CB_0A, CB_0B, CB_0C, CB_0D, CB_0E, CB_0F, // 08
    CB_10, CB_11, CB_12, CB_13, CB_14, CB_15, CB_16, CB_17, // 10
    CB_18, CB_19, CB_1A, CB_1B, CB_1C, CB_1D, CB_1E, CB_1F, // 18
    CB_20, CB_21, CB_22, CB_23, CB_24, CB_25, CB_26, CB_27, // 20
    CB_28, CB_29, CB_2A, CB_2B, CB_2C, CB_2D, CB_2E, CB_2F, // 28
    CB_30, CB_31, CB_32, CB_33, CB_34, CB_35, CB_36, CB_37, // 30
    CB_38, CB_39, CB_3A, CB_3B, CB_3C, CB_3D, CB_3E, CB_3F, // 38
    CB_40, CB_41, CB_42, CB_43, CB_44, CB_45, CB_46, CB_47, // 40
    CB_48, CB_49, CB_4A, CB_4B, CB_4C, CB_4D, CB_4E, CB_4F, // 48
    CB_50, CB_51, CB_52, CB_53, CB_54, CB_55, CB_56, CB_57, // 50
    CB_58, CB_59, CB_5A, CB_5B, CB_5C, CB_5D, CB_5E, CB_5F, // 58
    CB_60, CB_61, CB_62, CB_63, CB_64, CB_65, CB_66, CB_67, // 60
    CB_68, CB_69, CB_6A, CB_6B, CB_6C, CB_6D, CB_6E, CB_6F, // 68
    CB_70, CB_71, CB_72, CB_73, CB_74, CB_75, CB_76, CB_77, // 70
    CB_78, CB_79, CB_7A, CB_7B, CB_7C, CB_7D, CB_7E, CB_7F, // 78
    CB_80, CB_81, CB_82, CB_83, CB_84, CB_85, CB_86, CB_87, // 80
    CB_88, CB_89, CB_8A, CB_8B, CB_8C, CB_8D, CB_8E, CB_8F, // 88
    CB_90, CB_91, CB_92, CB_93, CB_94, CB_95, CB_96, CB_97, // 90
    CB_98, CB_99, CB_9A, CB_9B, CB_9C, CB_9D, CB_9E, CB_9F, // 98
    CB_A0, CB_A1, CB_A2, CB_A3, CB_A4, CB_A5, CB_A6, CB_A7, // A0
    CB_A8, CB_A9, CB_AA, CB_AB, CB_AC, CB_AD, CB_AE, CB_AF, // A8
    CB_B0, CB_B1, CB_B2, CB_B3, CB_B4, CB_B5, CB_B6, CB_B7, // B0
    CB_B8, CB_B9, CB_BA, CB_BB, CB_BC, CB_BD, CB_BE, CB_BF, // B8
    CB_C0, CB_C1, CB_C2, CB_C3, CB_C4, CB_C5, CB_C6, CB_C7, // C0
    CB_C8, CB_C9, CB_CA, CB_CB, CB_CC, CB_CD, CB_CE, CB_CF, // C8
    CB_D0, CB_D1, CB_D2, CB_D3, CB_D4, CB_D5, CB_D6, CB_D7, // D0
    CB_D8, CB_D9, CB_DA, CB_DB, CB_DC, CB_DD, CB_DE, CB_DF, // D8
    CB_E0, CB_E1, CB_E2, CB_E3, CB_E4, CB_E5, CB_E6, CB_E7, // E0
    CB_E8, CB_E9, CB_EA, CB_EB, CB_EC, CB_ED, CB_EE, CB_EF, // E8
    CB_F0, CB_F1, CB_F2, CB_F3, CB_F4, CB_F5, CB_F6, CB_F7, // F0
    CB_F8, CB_F9, CB_FA, CB_FB, CB_FC, CB_FD, CB_FE, CB_FF, // F8
    };


pfct tabED[ 256 ] =
    {
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 00
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 08
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 10
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 18
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 20
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 28
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 30
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 38
    ED_40, ED_41, ED_42, ED_43, ED_44, ED_45, ED_46, ED_47, // 40
    ED_48, ED_49, ED_4A, ED_4B, ED_44, ED_4D, ED_46, ED_4F, // 48
    ED_50, ED_51, ED_52, ED_53, ED_44, ED_45, ED_56, ED_57, // 50
    ED_58, ED_59, ED_5A, ED_5B, ED_44, ED_45, ED_5E, ED_5F, // 58
    ED_60, ED_61, ED_62, ED_63, ED_44, ED_45, ED_46, ED_67, // 60
    ED_68, ED_69, ED_6A, ED_6B, ED_44, ED_45, ED_46, ED_6F, // 68
    ED_70, ED_71, ED_72, ED_73, ED_44, ED_45, ED_56, ed___, // 70
    ED_78, ED_79, ED_7A, ED_7B, ED_44, ED_45, ED_5E, ed___, // 78
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 80
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 88
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 90
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // 98
    ED_A0, ED_A1, ED_A2, ED_A3, ed___, ed___, ed___, ed___, // A0
    ED_A8, ED_A9, ED_AA, ED_AB, ed___, ed___, ed___, ed___, // A8
    ED_B0, ED_B1, ED_B2, ED_B3, ed___, ed___, ed___, ed___, // B0
    ED_B8, ED_B9, ED_BA, ED_BB, ed___, ed___, ed___, ed___, // B8
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // C0
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // C8
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // D0
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // D8
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // E0
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // E8
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___, // F0
    ed___, ed___, ed___, ed___, ed___, ed___, ed___, ed___  // F8
    };


pfct tabIX[ 256 ] =
    {
    NO_OP, ___01, ___02, ___03, ___04, ___05, ___06, ___07, // 00
    ___08, DD_09, ___0A, ___0B, ___0C, ___0D, ___0E, ___0F, // 08
    ___10, ___11, ___12, ___13, ___14, ___15, ___16, ___17, // 10
    ___18, DD_19, ___1A, ___1B, ___1C, ___1D, ___1E, ___1F, // 18
    ___20, DD_21, DD_22, DD_23, DD_24, DD_25, DD_26, ___27, // 20
    ___28, DD_29, DD_2A, DD_2B, DD_2C, DD_2D, DD_2E, ___2F, // 28
    ___30, ___31, ___32, ___33, DD_34, DD_35, DD_36, ___37, // 30
    ___38, DD_39, ___3A, ___3B, ___3C, ___3D, ___3E, ___3F, // 38
    NO_OP, ___41, ___42, ___43, DD_44, DD_45, DD_46, ___47, // 40
    ___48, NO_OP, ___4A, ___4B, DD_4C, DD_4D, DD_4E, ___4F, // 48
    ___50, ___51, NO_OP, ___53, DD_54, DD_55, DD_56, ___57, // 50
    ___58, ___59, ___5A, NO_OP, DD_5C, DD_5D, DD_5E, ___5F, // 58
    DD_60, DD_61, DD_62, DD_63, NO_OP, DD_65, DD_66, DD_67, // 60
    DD_68, DD_69, DD_6A, DD_6B, DD_6C, NO_OP, DD_6E, DD_6F, // 68
    DD_70, DD_71, DD_72, DD_73, DD_74, DD_75, ___76, DD_77, // 70
    ___78, ___79, ___7A, ___7B, DD_7C, DD_7D, DD_7E, NO_OP, // 78
    ___80, ___81, ___82, ___83, DD_84, DD_85, DD_86, ___87, // 80
    ___88, ___89, ___8A, ___8B, DD_8C, DD_8D, DD_8E, ___8F, // 88
    ___90, ___91, ___92, ___93, DD_94, DD_95, DD_96, ___97, // 90
    ___98, ___99, ___9A, ___9B, DD_9C, DD_9D, DD_9E, ___9F, // 98
    ___A0, ___A1, ___A2, ___A3, DD_A4, DD_A5, DD_A6, ___A7, // A0
    ___A8, ___A9, ___AA, ___AB, DD_AC, DD_AD, DD_AE, ___AF, // A8
    ___B0, ___B1, ___B2, ___B3, DD_B4, DD_B5, DD_B6, ___B7, // B0
    ___B8, ___B9, ___BA, ___BB, DD_BC, DD_BD, DD_BE, ___BF, // B8
    ___C0, ___C1, ___C2, ___C3, ___C4, ___C5, ___C6, ___C7, // C0
    ___C8, ___C9, ___CA, DD_CB, ___CC, ___CD, ___CE, ___CF, // C8
    ___D0, ___D1, ___D2, ___D3, ___D4, ___D5, ___D6, ___D7, // D0
    ___D8, ___D9, ___DA, ___DB, ___DC, dd___, ___DE, ___DF, // D8
    ___E0, DD_E1, ___E2, DD_E3, ___E4, DD_E5, ___E6, ___E7, // E0
    ___E8, DD_E9, ___EA, ___EB, ___EC, dd___, ___EE, ___EF, // E8
    ___F0, ___F1, ___F2, ___F3, ___F4, ___F5, ___F6, ___F7, // F0
    ___F8, DD_F9, ___FA, ___FB, ___FC, DD_FD, ___FE, ___FF  // F8
    };

pfct tabIY[ 256 ] =
    {
    NO_OP, ___01, ___02, ___03, ___04, ___05, ___06, ___07, // 00
    ___08, FD_09, ___0A, ___0B, ___0C, ___0D, ___0E, ___0F, // 08
    ___10, ___11, ___12, ___13, ___14, ___15, ___16, ___17, // 10
    ___18, FD_19, ___1A, ___1B, ___1C, ___1D, ___1E, ___1F, // 18
    ___20, FD_21, FD_22, FD_23, FD_24, FD_25, FD_26, ___27, // 20
    ___28, FD_29, FD_2A, FD_2B, FD_2C, FD_2D, FD_2E, ___2F, // 28
    ___30, ___31, ___32, ___33, FD_34, FD_35, FD_36, ___37, // 30
    ___38, FD_39, ___3A, ___3B, ___3C, ___3D, ___3E, ___3F, // 38
    NO_OP, ___41, ___42, ___43, FD_44, FD_45, FD_46, ___47, // 40
    ___48, NO_OP, ___4A, ___4B, FD_4C, FD_4D, FD_4E, ___4F, // 48
    ___50, ___51, NO_OP, ___53, FD_54, FD_55, FD_56, ___57, // 50
    ___58, ___59, ___5A, NO_OP, FD_5C, FD_5D, FD_5E, ___5F, // 58
    FD_60, FD_61, FD_62, FD_63, NO_OP, FD_65, FD_66, FD_67, // 60
    FD_68, FD_69, FD_6A, FD_6B, FD_6C, NO_OP, FD_6E, FD_6F, // 68
    FD_70, FD_71, FD_72, FD_73, FD_74, FD_75, ___76, FD_77, // 70
    ___78, ___79, ___7A, ___7B, FD_7C, FD_7D, FD_7E, NO_OP, // 78
    ___80, ___81, ___82, ___83, FD_84, FD_85, FD_86, ___87, // 80
    ___88, ___89, ___8A, ___8B, FD_8C, FD_8D, FD_8E, ___8F, // 88
    ___90, ___91, ___92, ___93, FD_94, FD_95, FD_96, ___97, // 90
    ___98, ___99, ___9A, ___9B, FD_9C, FD_9D, FD_9E, ___9F, // 98
    ___A0, ___A1, ___A2, ___A3, FD_A4, FD_A5, FD_A6, ___A7, // A0
    ___A8, ___A9, ___AA, ___AB, FD_AC, FD_AD, FD_AE, ___AF, // A8
    ___B0, ___B1, ___B2, ___B3, FD_B4, FD_B5, FD_B6, ___B7, // B0
    ___B8, ___B9, ___BA, ___BB, FD_BC, FD_BD, FD_BE, ___BF, // B8
    ___C0, ___C1, ___C2, ___C3, ___C4, ___C5, ___C6, ___C7, // C0
    ___C8, ___C9, ___CA, FD_CB, ___CC, ___CD, ___CE, ___CF, // C8
    ___D0, ___D1, ___D2, ___D3, ___D4, ___D5, ___D6, ___D7, // D0
    ___D8, ___D9, ___DA, ___DB, ___DC, FD_DD, ___DE, ___DF, // D8
    ___E0, FD_E1, ___E2, FD_E3, ___E4, FD_E5, ___E6, ___E7, // E0
    ___E8, FD_E9, ___EA, ___EB, ___EC, fd___, ___EE, ___EF, // E8
    ___F0, ___F1, ___F2, ___F3, ___F4, ___F5, ___F6, ___F7, // F0
    ___F8, FD_F9, ___FA, ___FB, ___FC, fd___, ___FE, ___FF  // F8
    };


/********************************************************* !NAME! **************
* Nom : Z80_NMI
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Emulation d'une NMI du Z80
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
int Z80_NMI( void )
{
    Z80.IFF2 = Z80.IFF1;
    Z80.IFF1 = 0;
    RegSP -= 2;
    POKE16( RegSP, RegPC );
    RegPC = 0x66;
    return( 4 /* 4 NOPs */ );
}

int bycycle;

/********************************************************* !NAME! **************
* Nom : ExecInstZ80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Execution de l'instruction Z80 � l'adresse du PC
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80, CntHSync
*
********************************************************** !0! ****************/
int ExecInstZ80_orig(void)
{
    bycycle=0;

    while(bycycle<RegsCRTC[ 0 ] + 1) {
#ifndef HACK_IR
        RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) ); // Retire par RedBug, ca marche sans :)
#endif
        bycycle += tabinstr[ PEEK8(RegPC++) ]();
        
#ifndef HACK_IRQ
        VerifyIRQ();
#endif          
    }
#ifdef HACK_IR
    RegR = ( UBYTE )( ( ( RegR + RegsCRTC[ 0 ] + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ) ); // Fake RegR HACK
#endif

    return(bycycle);
}


/********************************************************* !NAME! **************
* Nom : ResetZ80
********************************************************** !PATHS! *************
* !./V1!\!./V2!\!./V3!\!./V4!\Fonctions
********************************************************** !1! *****************
*
* Fichier     : !./FPTH\/FLE!, ligne : !./LN!
*
* Description : Reset du Z80
*
* R�sultat    : /
*
* Variables globales modifi�es : Z80
*
********************************************************** !0! ****************/
void ResetZ80_orig( void )
{
    memset( &Z80, 0xFF, sizeof( Z80 ) );
    RegPC = 0;
    RegR = Z80.IFF1 = Z80.IFF2 = Z80.InterruptMode = 0;
}



void SetIRQZ80_orig(int i)
{
    IRQ=i;
#ifdef HACK_IRQ
    VerifyIRQ();
#endif
}

void VerifyIRQ(void) {
    if (IRQ) {
#ifdef HACK_IRQ
        if (halted) {
            RegPC ++;
            halted=0;
        }
#endif            
        if ( Z80.IFF1 ) // EI ?
            {
            Z80.IFF1 = 0;
            RegR = ( UBYTE )( ( ( RegR + 1 ) & 0x7F ) | ( UBYTE )( RegR & 0x80 ));
            CntHSync &= 0x1F;
            RegSP -= 2;
            POKE16( RegSP, RegPC );
            if ( Z80.InterruptMode < 2 ) {
                //
                // IM 0 et IM 1 -> RST 38H
                //
                RegPC = 0x38;
                bycycle += 4;
            } else {
                //
                // IM 2 -> CALL ( adr( IR ) )
                //
                RegPC = PEEK16( RegIR );
                bycycle += 4;
                }
            IRQ=0;
            }
    }
}

void WriteZ80(SRegs *z0)
{
    Z80.AF.Word = z0->AF.Word;
    Z80.BC.Word = z0->BC.Word;
    Z80.DE.Word = z0->DE.Word;
    Z80.HL.Word = z0->HL.Word;
    Z80.IR.Word = z0->IR.Word;
    Z80.IFF1 = z0->IFF1;
    Z80.IFF2 = z0->IFF2;
    Z80.IX.Word = z0->IX.Word;
    Z80.IY.Word = z0->IY.Word;
    Z80.SP.Word = z0->SP.Word;
    Z80.PC.Word = z0->PC.Word;
    Z80.InterruptMode = z0->InterruptMode;
    Z80._AF.Word = z0->_AF.Word;
    Z80._BC.Word = z0->_BC.Word;
    Z80._DE.Word = z0->_DE.Word;
    Z80._HL.Word = z0->_HL.Word;
}

void ReadZ80(SRegs *z0)
{
    z0->AF.Word = Z80.AF.Word;
    z0->BC.Word = Z80.BC.Word;
    z0->DE.Word = Z80.DE.Word;
    z0->HL.Word = Z80.HL.Word;
    z0->IR.Word = Z80.IR.Word;
    z0->IFF1 = Z80.IFF1;
    z0->IFF2 = Z80.IFF2;
    z0->IX.Word = Z80.IX.Word;
    z0->IY.Word = Z80.IY.Word;
    z0->SP.Word = Z80.SP.Word;
    z0->PC.Word = Z80.PC.Word;
    z0->InterruptMode = Z80.InterruptMode;
    z0->_AF.Word = Z80._AF.Word;
    z0->_BC.Word = Z80._BC.Word;
    z0->_DE.Word = Z80._DE.Word;
    z0->_HL.Word = Z80._HL.Word;
}
