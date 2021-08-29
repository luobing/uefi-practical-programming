//code by luobing 

#ifndef _GRAPHIC_H
#define _GRAPHIC_H
#include "Common.h"

EFI_STATUS GetGraphicModeNumber(
	IN	UINT32 HorRes,
	IN 	UINT32 VerRes,
	OUT UINT32 *ModeNum,
	OUT UINT32 *CurrentModeNum,
	OUT UINT32 *PixelsPerScanLine
);
EFI_STATUS SwitchGraphicsMode(BOOLEAN flag);
VOID SetMyMode(UINT32 ModeNumber);
VOID putpixel(UINTN x,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);

VOID HLine(UINTN x1,UINTN x2,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID VLine(UINTN x,UINTN y1,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID Line(UINTN x1, UINTN y1, UINTN x2, UINTN y2, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID  rectblock(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID  rectangle(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID  circle(UINTN centerx,UINTN centery,UINTN radius,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);

VOID getRectImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);
VOID putRectImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);	
VOID putPartRectImage_FOOL(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
					  IN UINTN distanceX, IN UINTN distanceY, IN UINTN partWidth, IN UINTN partHeight,
					  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);		
VOID putPartRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
					  IN UINTN imageX, IN UINTN imageY, IN UINTN partWidth, IN UINTN partHeight,
					  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);
VOID putImagePixel(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,
				   IN UINTN imgX, UINTN imgY, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);
VOID putImageLine(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,
				  IN UINTN imageX1, IN UINTN imageY1, IN UINTN imageX2, IN UINTN imageY2,
				  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);

#endif