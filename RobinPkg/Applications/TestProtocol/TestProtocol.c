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
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include <Protocol/SerialIo.h>	//串口？

// #include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Protocol/PciIo.h>						//获取PciIO protocol所需
#include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
#include <IndustryStandard/Pci.h>  //pci访问所需的头文件，包含pci22.h,pci23.h...
#include <Protocol/Rng.h>  //Random Number Generator Protocol 2019-08-31 11:32:03 robin

#include <Protocol/SimpleFileSystem.h> //文件系统访问
#include <IndustryStandard/Bmp.h> //for bmp



EFI_STATUS ListProtocolMsg(IN EFI_GUID *ProtocolGuid, OUT VOID **Interface);
/***
  Print a welcoming message.

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
  EFI_RNG_PROTOCOL *gRNGOut;
  EFI_SERIAL_IO_PROTOCOL *gSerialIO;

  gST->ConOut->SetAttribute(gST->ConOut,EFI_BACKGROUND_BLACK|EFI_RED);  //黑底红字
  Print((const CHAR16*)L"Action: SerialIoProtocol\n");
  ListProtocolMsg(&gEfiSerialIoProtocolGuid,(VOID**)&gSerialIO); //串口Protocol
  gST->ConOut->SetAttribute(gST->ConOut,EFI_BACKGROUND_BLACK|EFI_CYAN);  //黑底青字
  Print((const CHAR16*)L"Action: RngProtocol\n");
  ListProtocolMsg(&gEfiRngProtocolGuid,(VOID**)&gRNGOut); //随机数Protocol
  gST->ConOut->SetAttribute(gST->ConOut,EFI_BACKGROUND_BLACK|EFI_LIGHTGRAY); 

  return EFI_SUCCESS;
}

//Name: ListProtocolMsg
//Input: 
//Output: 
//Descriptor:
EFI_STATUS ListProtocolMsg(IN EFI_GUID *ProtocolGuid, OUT VOID **Interface)
{
	EFI_STATUS                         Status;
	EFI_HANDLE *myHandleBuff = NULL;
	// UINTN HandleIndex = 0;
	UINTN HandleCount = 0;
  UINTN i;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *InfEntryArray;
  UINTN InfEntryCount;

  Print((const CHAR16*)L" GUID: {0x%08x, 0x%04x, 0x%04x, {",ProtocolGuid->Data1,ProtocolGuid->Data2,ProtocolGuid->Data3);
  for (i = 0; i < 8; i++)
    Print((const CHAR16 *)L" 0x%02x", ProtocolGuid->Data4[i]);
  Print((const CHAR16 *)L"}}\n");
  //1 通过Protocol GUID获取到设备句柄 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		ProtocolGuid,
		NULL,
		&HandleCount,
		&myHandleBuff  //内存由函数分配，调用者记得释放
	);
  if (EFI_ERROR(Status))
  {
    Print((const CHAR16*)L"Not Found Handle!\n");
    return Status;
  }
  else
	  Print((const CHAR16*)L"Found Handle Count: %d\n",HandleCount);
  //2 顺序找到第一个Protocol
  for (i = 0; i < HandleCount; i++)
	{
		Status = gBS->HandleProtocol(
			myHandleBuff[i],
			ProtocolGuid,
			Interface);
		if (EFI_ERROR(Status))	continue;
		else
      break;
	}
  //3 打印Protocol的信息
  Status = gBS->OpenProtocolInformation(myHandleBuff[i],ProtocolGuid,&InfEntryArray,&InfEntryCount);
  if(EFI_ERROR(Status))
    Print((const CHAR16*)L"Not Get the Protocol's information!\n");
  else
  {
    Print((const CHAR16*)L"EntryCount=%d \n",InfEntryCount);
    //4 关闭Protocol
    gBS->CloseProtocol(myHandleBuff[i],ProtocolGuid,InfEntryArray->AgentHandle,InfEntryArray->ControllerHandle);
    if(InfEntryArray) //释放被调函数申请的内存
      gBS->FreePool(InfEntryArray); 
  }
  //释放被调函数申请的内存
  if(myHandleBuff)
    gBS->FreePool(myHandleBuff);
  return Status;
}