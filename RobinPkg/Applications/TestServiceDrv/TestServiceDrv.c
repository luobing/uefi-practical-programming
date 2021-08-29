/** @file
  This sample application bases on HelloWorld PCD setting 
  to print "UEFI Hello World!" to the UEFI Console.

  Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleTextInEx.h>
#include "MyProtocol.h"
#define EFI_MYSAMPLE_PROTOCOL_GUID \
{ \
    0xce345181, 0xabad, 0x11e2, {0x8e, 0x5f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
}

EFI_GUID gEfiMYSampleProtocolGUID = EFI_MYSAMPLE_PROTOCOL_GUID ;

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS      Status;
	EFI_MYSAMPLE_PROTOCOL *myprotocol;
	// EFI_HANDLE *myHandleBuff = NULL;
	// INTN i;
	// EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *InfEntryArray;
	// UINTN InfEntryCount;
	// gST->ConOut->OutputString(gST->ConOut,L"========= EDKII Test Samples ========\n\r");
	// gST->ConOut->OutputString(gST->ConOut,L"Author: luobing  \n\r");
	// gST->ConOut->OutputString(gST->ConOut,L"Data: 2019-10-15 12:47:33\n\r");
	// gST->ConOut->OutputString(gST->ConOut,L"Context: Test ServiceDriverSample--\n\r");
	// gST->ConOut->OutputString(gST->ConOut,L"======================================\n\r");
	//text out test

	//1 Locate MyProtocol
	// Print(L"1. Locate Myprocotol..\n");
	Status=gBS->LocateProtocol(&gEfiMYSampleProtocolGUID,NULL, (VOID **)&myprotocol);
	if(EFI_ERROR(Status))
	{
		Print(L"LocatProtocol error: %r\n", Status);
		return Status;
	}
	//2 Test function 
	myprotocol->MySample_In(myprotocol,L"Hello,My protocol!");
	myprotocol->MySample_Out(myprotocol);
	myprotocol->MySample_DoSth(myprotocol,L"Enjoy UEFI!");
	myprotocol->MySample_Out(myprotocol);

	return EFI_SUCCESS;
}
