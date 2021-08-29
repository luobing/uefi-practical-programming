//code by luobing 2019-6-7 21:52:13
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "HiiFont.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutput;
extern EFI_HII_FONT_PROTOCOL *gHiiFont;
//SimpleFont
extern EFI_WIDE_GLYPH gSimpleFontWideGlyphData[];
extern UINT32 gSimpleFontWideBytes;
extern EFI_NARROW_GLYPH gSimpleFontNarrowGlyphData[];
extern UINT32 gSimpleFontNarrowBytes;
static EFI_GUID gSimpleFontPackageListGuid = {0xf6643673, 0x6006, 0x3c38, {0x5c, 0xcd, 0xda, 0x1a, 0xeb, 0x3b, 0x26, 0xa2}};

//Font
extern EFI_WIDE_GLYPH gDMFontWideGlyphData[];
extern UINT32 gDMFontBytes;
extern EFI_NARROW_GLYPH gMyNarrowGlyphData[];
extern UINT32 nMyNarrowGlyphSize;
extern EFI_WIDE_GLYPH gMyWideGlyphData[];
extern UINT32 nMyWideGlyphSize;
static EFI_GUID gDMFontPackageListGuid = {0xf6645673, 0x6106, 0x3138, {0x5c, 0xcd, 0xda, 0x1a, 0xab, 0x3b, 0x26, 0xa2}};
#define WfontWidth 16
#define WfontHight 19
CONST UINT16 WideGlyphBytes = WfontHight * WfontWidth / 8;
CONST UINT16 NrGlyphBytes = 19;

