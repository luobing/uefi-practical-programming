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
#include  <Library/ShellCEntryLib.h>

#include <Pi/PiFirmwareFile.h>


#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeServicesLib.h>


extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  UINT64 flag;
  UINTN i;

  Print(L"Argc=%d\n", Argc);
  for (i = 0; i < Argc;i++)
  {
    Print(L"Argv[%d]= %s\n", i,Argv[i]);
  }
    
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO);
	Print(L"flag=%x\n",flag);
	WaitKey();
  SwitchGraphicsMode(TRUE);
  SetMyMode(0x3);
	SetBKG(&(gColorTable[3]));
  // ShowBMP256(Argv[1],100,150);
  ShowBMP24True(Argv[1],100,100);
  WaitKey();

    
  return(0);
}



