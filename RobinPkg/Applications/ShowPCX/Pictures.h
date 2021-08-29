//code by luobing 
//display 256 color bmp pictures

#ifndef _PICTURES_H
#define _PICTURES_H
#include "Common.h"
typedef struct pcx_header_type
{
	UINT8 Manufacturer;
	UINT8 version;
	UINT8 Encoding;
	UINT8 BitsPerPixel;
	UINT16 xMin,yMin;
	UINT16 xMax,yMax;
	UINT16 Hres;
	UINT16 Vres;
	UINT8 ColorMap[48];
	UINT8 reserved;
	UINT8 ColorPlanes;
	UINT16 BytesPerLine;    //每行的字节数，必定为偶数
	UINT16 PaletteType;
	UINT8 padding[58];
} PCX_HEADER, *PCX_HEADER_PTR;


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

EFI_STATUS decompressPCX24bits(IN EFI_FILE_PROTOCOL *file, OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
EFI_STATUS decompressPCX24bits_fast(IN EFI_FILE_PROTOCOL *file, OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
VOID putPCX256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putPCX256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID decompressPCX256(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
VOID decompressPCX256_special(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
              IN UINT8 MaskColor);
              
EFI_STATUS ShowBMP256(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowBMP24True(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowPCX24True(CHAR16 *fileName,UINTN x,UINTN y);
#endif