//============================== Hii Function begin==============================================
//ugly codes...
//必须先注册汉字simplefont库，才能显示汉字
EFI_STATUS TestStrPackage(void)
{
    EFI_STATUS Status = 0;
    EFI_GUID mStrPackageGuid = {0xedd31def, 0xf262, 0xc24e, 0xa2, 0xe4, 0xde, 0xf7, 0xde, 0xcd, 0xcd, 0xee};
    EFI_HANDLE HiiHandle = HiiAddPackages(&mStrPackageGuid, gImageHandle, HiiCHSStrings, NULL);
    Status = TestLanguage(HiiHandle);
    Status = TestString(HiiHandle);
    return Status;
}
//ugly codes...
EFI_STATUS TestLanguage(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = 0;
    CHAR8 buf[256];
    CHAR16 buf16[256];
    UINTN LangSize = 256;
    UINTN i = 0;

    Status = gHiiString->GetLanguages(
        gHiiString,
        HiiHandle,
        buf,
        &LangSize);
    for (i = 0; i < LangSize; i++)
    {
        buf16[i] = buf[i];
    }
    buf16[LangSize] = 0;
    Print(L"Support Language: %s\n", buf16);
    return Status;
}
//ugly codes...
EFI_STATUS TestString(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = 0;
    //CHAR8* BestLanguage = "en-US";
    CHAR8 *BestLanguage = "zh-Hans";
    EFI_STRING TempString = NULL;
    UINTN StringSize = 0;
    Status = gHiiString->GetString(
        gHiiString,
        BestLanguage,
        HiiHandle,
        STRING_TOKEN(STR_HELLOWORLD),
        TempString,
        &StringSize,
        NULL);
    //gBS->AllocatePool(EfiBootServicesData,0x100,(void**)&buffer);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, StringSize, (void **)&TempString);
        if (EFI_ERROR(Status))
            return EFI_BUFFER_TOO_SMALL;
        Status = gHiiString->GetString(
            gHiiString,
            BestLanguage,
            HiiHandle,
            STRING_TOKEN(STR_HELLOWORLD),
            TempString,
            &StringSize,
            NULL);
        Print(L"%s\n", TempString);
        gBS->FreePool(TempString);
    }
    else
    {
        Print(L"GetString %r\n", Status);
    }
    return 0;
}
//----------------------------------- Hii SimpleFont -----------------------
/***
  注册SimpleFont点阵库

  @param[in/out]  void
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS LoadSimpleFont(void)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *handles = 0; //for simpleFont of Hii
    handles = HiiGetHiiHandles(&gSimpleFontPackageListGuid);
    if (handles == 0)
    {
        Status = CreateSimpleFontPkg(gSimpleFontNarrowGlyphData, gSimpleFontNarrowBytes, gSimpleFontWideGlyphData, gSimpleFontWideBytes);
        Print(L"LoadSimpleFont:CreateSimpleFontPkg= %r\n", Status);
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreatesimpleFontPkg()  handles==0\n\r");
        //return Status;
    }
    else
    {
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreatesimpleFontPkg()  handles==1\n\r");
        FreePool(handles);
        return EFI_LOAD_ERROR; //refer to EfiError.h
    }

    return Status;
}

/***
  构建SimpleFont资源包，并向Hii数据库注册

  @param[in]  NarrowGlyph   窄字符字模信息
  @param[in]  nNarrow       窄字符数组字节长
  @param[in]  WideGlyph     宽字符字模信息 
  @param[in]  nWide         宽字符数组字节长
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS CreateSimpleFontPkg(EFI_NARROW_GLYPH *NarrowGlyph, UINT32 nNarrow, EFI_WIDE_GLYPH *WideGlyph, UINT32 nWide)
{
    // EFI_STATUS Status;
    EFI_HII_SIMPLE_FONT_PACKAGE_HDR *simpleFont;
    UINT8 *Package;
    UINT8 *Location = NULL;
    UINT32 packageLen = sizeof(EFI_HII_SIMPLE_FONT_PACKAGE_HDR) + nNarrow + nWide + 4;

    Package = (UINT8 *)AllocateZeroPool(packageLen);
    WriteUnaligned32((UINT32 *)Package, packageLen);
    simpleFont = (EFI_HII_SIMPLE_FONT_PACKAGE_HDR *)(Package + 4);
    simpleFont->Header.Length = (UINT32)(packageLen - 4);
    simpleFont->Header.Type = EFI_HII_PACKAGE_SIMPLE_FONTS;
    // simpleFont->NumberOfNarrowGlyphs = 0;
    simpleFont->NumberOfNarrowGlyphs = (UINT16)(nNarrow / sizeof(EFI_NARROW_GLYPH));
    simpleFont->NumberOfWideGlyphs = (UINT16)(nWide / sizeof(EFI_WIDE_GLYPH));
    Location = (UINT8 *)(&simpleFont->NumberOfWideGlyphs + 1); //放到此处赋值，需要对齐
    CopyMem(Location, NarrowGlyph, nNarrow);
    Location += nNarrow;
    CopyMem(Location, WideGlyph, nWide);
    //
    // Add this simplified font package to a package list then install it.
    //
    EFI_HII_HANDLE simpleFontHiiHandle = HiiAddPackages(
        &gSimpleFontPackageListGuid,
        NULL,
        Package,
        NULL);
    if (simpleFontHiiHandle == NULL)
    {
        return EFI_NOT_FOUND; //查看了EfiError.h,选择这个返回值
    }
    FreePool(Package);
    return EFI_SUCCESS;
}

//----------------------------------- Hii Font -----------------------
/***
  注册Font点阵库

  @param[in/out]  void
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS LoadFont(void)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *handles1 = 0; //for Font of Hii
    handles1 = HiiGetHiiHandles(&gDMFontPackageListGuid);
    if (handles1 == 0)
    {
        //extern CHAR16* FontName ;
        CHAR16 *FontName = (CHAR16 *)L"LUOBING"; //随便取个名字
        Status = CreateMyFontPkg(FontName, 16, 19,
                                 gMyNarrowGlyphData, nMyNarrowGlyphSize, 32, (CHAR16)(nMyNarrowGlyphSize / sizeof(EFI_NARROW_GLYPH)),
                                 gMyWideGlyphData, nMyWideGlyphSize, 0x4e00, (CHAR16)(nMyWideGlyphSize / sizeof(EFI_WIDE_GLYPH)));
        Print(L"LoadFont:CreateMyFontPkg= %r\n", Status);
    }
    else
    {
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreateMyFontPkg()  handles1==1\n\r");
        FreePool(handles1);
        return EFI_LOAD_ERROR; //refer to EfiError.h
    }

    return Status;
}


EFI_STATUS FillNarrowGLYPH(UINT8 *p, EFI_NARROW_GLYPH *NarrowGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 NrStart, CHAR16 NrCharNum)
{
    UINT8 *BitmapData = NULL;
    UINTN Length = 0;
    EFI_HII_GLYPH_INFO Cell = {8, 19, 0, 0, (INT16)8};
    EFI_HII_GIBT_GLYPHS_BLOCK *GlyphsBlock; //2019-6-7 11:59:05 luobing ammend

    // SKIP
    if (Next != NrStart)
    {
        EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
        FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block->SkipCount = NrStart - Next;

        p = (UINT8 *)(FontSkip2Block + 1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    //    EFI_HII_GIBT_GLYPHS_BLOCK * GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS;
    GlyphsBlock->Cell = Cell;
    GlyphsBlock->Count = NrCharNum;
    BitmapData = GlyphsBlock->BitmapData;
    {
        UINT16 i = 0, j = 0;
        for (i = 0; i < NrCharNum; i++)
        {
            for (j = 0; j < 19; j++)
            {
                BitmapData[j] = NarrowGlyph[i].GlyphCol1[j];
            }
            BitmapData += 19;
        }
    }
    Length += (sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) + 19 * NrCharNum - 1);
    return (EFI_STATUS)Length;
}

EFI_STATUS FillWideGLYPH(UINT8 *p, EFI_WIDE_GLYPH *WideGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
{
    UINT8 *BitmapData = NULL;
    UINTN Length = 0;
    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *GlyphsDefaultBlock; //2019-6-7 11:58:55 luobing ammend

    // SKIP
    if (Next != Start)
    {
        EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
        FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block->SkipCount = Start - Next;

        p = (UINT8 *)(FontSkip2Block + 1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    //    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
    GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)(p);
    GlyphsDefaultBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
    GlyphsDefaultBlock->Count = CharNum;
    BitmapData = GlyphsDefaultBlock->BitmapData;
    {
        UINT16 i = 0, j = 0;
        for (i = 0; i < CharNum; i++)
        {
            for (j = 0; j < WideGlyphBytes; j++)
            {
                BitmapData[j] = WideGlyph[i].GlyphCol1[(j % 2) * 19 + j / 2];
            }
            BitmapData += WideGlyphBytes;
        }
    }
    Length += (sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + WideGlyphBytes * CharNum - 1);
    return (EFI_STATUS)Length;
}

// EFI_STATUS FillGLYPH(UINT8 *p,
//                      UINT8 *Glyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
// {
//     UINT8 *BitmapData = NULL;
//     UINTN Length = 0;
//     EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *GlyphsDefaultBlock; //2019-6-7 11:59:45 luobing ammend
//     // SKIP
//     if (Next != Start)
//     {
//         EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
//         FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
//         FontSkip2Block->SkipCount = Start - Next;

//         p = (UINT8 *)(FontSkip2Block + 1);
//         Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
//     }

//     //
//     //    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
//     GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)(p);
//     GlyphsDefaultBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
//     GlyphsDefaultBlock->Count = CharNum;
//     BitmapData = GlyphsDefaultBlock->BitmapData;
//     CopyMem(BitmapData, Glyph, SizeInBytes);
//     Length += (sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + SizeInBytes - 1);
//     return (EFI_STATUS)Length;
// }
/***
  构建Font资源包，并向Hii数据库注册
  @param[in]  FontName      字体名字
  @param[in]  FontWidth     字体宽度
  @param[in]  FontHeight    字体高度
  @param[in]  NarrowGlyph   窄字符字模信息
  @param[in]  NrSizeInBytes 窄字符数组字节长
  @param[in]  NrStart       窄字符起始字符
  @param[in]  NrCharNum     窄字符数目
  @param[in]  WideGlyph     宽字符字模信息 
  @param[in]  SizeInBytes   宽字符数组字节长
  @param[in]  Start         宽字符起始字符
  @param[in]  CharNum       宽字符数目
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
  注意： Font资源包中，不存在窄字符和宽字符的概念，本函数主要是借用了SimpleFont的字模信息，
        构建了Font资源包而已。
***/
EFI_STATUS CreateMyFontPkg(CHAR16 *FontName, UINT16 FontWidth, UINT16 FontHeight,
                           EFI_NARROW_GLYPH *NarrowGlyph, UINT32 NrSizeInBytes, CHAR16 NrStart, CHAR16 NrCharNum,
                           EFI_WIDE_GLYPH *WideGlyph, UINT32 SizeInBytes, CHAR16 Start, CHAR16 CharNum)
{
    EFI_HII_FONT_PACKAGE_HDR *FontPkgHeader; //Font 专用的数据结构
    UINT32 PackageLength;
    UINT8 *Package;
    UINTN BlockLength = 0;
    UINT8 *pCurrent = 0;
    CHAR16 NextChar = 0;
    EFI_GLYPH_GIBT_END_BLOCK *FontEndBlock; //2019-6-7 12:01:37 luobing ammend
    EFI_HII_GLYPH_INFO Cell;
    //    EFI_HII_GLYPH_INFO Cell = {FontWidth, FontHeight, 10, 10, (INT16)FontWidth};  //2019-6-7 12:02:37 luobing: build error

    UINT16 FontNameLen = (UINT16)StrLen(FontName) * 2 + 2;
    //init Cell
    Cell.Width = FontWidth;
    Cell.Height = FontHeight;
    Cell.OffsetX = 0;
    Cell.OffsetY = 0;
    Cell.AdvanceX = (INT16)FontWidth;

    PackageLength = 4 + sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen /*Max Length of Font Name*/) +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) - 1 + CharNum * WideGlyphBytes +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) - 1 + NrCharNum * NrGlyphBytes +
                    sizeof(EFI_GLYPH_GIBT_END_BLOCK);
    Package = (UINT8 *)AllocateZeroPool(PackageLength);
    //ASSERT (Package != NULL);

    // Header
    WriteUnaligned32((UINT32 *)Package, PackageLength);
    FontPkgHeader = (EFI_HII_FONT_PACKAGE_HDR *)(Package + 4);
    FontPkgHeader->Header.Length = (UINT32)(PackageLength - 4);
    FontPkgHeader->Header.Type = EFI_HII_PACKAGE_FONTS;
    FontPkgHeader->HdrSize = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameLen;
    FontPkgHeader->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameLen;
    FontPkgHeader->Cell = Cell;
    FontPkgHeader->FontStyle = EFI_HII_FONT_STYLE_NORMAL;
    CopyMem((FontPkgHeader->FontFamily), FontName, FontNameLen);

    pCurrent = (UINT8 *)(Package + 4 + FontPkgHeader->GlyphBlockOffset);

    //CHAR 0...255
    BlockLength = FillNarrowGLYPH(pCurrent, NarrowGlyph, NrSizeInBytes, 1, NrStart, NrCharNum);
    pCurrent += BlockLength;
    NextChar = NrStart + NrCharNum;
    // EFI_HII_GIBT_GLYPHS_DEFAULT
    BlockLength = FillWideGLYPH(pCurrent, WideGlyph, SizeInBytes, NextChar, Start, CharNum);
    pCurrent += BlockLength;
    // END BLOCK
    FontEndBlock = (EFI_GLYPH_GIBT_END_BLOCK *)(pCurrent);

    FontEndBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_END;

    //
    // Add this simplified font package to a package list then install it.
    //
    {
        EFI_HANDLE gFontHiiHandle = HiiAddPackages(
            &gDMFontPackageListGuid,
            NULL,
            Package,
            NULL);
        (void)gFontHiiHandle;
        //ASSERT (gFontHiiHandle != NULL);
    }
    FreePool(Package);
    return EFI_SUCCESS;
}

