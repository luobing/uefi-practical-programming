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
extern EFI_USB_IO_PROTOCOL *gUsbIO[256];
extern UINTN gUsbIOCount;
extern EFI_USB2_HC_PROTOCOL *gUsb2HC[256];
extern UINTN gUsb2HCCount;
extern EFI_USBFN_IO_PROTOCOL *gUsbFnIO[256];
extern UINTN gUsbFnIOCount;

INT32 robin_rand(); //robin's random function
VOID Str2Upper(IN CHAR16 *upperBuff, IN CHAR16 *srcStr);
CHAR16 Char2Upper (IN CHAR16 Char);

VOID lsUsbIO(void);
VOID lsUsb2HC(void);
VOID lsUsbFnIO(void);
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
  // UINTN i;
  // EFI_STATUS Status;
  CHAR16 buff[200];
  // EFI_INPUT_KEY key={0,0};  
  // UINTN clrflag=0;
  // Print(L"Argc=%d\n",Argc);
  // for(i=0;i<Argc;i++)
  // {
  //   Print(L"Argv[%d]:%S\n",i,Argv[i]);
  //   Str2Upper(buff,Argv[i]);
  //   Print(L"buff:%S\n",buff);
  //   // StrCmp(L"hc",Argv[i])
  // }
  
  
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT |USB_IO |USB2_HC |USBFN_IO);
	Print(L"flag=%x\n",flag);
	WaitKey();
  if(Argc==2)
  {
    Str2Upper(buff,Argv[1]);
    if(StrCmp(L"HC",buff)==0)   //USB Host Controller 
    {
      // Print(L"HC\n");
      lsUsb2HC();
    }
    if(StrCmp(L"IO",buff)==0)  //USB IO
    {
      // Print(L"IO\n");
      lsUsbIO();     
    }
    if(StrCmp(L"FUNC",buff)==0)  //USB func io
    {
      // Print(L"IO\n");
      lsUsbFnIO();     
    }
  }
  else
  {
    Print(L"Syntax: ListUSB hc/io/func\n");
    return 0;
  }
    
  
  // {
	// 	//图形显示测试
	// 	SwitchGraphicsMode(TRUE);
	// 	SetBKG(&(gColorTable[DEEPBLUE]));
  //   {
  //     //test
  //     UINT8 *s_text = "Alasse' aure,";  //《魔戒》精灵语的 “你好，日安”
  //     UINT8 *s_text1 = "欢迎进入UEFI的世界！";
  //     UINT8 *s_text2 = "按'ESC'键退出此界面";
  //     UINT8 *ptr=(UINT8 *)s_text1;
	// 	  // Print(L"TestStr: %02x,%02x,%02x,%02x,%02x,%02x\n",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5]);
  //     draw_string(s_text, 110, 60, &MyFontArray, &(gColorTable[WHITE]));
  //     draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[WHITE]));
  //     draw_string(s_text2, 135, 140, &MyFontArray1, &(gColorTable[YELLOW]));
  //     while(key.ScanCode!=0x17)	//ESC
  //     {
  //       GetKey(&key);
  //       if(clrflag==0)
  //       {
  //         clrflag=1;
  //         draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[DEEPBLUE]));
  //       }
  //       else
  //       {
  //         clrflag=0;
  //         draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[WHITE]));
  //       }
  //     }
  //   }
	// 	// WaitKey();
  // }
  // SetMyMode(OldGraphicsMode);
  // SwitchGraphicsMode(FALSE);
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
//将字符改为大写字母
CHAR16 Char2Upper (IN CHAR16 Char)
{
  if (Char >= L'a' && Char <= L'z') {
    return (CHAR16) (Char - (L'a' - L'A'));
  }

  return Char;
}
//将字符串改为大写字母
VOID Str2Upper(IN CHAR16 *upperBuff, IN CHAR16 *srcStr)
{
  UINTN Length,i;

  Length = StrLen(srcStr);
  for(i=0; i<Length; i++)
    upperBuff[i] = Char2Upper(srcStr[i]);
  upperBuff[i] = L'\0';
}

