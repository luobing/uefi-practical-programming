/** @file
    A simple, basic, application showing how the Hello application could be
    built using the "Standard C Libraries" from StdLib.

    Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
extern "C"
{

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include "Common.h"
#include "Graphic.h"
#include "Window.h"
}

#include "Cppglobal.h"
// #include "GuiLite.h"


 #define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))

extern "C" EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
//Graphics API
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
	color.Blue = (UINT8)rgb;
	color.Green = (UINT8)(rgb >> 8);
	color.Red = (UINT8)(rgb >> 16);
	color.Reserved = 0;

	putpixel(x, y, &color);

	return;
}

void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
	color.Blue = (UINT8)rgb;
	color.Green = (UINT8)(rgb >> 8);
	color.Red = (UINT8)(rgb >> 16);
	color.Reserved = 0;

	rectblock(x0, y0, x1, y1, &color);

	return;
}
//Implement it, if you have more fast solution than drawing pixels one by one.
//void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb){}

//UI entry
struct EXTERNAL_GFX_OP
{
	void (*draw_pixel)(int x, int y, unsigned int rgb);
	void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;
extern "C" void startHelloTimer(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);

VOID WaitKey(VOID);

// testClass t2;
/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int
main (
  IN int Argc,
  IN char **Argv
  )
{
	UINT64 flag;

	__do_global_ctors_aux();

	gST->ConOut->OutputString(gST->ConOut,(CHAR16 *)L"========= EDKII Test Samples ========\n\r");
	gST->ConOut->OutputString(gST->ConOut,(CHAR16 *)L"Author: luobing  \n\r");
	gST->ConOut->OutputString(gST->ConOut,(CHAR16 *)L"Data: 2020-3-1 11:57:51\n\r");
	gST->ConOut->OutputString(gST->ConOut,(CHAR16 *)L"Context: uefi & c++  --\n\r");
	gST->ConOut->OutputString(gST->ConOut,(CHAR16 *)L"======================================\n\r");
	WaitKey();
	flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO);
	Print((CHAR16 *)L"flag=%x\n",flag);
	//1 init graphics
	SwitchGraphicsMode(TRUE);
    SetMyMode(0x3);
	SetBKG(&(gColorTable[3]));
	WaitKey();

	// testClass t;
	// Print((CHAR16 *)L"This is a sample of uefi(c++)!\n");	
	my_gfx_op.draw_pixel = gfx_draw_pixel;
	my_gfx_op.fill_rect =  gfx_fill_rect;//gfx_fill_rect;
	startHelloTimer(NULL, 240, 320, 2, &my_gfx_op);
	while(1);

	__do_global_dtors_aux();
	return 0;
}

//Function Name: WaitKey
//Input: none
//Output: none
//Description: Wait any key press
VOID WaitKey(VOID)
{
  EFI_INPUT_KEY     Key;
  UINTN Index;

  // flush keyboard buffer
  while ( gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS );
  gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
  return;
}