/** @file
    A simple, basic, EDK II native, "hello" application to verify that
    we can build applications without LibC.

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


#include "common.h"
#include "FileRW.h"
#include "KeyBoard.h"
#include "Graphic.h"
#include "Window.h"
#include "PciRW.h"
#include "Pictures.h"

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/BaseLib.h>  //字符串转换
#include  <Library/ShellCEntryLib.h>

#include <Pi/PiFirmwareFile.h>


#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeServicesLib.h>

#define MyPic                      L"luobing.bmp"		//where the my bmp located 
// EFI_STATUS ShowBmp (void);


extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
// INTN
// EFIAPI
// ShellAppMain (
//   IN UINTN Argc,
//   IN CHAR16 **Argv
//   )
int
main (
  IN int Argc,
  IN char **Argv
  )
{
  UINT64 flag;
  
  // Print(L"Hello there fellow Programmer.\n");
  // Print(L"Welcome to the world of EDK II.\n");
  // Print(L"Argc=%d\n", Argc);
  // for (i = 0; i < Argc;i++)
  // {
  //   Print(L"Argv[%d]= %s\n", i,Argv[i]);
  // }
    
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO);
	Print(L"flag=%x\n",flag);
	
  SwitchGraphicsMode(TRUE);
  SetMyMode(0x3);
	SetBKG(&(gColorTable[3]));
  WaitKey();
  ShowJPEG24True(Argv[1],100,100);
    
  WaitKey();

	SetMyMode(0x0);
	SwitchGraphicsMode(FALSE);     
  
  return(0);
}




