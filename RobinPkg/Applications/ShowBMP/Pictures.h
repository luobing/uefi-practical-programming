//display 256 color bmp pictures

#ifndef _PICTURES_H
#define _PICTURES_H
#include "Common.h"

VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putBMP256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putBMP256_fun(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  );
EFI_STATUS ShowBMP256(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowBMP24True(CHAR16 *fileName,UINTN x,UINTN y);
#endif