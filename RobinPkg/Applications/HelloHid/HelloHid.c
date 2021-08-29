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

BOOLEAN findMyHidDevice(OUT INT16 *index, IN UINT16 MyVID, IN UINT16 MyPID);
VOID Endpoint_OutIn(IN INT16 index);
VOID Output_Input_Report(IN INT16 index);
VOID Feature_Report(IN INT16 index);

// EFI_STATUS MyHidTransfer(
//   IN VOID *Data,
//   IN UINTN DataLength,
//   IN VOID *Context,
//   IN UINT32 Result
// );
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
  BOOLEAN retVal;
  UINT16 myDevice;
  // UINTN i;
  // EFI_STATUS Status;
  // CHAR16 buff[200];
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
  retVal = findMyHidDevice(&myDevice,0x8765,0x4321);
  if(retVal)
  {
    Feature_Report(myDevice);
    Delayms(1000);
    Output_Input_Report(myDevice);
    Delayms(1000);
    Endpoint_OutIn(myDevice);
  }
  else
  {
    Print(L"MyHidDevice is not exist!\n");
  }
  

  return(0);

}

//找到需要通信的设备，通过vid和pid来确定
//在gUsbIO[256]中寻找指定的设备，并通过指定参数index返回
//Name: findMyHidDevice
//Input: *index,MyVID,MyPID
//Output: true: found device,the index of gUsbIO array save in *index
//        false: not found device
BOOLEAN findMyHidDevice(OUT INT16 *index, IN UINT16 MyVID, IN UINT16 MyPID)
{
  EFI_STATUS Status;
  INT16 i;
  EFI_USB_DEVICE_DESCRIPTOR     UsbDevDesc;

  if(gUsbIOCount == 0)
    return FALSE;
  // {
  //   Print(L"Not found Usb Device.\n");
  //   return FALSE;
  // }

  for(i=0;i<(UINT16)gUsbIOCount;i++)
  {
    Status = gUsbIO[i]->UsbGetDeviceDescriptor(gUsbIO[i], &UsbDevDesc);     
    if(Status == EFI_SUCCESS)
    {
      if((UsbDevDesc.IdVendor == MyVID) && (UsbDevDesc.IdProduct == MyPID))
      {
        *index = i;
        return TRUE;
      }
    }
  }
  return FALSE;
}
//Name: Feature_Report
//Input: index
//Output:  
VOID Feature_Report(IN INT16 index)
{
  EFI_STATUS Status;
  UINT8   ReportId;
  UINT8 myBuffer[16];
  INTN i;

  gBS->SetMem(myBuffer,16,0xA0);
  ReportId = 0;
  Status = UsbSetReportRequest(
    gUsbIO[index],
    0,               //interface,
    ReportId,
    HID_FEATURE_REPORT,
    16,
    myBuffer
  );
  if(EFI_ERROR(Status))
  {
    Print(L"UsbSetReportRequest Error!\n");
    return;
  }
  gBS->SetMem(myBuffer,16,0x00);
  Status = UsbGetReportRequest(
    gUsbIO[index],
    0,               //interface,
    ReportId,
    HID_FEATURE_REPORT,
    16,
    myBuffer
  );
  if(EFI_ERROR(Status))
  {
    Print(L"UsbGetReportRequest Error!\n");
    return;
  }
  Print(L"Feature_Report,data from MyHidDevice:\n");
  for(i=0;i<16;i++)
    Print(L"0x%02x ",myBuffer[i]);
  Print(L"\n");
}

//Name: Output_Input_Report
//Input: index
//Output:  
VOID Output_Input_Report(IN INT16 index)
{
  EFI_STATUS Status;
  UINT8   ReportId;
  UINT8 myBuffer[16];
  INTN i;

  gBS->SetMem(myBuffer,16,0xA0);
  ReportId = 0;
  Status = UsbSetReportRequest(
    gUsbIO[index],
    0,               //interface,
    ReportId,
    HID_OUTPUT_REPORT,
    16,
    myBuffer
  );
  if(EFI_ERROR(Status))
  {
    Print(L"UsbSetReportRequest Error!\n");
    return;
  }
  gBS->SetMem(myBuffer,16,0x00);
  Status = UsbGetReportRequest(
    gUsbIO[index],
    0,               //interface,
    ReportId,
    HID_INPUT_REPORT,
    16,
    myBuffer
  );
  if(EFI_ERROR(Status))
  {
    Print(L"UsbGetReportRequest Error!\n");
    return;
  }
  Print(L"Output_Input_Report,data from MyHidDevice:\n");
  for(i=0;i<16;i++)
    Print(L"0x%02x ",myBuffer[i]);
  Print(L"\n");
}


