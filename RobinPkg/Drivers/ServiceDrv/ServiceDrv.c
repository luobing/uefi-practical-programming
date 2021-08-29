/** @file
This is an example of how a driver retrieve HII data using HII Package List
Protocol, and how to publish the HII data.

Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
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
#include <Library/DevicePathLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include "MyProtocol.h"

// extern EFI_MYSAMPLE_PROTOCOL RobinSampleProtocol;
extern  MY_PRIVATE_DATA gMyData;

//------------------------------------  Main Entry -------------------------------------------------------------------
/**   Main entry for this driver.
  
  @param[in] ImageHandle     Image handle this driver.
  @param[in] SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
MyProtocolEntry (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  MySampleProtocolInit();

  Print(L"Install RobinSampleProtocol...\n");
  Status = gBS->InstallProtocolInterface (
            &ImageHandle,
            &gEfiMYSampleProtocolGUID ,
            EFI_NATIVE_INTERFACE,
            &gMyData.myProtocol
            );
  if(Status == EFI_SUCCESS)
    Print(L"Success!\n");
  else
    Print(L"Fail!\n");  

  return Status;
}

