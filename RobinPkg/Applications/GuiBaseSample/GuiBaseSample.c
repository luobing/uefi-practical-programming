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
extern EFI_SIMPLE_POINTER_PROTOCOL *gMouse;   //Mouse
INT32 robin_rand(); //robin's random function
VOID NotifyWaitFunc(IN EFI_EVENT Event, IN VOID *Context);
VOID NotifySignalFunc(IN EFI_EVENT Event, IN VOID *Context);
VOID TestNotifyEvent(VOID);
VOID RandomBlock(UINT32 Width,UINT32 Height,EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltArray,UINT32 BltArraySize);

EFI_STATUS HotKeySample(IN EFI_KEY_DATA *hotkey);
EFI_STATUS HotKeyNotifyFunc(IN EFI_KEY_DATA *hotkey);

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
  
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO | RNG_OUT | SIMPLE_POINTER);
	Print(L"flag=%x\n",flag);
	WaitKey();
  // TestNotifyEvent();
	// WaitKey();	
  // {
  //   EFI_KEY_DATA myhotkey={0,0};
  //   myhotkey.Key.ScanCode=0;
  //   myhotkey.Key.UnicodeChar='a';
  //   myhotkey.KeyState.KeyShiftState=EFI_LEFT_CONTROL_PRESSED;
  //   myhotkey.KeyState.KeyToggleState=0;
  //   HotKeySample(&myhotkey);
  // }
  //Mouse
  if((flag&SIMPLE_POINTER) != SIMPLE_POINTER)
  {
    //1 list mouse'mode
    EFI_SIMPLE_POINTER_STATE State;
    UINTN Index;
    Print(L"Print Current Mode of Mouse:\n");
    Print(L"::ResolutionX=0x%x\n",gMouse->Mode->ResolutionX);
    Print(L"::ResolutionY=%d\n",gMouse->Mode->ResolutionY);
    Print(L"::ResolutionZ=%d\n",gMouse->Mode->ResolutionZ);
    Print(L"::LeftButton=%d\n",gMouse->Mode->LeftButton);
    Print(L"::RightButton=%d\n",gMouse->Mode->RightButton);
    //2 get mouse's state
    while(1)
    {
      gMouse->GetState(gMouse,&State);
      Print(L">>RelativeMovementX=0x%x\n",State.RelativeMovementX);
      Print(L">>RelativeMovementY=0x%x\n",State.RelativeMovementY);
      Print(L">>RelativeMovementZ=0x%x\n",State.RelativeMovementZ);
      Print(L">>LeftButton=0x%x\n",State.LeftButton);
      Print(L">>RightButton=0x%x\n",State.RightButton);
      Print(L"--------- ---------- ----------\n");
      gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
    }
  }
  else
    Print(L"Load Mouse Protocol Error!\n");
  
  
  // SwitchGraphicsMode(TRUE);
	// SetBKG(&(gColorTable[3]));
  
  // SetMyMode(0x0);
	// SwitchGraphicsMode(FALSE);
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

//--------------hot key sample---------------------------
extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;

EFI_STATUS HotKeySample(IN EFI_KEY_DATA *hotkey)
{
  EFI_STATUS Status;
  EFI_KEY_DATA key;
  EFI_HANDLE hotkeyNotifyHandle;
  
  hotkey->KeyState.KeyShiftState|=EFI_SHIFT_STATE_VALID;
  hotkey->KeyState.KeyToggleState|=EFI_TOGGLE_STATE_VALID|EFI_KEY_STATE_EXPOSED;

  Status = gSimpleTextInputEx->RegisterKeyNotify(gSimpleTextInputEx,
                                                hotkey,
                                                HotKeyNotifyFunc,
                                                (VOID **)&hotkeyNotifyHandle);
  Print(L"RegisterKeyNotify=%r\n",Status);

  while(key.Key.ScanCode!=0x17)
  {
    UINTN index;
    gBS->WaitForEvent(1,&(gSimpleTextInputEx->WaitForKeyEx),&index);
    Status = gSimpleTextInputEx->ReadKeyStrokeEx(gSimpleTextInputEx,&key);
    Print(L"key.Key.UnicodeChar=%x\n",key.Key.UnicodeChar);
  }

  Status = gSimpleTextInputEx->UnregisterKeyNotify(gSimpleTextInputEx,hotkeyNotifyHandle);

  return EFI_SUCCESS;
}

EFI_STATUS HotKeyNotifyFunc(IN EFI_KEY_DATA *hotkey)
{
  Print(L"Hot key pressed!\n");
  return EFI_SUCCESS;
}