//处理USBIO
VOID lsUsbIO(void)
{
  EFI_STATUS Status;
  UINTN i;
  EFI_USB_DEVICE_DESCRIPTOR     UsbDevDesc;

  if(gUsbIOCount == 0)
  {
    Print(L"Not found Usb Device.\n");
    return;
  }
  Print(L"Usb device: %d \n",gUsbIOCount);
  Print(L"No. DevClass SubClass IdVendor IdProduct \n");
          
  for(i=0;i<gUsbIOCount;i++)
  {
    Print(L"%03d ",i);
    Status = gUsbIO[i]->UsbGetDeviceDescriptor(gUsbIO[i], &UsbDevDesc);     
    if(EFI_ERROR(Status))
      Print(L"Get Device Descriptor Error!\n");
    else
    {
      Print(L"     %03d      %03d ",UsbDevDesc.DeviceClass,UsbDevDesc.DeviceSubClass);
      Print(L"  0x%04x    0x%04x\n",UsbDevDesc.IdVendor,UsbDevDesc.IdProduct);
    }   
  }
}
//处理USB hc
VOID lsUsb2HC(void)
{
  EFI_STATUS Status;
  UINTN i;
  UINT8 maxSpeed,portNumber,is64BC;
  EFI_USB_HC_STATE state;
  CHAR16 *speed[]={L"FULL ",L"LOW  ",L"HIGH ",L"SUPER"};
  CHAR16 *hcState[]={L"Halt",L"Operational",L"Suspend"};
  
  if(gUsb2HCCount == 0)
  {
    Print(L"Not found Usb Host Controller.\n");
    return;
  }
  Print(L"Usb HC: %d \n",gUsb2HCCount);
  Print(L"No. MaxSpeed PortNumber Is64BitCapable State\n");
  for(i=0;i<gUsb2HCCount;i++)
  {
    Print(L"%03d ",i);
    Status = gUsb2HC[i]->GetCapability(gUsb2HC[i],&maxSpeed,&portNumber,&is64BC);
    if(EFI_ERROR(Status))
      Print(L"???");
    else
    {
      Print(L"%*S %*d %*d ",8,speed[maxSpeed],10,portNumber,14,is64BC);
    }
    Status = gUsb2HC[i]->GetState(gUsb2HC[i],&state);
    if(EFI_ERROR(Status))
      Print(L" ???\n");
    else
    {
      Print(L"%S \n",hcState[state]);
    }
    
  }
  // Print(L"MaxSpeed:FULL-00  LOW-01  HIGH-02  SUPER-03\n");
}
//处理usb funciton io
VOID lsUsbFnIO(void)
{
  EFI_STATUS Status;
  UINT16 vid,pid;
  UINTN i;
  EFI_USBFN_DEVICE_INFO_ID id = EfiUsbDeviceInfoProductName;
  CHAR16 productName[256];
  UINTN LengthName=256;
  if(gUsbFnIOCount == 0)
  {
    Print(L"Not found Usb Func IO.\n");
    return;
  }
  Print(L"Usb FuncIO: %d \n",gUsbFnIOCount);
  Print(L"No. Vid    Pid \n");
  for(i=0;i<gUsbFnIOCount;i++)
  {
    Print(L"%03d ",i);
    Status = gUsbFnIO[i]->GetVendorIdProductId(gUsbFnIO[i],&vid,&pid);
    if(EFI_ERROR(Status))
      Print(L"Get FnIO GetVendorIdProductId  Error!\n");
    else
    {
      Print(L"0x%04x 0x%04x\n",vid,pid);
    }
    Status = gUsbFnIO[i]->GetDeviceInfo(gUsbFnIO[i],id,&LengthName,productName);
    if(EFI_ERROR(Status))
      Print(L"Get FnIO GetDeviceInfo  Error!\n");
    else
    {
      Print(L"  ProductName:%S\n",productName);
    }
  }
}