//----------------------------------- 字符串显示 -----------------------
/***
  图形模式下显示字符串
  @param[in]  x         显示位置X坐标
  @param[in]  y         显示位置Y坐标
  @param[in]  String    字符串
  @param[in]  FGColor   显示字符的前景色
  @param[in]  BGColor   显示字符的背景色
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS putHiiFontStr(UINTN x, UINTN y, EFI_STRING String, CHAR16 *fontName, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *FGColor, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BGColor)
{
    //gHiiFont and gGraphicsOutput has been loaded
    EFI_STATUS Status = 0;
    EFI_IMAGE_OUTPUT gSystemFrameBuffer;
    EFI_IMAGE_OUTPUT *pSystemFrameBuffer = &gSystemFrameBuffer;
    EFI_FONT_DISPLAY_INFO *fontDisplayInfo;

    if (fontName == NULL)
    {
        // fontDisplayInfo = NULL;
        fontDisplayInfo = (EFI_FONT_DISPLAY_INFO *)AllocateZeroPool(sizeof(EFI_FONT_DISPLAY_INFO) + 2);
        fontDisplayInfo->ForegroundColor = *FGColor;
        fontDisplayInfo->BackgroundColor = *BGColor;
        fontDisplayInfo->FontInfoMask = (EFI_FONT_INFO_ANY_FONT | EFI_FONT_INFO_ANY_SIZE);
        fontDisplayInfo->FontInfo.FontStyle = EFI_HII_FONT_STYLE_ITALIC; //特意设定Font资源包注册时没有使用的，显示时使用SimpleFont
        fontDisplayInfo->FontInfo.FontSize = 19;
        *(fontDisplayInfo->FontInfo.FontName) = 0x00;
    }
    else
    {
        fontDisplayInfo = (EFI_FONT_DISPLAY_INFO *)AllocateZeroPool(sizeof(EFI_FONT_DISPLAY_INFO) + StrLen(fontName) * 2 + 2);
        fontDisplayInfo->ForegroundColor = *FGColor;
        fontDisplayInfo->BackgroundColor = *BGColor;
        fontDisplayInfo->FontInfoMask = (EFI_FONT_INFO_ANY_FONT | EFI_FONT_INFO_ANY_SIZE);
        fontDisplayInfo->FontInfo.FontStyle = EFI_HII_FONT_STYLE_NORMAL;
        fontDisplayInfo->FontInfo.FontSize = 19;
        CopyMem(fontDisplayInfo->FontInfo.FontName, fontName, StrLen(fontName) * 2 + 2);
    }

    gSystemFrameBuffer.Width = (UINT16)gGraphicsOutput->Mode->Info->HorizontalResolution;
    gSystemFrameBuffer.Height = (UINT16)gGraphicsOutput->Mode->Info->VerticalResolution;
    gSystemFrameBuffer.Image.Screen = gGraphicsOutput;

    Status = gHiiFont->StringToImage(
        gHiiFont,
        EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
            EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
            EFI_HII_IGNORE_LINE_BREAK | EFI_HII_DIRECT_TO_SCREEN,
        String,
        fontDisplayInfo,
        &pSystemFrameBuffer,
        (UINTN)x,
        (UINTN)y,
        0,
        0,
        0);
    FreePool(fontDisplayInfo);
    return Status;
}