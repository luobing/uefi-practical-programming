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
#include "Common.h"
#include "Keyboard.h"
#include "Graphic.h"
#include "Font.h"
#include "Window.h"

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>


extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
INT32 robin_rand(); //robin's random function
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
    
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT);
	Print(L"flag=%x\n",flag);
	WaitKey();
  

  {
		//图形显示测试
		SwitchGraphicsMode(TRUE);
		SetBKG(&(gColorTable[3]));
    {
      //test
      UINT8 *s_text =(UINT8 *) "1.水光潋滟晴方好";
      draw_string(s_text, 80, 60, &MyFontArray, &(gColorTable[8]));
      draw_string(s_text, 80, 100, &MyFontArray, &(gColorTable[7]));
      draw_string(s_text, 80, 140, &MyFontArray, &(gColorTable[6]));

    }
		WaitKey();
  }
  
  return(0);

}




/** Compute a pseudo-random number.
  *
  * Compute x = (7^5 * x) mod (2^31 - 1)
  * without overflowing 31 bits:
  *      (2^31 - 1) = 127773 * (7^5) + 2836
  * From "Random number generators: good ones are hard to find",
  * Park and Miller, Communications of the ACM, vol. 31, no. 10,
  * October 1988, p. 1195.
**/
// #define RAND_MAX  0x7fffffff
static UINT32 next = 1;
INT32 robin_rand()
{
  INT32 hi, lo, x;

  /* Can't be initialized with 0, so use another value. */
  if (next == 0)
    next = 123459876;
  hi = next / 127773;
  lo = next % 127773;
  x = 16807 * lo - 2836 * hi;
  if (x < 0)
    x += 0x7fffffff;
  return ((next = x) % ((UINT32)0x7fffffff + 1));
}