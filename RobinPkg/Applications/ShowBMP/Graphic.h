//code by luobing 2013-3-19 11:00:09

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
VOID  rectblock(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID  rectangle(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID  circle(UINTN centerx,UINTN centery,UINTN radius,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);

VOID getRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);
VOID putRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer);									
//Debug function
EFI_STATUS DisplayModeInformation(void);
EFI_STATUS DisplaySpecifyModeMessage(UINT32 ModeNumber);
EFI_STATUS DisplayTextModeInformation(void);
EFI_STATUS DisplaySpecifyTextModeMessage(UINTN ModeNumber);

#endif