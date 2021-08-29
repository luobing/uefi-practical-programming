//code by luobing  
#ifndef _WINDOW_H
#define _WINDOW_H	
#include "Common.h"


//***************************  Macro **********************************************
//#define SYGraphicsMode  0x2         //800X600
#define SY_SCREEN_WIDTH   1024 
#define SY_SCREEN_HEIGHT  768
#define BLACK 0
#define WHITE 1
#define LIGHTGRAY 2
#define DEEPBLUE 3
#define DARKBLACK 4
#define LIGHTBLACK 5
#define YELLOW 6
#define GREEN 7
#define RED 8
#define BLUE 9

VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);

extern UINT32 OldGraphicsMode;
#endif