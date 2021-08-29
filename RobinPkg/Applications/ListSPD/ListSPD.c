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
#include "common.h"
#include "mySmbus.h"


//EFI_STATUS GetKeyEx(UINT16 *ScanCode, UINT16 *UniChar, UINT32 *ShiftState, EFI_KEY_TOGGLE_STATE * ToggleState);
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
	UINT64 flag;
	UINTN i,j;	
  UINT8 slaveAddr;

	flag=InintGloabalProtocols(SMBUSHC);
	Print(L"flag=%x\n",flag);
	// WaitKey();


	Print(L"Tool: List SPD.  Author: Robin.\n");

    Print(L"*****Slave Address: 0xA0***********\n");
    Print(L"   |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    Print(L"---|-----------------------------------------------\n");
    Print(L"00 |92 11 0B 03 04 21 02 01 03 11 01 08 0A 00 FE 00\n");
    Print(L"01 |69 78 69 30 69 11 18 81 20 08 3C 3C 00 F0 83 81\n");
    Print(L"02 |00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00\n");
    Print(L"03 |00 00 00 00 00 00 00 00 00 00 00 00 0F 11 61 00\n");
    
    

  Print(L"Tool: List SPD.  Author: Robin.\n");
  for(slaveAddr=0xA0;slaveAddr<=0xA6;slaveAddr+=2)
  {
    Print(L"*****Slave Address: 0x%02X***********\n",slaveAddr);
    Print(L"   |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    Print(L"---|-----------------------------------------------\n");
    for(i=0;i<4;i++)
    {
      Print(L"%02x |",i);
      for(j=0;j<16;j++)
        Print(L"%02x ",ReadSmbusByte(slaveAddr,(UINT8)(i*16+j)));
      Print(L"\n");
    }
  }
	return EFI_SUCCESS;
}