// EFI_STATUS MyHidTransfer(
//   IN VOID *Data,
//   IN UINTN DataLength,
//   IN VOID *Context,
//   IN UINT32 Result
// )
// {
//   UINTN i;
//   UINT8 *buff=(UINT8 *)Context;

//   if(EFI_ERROR(Result))
//   {
//     Print(L"MyHidTransfer: %r",Result);
//     return EFI_DEVICE_ERROR;
//   }
//   Print(L"MyHidTransfer:DataLength = %d\n",DataLength);
//   for(i=0;i<DataLength;i++)
//     Print(L"0x%02x ",buff[i]);
//   Print(L"\n");
// }
//此函数测试不成功???:?? 2021年5月5日19:43:24
//发现是机器的问题
VOID Endpoint_OutIn(IN INT16 index)
{
  EFI_STATUS Status;
  // UINT8   ReportId;
  UINT8 myBuffer[16];
  UINTN lenBuffer;
  UINTN i;
  UINT32 result;

  gBS->SetMem(myBuffer,16,0xA0);
  lenBuffer=16;
  Status = gUsbIO[index]->UsbSyncInterruptTransfer(
    gUsbIO[index],
    0x01, //EP1 OUT
    myBuffer,
    &lenBuffer,
    32,
    &result
  );
  if(EFI_ERROR(Status))
  {
    Print(L"OUT:UsbSyncInterruptTransfer Error!\n");
    Print(L"Status:%r\n",Status);
    return;
  }
  if(EFI_ERROR(result))
  {
    Print(L"UsbSyncInterruptTransfer result:%r\n",result);
    Print(L"\n");
    return;
  }
  
  gBS->SetMem(myBuffer,16,0x00);
  Status = gUsbIO[index]->UsbSyncInterruptTransfer(
    gUsbIO[index],
    0x81, //EP1 IN
    myBuffer,
    &lenBuffer,
    32,
    &result
  );
  if(EFI_ERROR(Status))
  {
    Print(L"IN:UsbSyncInterruptTransfer Error!\n");
    Print(L"Status:%r\n",Status);
    return;
  }
  if(EFI_ERROR(result))
  {
    Print(L"UsbSyncInterruptTransfer result:%r\n",result);
    Print(L"\n");
    return;
  }
  else
  {
    Print(L"Endpoint_OutIn,data from MyHidDevice=%d:\n",lenBuffer);
    for(i=0;i<lenBuffer;i++)
      Print(L"0x%02x ",myBuffer[i]);
    Print(L"\n");
  }
  
  // ReportId = 0;
//异步中断
  // Status = gUsbIO[index]->UsbAsyncInterruptTransfer(
  //   gUsbIO[index],
  //   0x01, //EP1 OUT
  //   TRUE,
  //   32,   //?? PollingInterval
  //   16,   //DataLength
  //   MyHidTransfer, //InterruptCallback
  //   myBuffer  //pass to MyHidTransfer()
  // );
  // if(EFI_ERROR(Status))
  // {
  //   Print(L"OUT:UsbAsyncInterruptTransfer Error!\n");
  //   Print(L"Status:%r\n",Status);
  //   return;
  // }
  // gBS->SetMem(myBuffer,16,0x00);
  // Status = gUsbIO[index]->UsbAsyncInterruptTransfer(
  //   gUsbIO[index],
  //   0x81, //EP1 IN
  //   TRUE,
  //   32,   //?? PollingInterval
  //   16,   //DataLength
  //   MyHidTransfer, //InterruptCallback
  //   myBuffer  //pass to MyHidTransfer()
  // );
  // if(EFI_ERROR(Status))
  // {
  //   Print(L"IN:UsbAsyncInterruptTransfer Error!\n");
  //   Print(L"Status:%r\n",Status);
  //   return;
  // }
  // Print(L"connectMyHidDevice1:Get data from MyHidDevice:\n");
  // for(i=0;i<16;i++)
  //   Print(L"0x%02x ",myBuffer[i]);
  // Print(L"\n");
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