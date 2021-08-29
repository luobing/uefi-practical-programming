//Font.h:
//Code by luobing 2019-5-16 15:47:30
#ifndef _FONT_H
#define _FONT_H

#include "Common.h"

//Add for UTF-8 Chinese char robin 2020-0522 1229
//FONT
typedef struct struct_lattice
{
	UINT32			utf8_code;
	UINT8			width;
	UINT8 *	pixel_gray_array;
} LATTICE;
typedef struct struct_font_info
{
	UINT8	height;
	UINT32	count;
	LATTICE*		lattice_array;
} FONT_INFO;

// extern FONT_INFO MicroYH;
// extern FONT_INFO MicroYH1;
extern FONT_INFO MyFontArray;
LATTICE *get_lattice(FONT_INFO *font, UINT32 utf8_code);
VOID draw_lattice(UINTN x, UINTN y, UINT8 width, UINT8 height, UINT8 *p_data, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *font_color);
UINT8 draw_single_char(UINT32 utf8_code, UINTN x, UINTN y, FONT_INFO *font, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *font_color);
VOID draw_string(UINT8 *str, UINTN x, UINTN y, FONT_INFO *font, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *font_color);
VOID draw_string_alpha(UINT8 *str, UINTN x, UINTN y, FONT_INFO *font, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *font_color, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *bg_color, UINT16 Alpha);
#endif