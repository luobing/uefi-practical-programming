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
#include "GopRotate.h"
EFI_GUID gGraphicsOutputProtocolRotateProtocolGuid = { 0xda62085c, 0x96e4, 0x4345, { 0xa3, 0xb0, 0x98, 0x89, 0xda, 0xf3, 0x16, 0xb7 } };

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
	EFI_STATUS      Status;
	GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *GopRotate = NULL;
	ROTATE_SCREEN CurDirect;
	if ((Argc!=2) && (Argc!=1) )
	{
		Print(L"Syntax: TestGopRotate [Direct]\n");
		Print(L"Sample: TestGopRotate 2\n");
		return EFI_SUCCESS;
	}
	//1 Locate MyProtocol
	// Print(L"1. Locate Myprocotol..\n");
	Status=gBS->LocateProtocol(&gGraphicsOutputProtocolRotateProtocolGuid,NULL, (VOID **)&GopRotate);
	if(EFI_ERROR(Status))
	{
		Print(L"LocatProtocol error: %r\n", Status);
		return Status;
	}
	//2 Test function 
	if(Argc  == 1)
	{
		GopRotate->GetRotation(GopRotate,&CurDirect);
		switch(CurDirect)
		{
			case Rotate0:
				Print(L"Rotate0\n");
				break;
			case Rotate90:
				Print(L"Rotate90\n");
				break;
			case Rotate180:
				Print(L"Rotate180\n");
				break;
			case Rotate270:
				Print(L"Rotate270\n");
				break;
			default:
				Print(L"Error,Direct=%d\n",CurDirect);
			break;
		}
	}
	if(Argc == 2)
	{
		// Print(L"Argv[0] %S\n",Argv[0]);
		// Print(L"Argv[1] %S\n",Argv[1]);
		switch(Argv[1][0])
		{
			case '0':
				GopRotate->SetRotation(GopRotate, Rotate0);
				break;
			case '1':
				GopRotate->SetRotation(GopRotate, Rotate90);
				break;
			case '2':
				GopRotate->SetRotation(GopRotate, Rotate180);
				break;
			case '3':
				GopRotate->SetRotation(GopRotate, Rotate270);
				break;
			default:
				break;
		}
	}
	return EFI_SUCCESS;
}
