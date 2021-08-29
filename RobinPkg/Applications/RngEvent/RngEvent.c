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
#include "FileRW.h"
#include "Keyboard.h"
#include "Graphic.h"
#include "Window.h"
#include "PciRW.h"

#include  <Uefi.h>
#include  <Library/UefiLib.h>
// #include  <Library/ShellCEntryLib.h>


extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
INT32 robin_rand(); //robin's random function
VOID NotifyWaitFunc(IN EFI_EVENT Event, IN VOID *Context);
VOID NotifySignalFunc(IN EFI_EVENT Event, IN VOID *Context);
VOID TestNotifyEvent(VOID);
VOID RandomBlock(UINT32 Width,UINT32 Height,EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltArray,UINT32 BltArraySize);


/***
  Establishes the main structure of the application.

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

    
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO | RNG_OUT);
	Print(L"flag=%x\n",flag);
	WaitKey();
  // TestNotifyEvent();
	// WaitKey();	
 
  SwitchGraphicsMode(TRUE);
	SetBKG(&(gColorTable[3]));
  RandomBlock(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,gColorTable,10);

  SetMyMode(0x0);
	SwitchGraphicsMode(FALSE);
  return(0);
}

VOID RandomBlock(UINT32 Width,UINT32 Height,EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltArray,UINT32 BltArraySize)
{
  //随机显示图形块测试
  EFI_EVENT myEvent;
  EFI_STATUS Status;
  UINTN       repeats=0;
  UINTN       index=0;
  UINT32 rand_x1, rand_y1,randColor;
  
  //create timer event and settimer
  Status = gBS->CreateEvent(EVT_TIMER  , TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &myEvent);
  Status = gBS->SetTimer(myEvent,TimerPeriodic , 2 * 1000 * 1000); //triggered per 200ms
  while(1)
  {
    Status = gBS->WaitForEvent(1, &myEvent, &index);
    //do something
    rand_x1 = (robin_rand() % Width);
    rand_y1 = (robin_rand() % Height);
    randColor = (robin_rand()) % BltArraySize;     
    rectblock(rand_x1,rand_y1,rand_x1+20,rand_y1+20,&(BltArray[randColor]));
    if(repeats ++ == 100){
          break;
      }
  }
  Status = gBS->CloseEvent(myEvent);
}

VOID TestNotifyEvent(VOID)
{
	EFI_EVENT myWaitEvent;
  EFI_EVENT mySignalEvent;
	UINTN       index=0;
  EFI_STATUS Status;

	Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_NOTIFY, (EFI_EVENT_NOTIFY)NotifyWaitFunc, (VOID*)L"Hi,UEFI!", &myWaitEvent);
	Print(L"CreateEvent myWaitEvent=%r\n",Status);
  Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, (EFI_EVENT_NOTIFY)NotifySignalFunc, (VOID*)L"Hi,me!", &mySignalEvent);
  Print(L"CreateEvent mySignalEvent=%r\n",Status);
	Status = gBS->WaitForEvent(1, &myWaitEvent, &index);
  Print(L"myWaitEvent is signaled! index=%d %r\n",index,Status);
  gBS->SignalEvent(mySignalEvent);
  Status = gBS->WaitForEvent(1, &mySignalEvent, &index);
  Print(L"mySignalEvent is signaled! index=%d %r\n",index, Status);
  gBS->CloseEvent(myWaitEvent);
  gBS->CloseEvent(mySignalEvent);
}

VOID NotifyWaitFunc(IN EFI_EVENT Event, IN VOID *Context)
{
  static UINTN count = 0;
  Print(L"NotifyWaitFunc: count=%d, Context=%s\n",count,Context);
  count++;
  if((count%5)==0)
    gBS->SignalEvent(Event);
    
}

VOID NotifySignalFunc(IN EFI_EVENT Event, IN VOID *Context)
{
  Print(L"NotifySignalFunc: Context=%s\n",Context);
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

INT32 robin_rand(VOID)
{
  INT32 hi, lo, x;
  static UINT32 next = 1;
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
