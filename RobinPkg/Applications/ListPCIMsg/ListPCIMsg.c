/** @file
    A simple, basic, application showing how the Hello application could be
    built using the "Standard C Libraries" from StdLib.

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
#include "PciRW.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_PCI_IO_PROTOCOL *gPCIIOArray[256];
extern UINTN gPCIIO_Count; 
//debug function
EFI_STATUS ListPCIMessage1(void);
EFI_STATUS ListPCIMessage2(void);

/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int
main (
  IN int Argc,
  IN char **Argv
  )
{
	UINT64 flag;

	if(Argc == 2)
	{
		if((Argv[1][0] == '-') || (Argv[1][0] == '/'))
		{
			switch(Argv[1][1])
			{
				case '1':
					flag=InintGloabalProtocols(PCI_ROOTBRIDGE_IO);
					Print(L"flag=%x\n",flag);
					if((flag & PCI_ROOTBRIDGE_IO)== 0) //�ҵ���
						ListPCIMessage1();
					break;

				case '2':
					flag=InintGloabalProtocols(PCI_IO);
					Print(L"flag=%x\n",flag);
					if((flag & PCI_IO)== 0) //�ҵ���
						ListPCIMessage2();
					break;
				default:
					Print(L"Syntax is: listpci -1(or -2)\n");
					break;
			}
		}
		else
			Print(L"Syntax is: listpci -1(or -2)\n");
	}
	else
		Print(L"Syntax is: listpci -1(or -2)\n");

	return 0;
}



//=========Debug function=============================================================
//Name: ListPCIMessage1(void)
//Input:
//Output:
EFI_STATUS ListPCIMessage1(void)
{
	EFI_STATUS	Status=EFI_SUCCESS;
	PCI_TYPE00 Pci;
	UINT16 i,j,k,count=0;
	
	for(k=0;k<=PCI_MAX_BUS;k++)
		for(i=0;i<=PCI_MAX_DEVICE;i++)
			for(j=0;j<=PCI_MAX_FUNC;j++)
			{
				//�ж��豸�Ƿ����
				Status = PciDevicePresent(gPCIRootBridgeIO,&Pci,(UINT8)k,(UINT8)i,(UINT8)j);
				if (Status == EFI_SUCCESS)		//�ҵ��豸��
				{
					++count;		
					Print(L"%02d. Bus-%02x Dev-%02x Func-%02x: ",count,(UINT8)k,(UINT8)i,(UINT8)j);
					Print(L"VendorID-%x DeviceID-%x ClassCode-%x",Pci.Hdr.VendorId,Pci.Hdr.DeviceId,Pci.Hdr.ClassCode[0]);
					Print(L"\n");
				}
			}
	return EFI_SUCCESS;
}
//Name: ListPCIMessage2(void)
//Input:
//Output:
EFI_STATUS ListPCIMessage2(void)
{
	UINTN i,count=0;
	PCI_TYPE00 Pci;
	for(i=0;i<gPCIIO_Count;i++)
	{
		gPCIIOArray[i]->Pci.Read(gPCIIOArray[i],EfiPciWidthUint32,0,sizeof (PCI_TYPE00) / sizeof (UINT32),&Pci);
				
		++count;
		Print(L"%02d. VendorID-%x DeviceID-%x ClassCode-%x",count,Pci.Hdr.VendorId,Pci.Hdr.DeviceId,Pci.Hdr.ClassCode[0]);
		Print(L"\n");
		
	}
	return EFI_SUCCESS;
}