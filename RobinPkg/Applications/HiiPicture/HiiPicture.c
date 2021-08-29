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
//#include  <Uefi.h>
//#include  <Library/UefiLib.h>
//#include  <Library/ShellCEntryLib.h>
//所有包含的目录到mdepkg下去找

#include "common.h"
#include "Keyboard.h"
#include "Graphic.h"
#include "Window.h"
#include "HiiFont.h"
#include "HiiImage.h"
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <wchar.h>

extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];

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
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
	UINT64 flag;
	EFI_STATUS Status;
	EFI_HII_HANDLE imgPL;
	EFI_GUID Img_GUID = {0xedd31eaf, 0xf232, 0xc24e, 0xa2, 0xe4, 0xde, 0xf7, 0xde, 0xcd, 0xcd, 0xee};  

	flag=InintGloabalProtocols(S_TEXT_INPUT_EX|GRAPHICS_OUTPUT|HII_FONT|HII_IMAGE|HII_IMAGE_EX|HII_IMAGE_DECODER);
	Print(L"flag=0x%x\n",flag);	
	//进入图形模式，并设置背景
	SwitchGraphicsMode(TRUE);
	SetBKG(&(gColorTable[3]));
	WaitKey();
	
	Status = HiiImgpackRegister(Img_GUID,&imgPL);
	if(EFI_ERROR(Status))
	{
		Print(L"HiiImgpackRegister: %r\n",Status);
	}
	else
	{
		ShowHiiImage(imgPL,IMAGE_TOKEN(IMG_24TB),100,100);
		ShowHiiImage(imgPL,IMAGE_TOKEN(IMG_256B),300,300);
	}
	
	
	// TestHiiImage(Img_GUID1,IMAGE_TOKEN(IMG_BMP),100,100);
	// TestHiiImage(Img_GUID2,IMAGE_TOKEN(IMG_BMP1),310,310);
	// TestHiiImage(Img_GUID3,IMAGE_TOKEN(IMG_JPG),00,00);
	// TestHiiImage(IMAGE_TOKEN(IMG_PNG),300,300);
	// TestHiiImage(IMAGE_TOKEN(IMG_JPG),500,400);
	WaitKey();

	SetMyMode(0x0);
	SwitchGraphicsMode(FALSE);

	return 0;
}

