//code by luobing 2019-6-7 21:52:13
#ifndef _HIIFONT_H
#define _HIIFONT_H
#include "Common.h"
EFI_STATUS TestStrPackage(void);
EFI_STATUS TestLanguage(EFI_HANDLE HiiHandle);
EFI_STATUS TestString(EFI_HANDLE HiiHandle );

EFI_STATUS LoadSimpleFont(void);
EFI_STATUS CreateSimpleFontPkg(EFI_NARROW_GLYPH* NarrowGlyph, UINT32 nNarrow, EFI_WIDE_GLYPH* WideGlyph, UINT32 nWide);

EFI_STATUS LoadFont(void);
EFI_STATUS putHiiFontStr(UINTN x,UINTN y,EFI_STRING String,CHAR16* fontName,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *FGColor,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BGColor);
EFI_STATUS FillNarrowGLYPH(UINT8* p, 
        EFI_NARROW_GLYPH* NarrowGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 NrStart, CHAR16 NrCharNum);
EFI_STATUS FillWideGLYPH(UINT8* p, 
        EFI_WIDE_GLYPH* WideGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum);
EFI_STATUS FillGLYPH(UINT8* p, 
        UINT8* Glyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum);        
EFI_STATUS CreateMyFontPkg(CHAR16* FontName, UINT16 FontWidth, UINT16 FontHeight,  
        EFI_NARROW_GLYPH* NarrowGlyph, UINT32 NrSizeInBytes, CHAR16 NrStart, CHAR16 NrCharNum,
        EFI_WIDE_GLYPH* WideGlyph, UINT32 SizeInBytes, CHAR16 Start, CHAR16 CharNum);      
        



#endif