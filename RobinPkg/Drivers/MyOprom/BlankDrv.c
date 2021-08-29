/** @file
  Blank driver - does nothing.

Copyright (c) 2006 - 2011, Intel Corporation. <BR>
All rights reserved. This program and the accompanying materials
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
#include "BlankDrv.h"
#include "Common.h"
#include "Keyboard.h"
#include "Graphic.h"
#include "Font.h"
#include "Window.h"


UINT16 MyIoBaseAddr = 0;
// UINT32 DualNetModeNum = 0;
UINT32 HorRes = SY_SCREEN_WIDTH;
UINT32 VerRes = SY_SCREEN_HEIGHT;
EFI_DRIVER_BINDING_PROTOCOL gBlankDrvDriverBinding = {
  BlankDrvDriverBindingSupported,
  BlankDrvDriverBindingStart,
  BlankDrvDriverBindingStop,
  0xa,
  NULL,
  NULL
};


/**
  The user Entry Point for module BlankDrv.

  The user code starts with this function.

  @param  ImageHandle    The firmware allocated handle for the EFI image.  
  @param  SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
    EFI_STATUS          Status;
  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gBlankDrvDriverBinding,
             ImageHandle,
             &gBlankDrvComponentName,
             &gBlankDrvComponentName2
             );
  ASSERT_EFI_ERROR (Status);
     
  return Status;
}


/**
  Test to see if this driver supports ControllerHandle.

  This service is called by the EFI boot service ConnectController(). In order
  to make drivers as small as possible, there are a few calling restrictions for
  this service. ConnectController() must follow these calling restrictions.
  If any other agent wishes to call Supported() it must also follow these
  calling restrictions.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
BlankDrvDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath   OPTIONAL
  )
{
  EFI_STATUS            Status;
  UINT16 MyVendorID = 0;
  UINT16 MyDeviceID = 0;
  EFI_PCI_IO_PROTOCOL  *PciIo;

  // open PCI protocol
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
				  				(VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  
  // read deviceID and vendorID
  Status = PciIo->Pci.Read (
  		PciIo,
  		EfiPciIoWidthUint16,
  		0,  // 
  		1,
  		&MyVendorID
  		);
	if (EFI_ERROR (Status)) {
		goto Done;
	}
  Status = PciIo->Pci.Read (
  		PciIo,
  		EfiPciIoWidthUint16,
  		2,  // 
  		1,
  		&MyDeviceID
  		);
  if (EFI_ERROR (Status)) {
		goto Done;
	}
//
// Evaluate Vendor ID and Device ID
//
	Status = EFI_SUCCESS;
	if (MyVendorID != CH366_VENDOR_ID || MyDeviceID != CH366_DEVICE_ID) {
    Status = EFI_UNSUPPORTED;
    goto Done;
	}

	Done:
//
// Close the PCI I/O Protocol
//

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}


/**
  Start this driver on ControllerHandle.

  This service is called by the EFI boot service ConnectController(). In order
  to make drivers as small as possible, there are a few calling restrictions for
  this service. ConnectController() must follow these calling restrictions. If
  any other agent wishes to call Start() it must also follow these calling
  restrictions.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
BlankDrvDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath   OPTIONAL
  )
{
  EFI_STATUS            Status;
  // BLANKDRV_DEV         *BlankDrvDevice;
  EFI_PCI_IO_PROTOCOL  *PciIo;
//  UINTN				EventIndex;
 // EFI_INPUT_KEY		Key;
  // UINT64 flag;

  UINT8 Status_Command = 0;

    //
  // Allocate Private context data for UGA Draw inteface.
  //
  // BlankDrvDevice = AllocateZeroPool (sizeof (BLANKDRV_DEV));
  // if (BlankDrvDevice == NULL) {
  //   Status = EFI_OUT_OF_RESOURCES;
  //   goto Done;
  // }

  //
  // Set up context record
  //
  // BlankDrvDevice->Signature  = BLANKDRV_DEV_SIGNATURE;
  // BlankDrvDevice->Handle     = NULL;
//
// Open the PCI I/O Protocol
//
	Status = gBS->OpenProtocol (
								Controller,
								&gEfiPciIoProtocolGuid,
								(VOID **)&PciIo,
								This->DriverBindingHandle,
								Controller,
								EFI_OPEN_PROTOCOL_BY_DRIVER
								);
if (EFI_ERROR (Status)) {
goto Done;
}
// Status = gBS->InstallMultipleProtocolInterfaces (
//                   &BlankDrvDevice->Handle,
//                   &gEfiSimpleTextInProtocolGuid,
//                   &BlankDrvDevice->SimpleTextIn,
//                   NULL
//                   );
               
 // Read Io Base address
  Status = PciIo->Pci.Read (
  		PciIo,
  		EfiPciIoWidthUint16,
  		16,  // word 0,1 = VendorID
  		1,
  		&MyIoBaseAddr
  		);
	if (EFI_ERROR (Status)) {
		goto Done;
	}
	 // read deviceID and vendorID
  Status = PciIo->Pci.Read (
  		PciIo,
  		EfiPciIoWidthUint8,
  		4,  // word 0,1 = VendorID
  		1,
  		&Status_Command
  		);			
	if (EFI_ERROR (Status)) {
		goto Done;
	}
	Status_Command|=0x07;
	  Status = PciIo->Pci.Write (
  		PciIo,
  		EfiPciIoWidthUint8,
  		4,  // word 0,1 = VendorID
  		1,
  		&Status_Command
  		);			
	if (EFI_ERROR (Status)) {
		goto Done;
	}
	MyIoBaseAddr=(MyIoBaseAddr&0x0fffe);	//clear bit0
  // Print(L"Hello,UEFI!\n");
  // WaitKey();
  HelloUEFI();
//		if (MyIoBaseAddr!=0xE000) {
//		goto Done;
//	}
  //gST->ConOut->OutputString(gST->ConOut,L"Try!\n"); 
// 	flag=InintGloabalProtocols(S_TEXT_INPUT_EX|GRAPHICS_OUTPUT|PCI_ROOTBRIDGE_IO|PCI_IO);
// //	GetPMBASE();
	
//   Status = GetGraphicsModeNumber(gGraphicsOutput, HorRes, VerRes, &DualNetModeNum, TRUE, NULL, NULL);
//     if(EFI_ERROR(Status))
//     	{
//     		Status = GetGraphicsModeNumber(gGraphicsOutput, HorRes, VerRes, &DualNetModeNum, FALSE, &HorRes, &VerRes);
//         if(EFI_ERROR(Status))
//         	DualNetModeNum = 2; //default to Mode 2
//       }

// 	main();

	
Status = EFI_SUCCESS;
Done:
  //
	// gST->ConOut->OutputString(gST->ConOut,L"please input key(ESC to exit):\n\r");
 	return Status;

}


/**
  Stop this driver on ControllerHandle.

  This service is called by the EFI boot service DisconnectController().
  In order to make drivers as small as possible, there are a few calling
  restrictions for this service. DisconnectController() must follow these
  calling restrictions. If any other agent wishes to call Stop() it must
  also follow these calling restrictions.
  
  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
BlankDrvDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer   OPTIONAL
  )
{

  //
// Close the PCI I/O Protocol
//
gBS->CloseProtocol (
  Controller,
  &gEfiPciIoProtocolGuid,
  This->DriverBindingHandle,
  Controller
);
return EFI_SUCCESS;
	//return EFI_UNSUPPORTED;
}


/**
  This is the default unload handle for all the network drivers.

  Disconnect the driver specified by ImageHandle from all the devices in the handle database.
  Uninstall all the protocols installed in the driver entry point.
  
  @param[in]  ImageHandle       The drivers' driver image.

  @retval EFI_SUCCESS           The image is unloaded.
  @retval Others                Failed to unload the image.

**/
EFI_STATUS
EFIAPI
DefaultUnload (
  IN EFI_HANDLE             ImageHandle
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *DeviceHandleBuffer;
  UINTN                             DeviceHandleCount;
  UINTN                             Index;
  EFI_DRIVER_BINDING_PROTOCOL       *DriverBinding;
  EFI_COMPONENT_NAME_PROTOCOL       *ComponentName;
  EFI_COMPONENT_NAME2_PROTOCOL      *ComponentName2;

  //
  // Get the list of all the handles in the handle database.
  // If there is an error getting the list, then the unload
  // operation fails.
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &DeviceHandleCount,
                  &DeviceHandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Disconnect the driver specified by ImageHandle from all
  // the devices in the handle database.
  //
  for (Index = 0; Index < DeviceHandleCount; Index++) {
    Status = gBS->DisconnectController (
                    DeviceHandleBuffer[Index],
                    ImageHandle,
                    NULL
                    );
  }

  //
  // Uninstall all the protocols installed in the driver entry point
  //
  for (Index = 0; Index < DeviceHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    DeviceHandleBuffer[Index],
                    &gEfiDriverBindingProtocolGuid,
                    (VOID **) &DriverBinding
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    if (DriverBinding->ImageHandle != ImageHandle) {
      continue;
    }

    gBS->UninstallProtocolInterface (
          ImageHandle,
          &gEfiDriverBindingProtocolGuid,
          DriverBinding
          );
    Status = gBS->HandleProtocol (
                    DeviceHandleBuffer[Index],
                    &gEfiComponentNameProtocolGuid,
                    (VOID **) &ComponentName
                    );
    if (!EFI_ERROR (Status)) {
      gBS->UninstallProtocolInterface (
             ImageHandle,
             &gEfiComponentNameProtocolGuid,
             ComponentName
             );
    }

    Status = gBS->HandleProtocol (
                    DeviceHandleBuffer[Index],
                    &gEfiComponentName2ProtocolGuid,
                    (VOID **) &ComponentName2
                    );
    if (!EFI_ERROR (Status)) {
      gBS->UninstallProtocolInterface (
             ImageHandle,
             &gEfiComponentName2ProtocolGuid,
             ComponentName2
             );
    }
  }

  //
  // Free the buffer containing the list of handles from the handle database
  //
  if (DeviceHandleBuffer != NULL) {
    gBS->FreePool (DeviceHandleBuffer);
  }

  return EFI_SUCCESS;
}

//==========luobing add for oprom==========================
VOID HelloUEFI(VOID)
{
  UINT64 flag;
  EFI_INPUT_KEY key={0,0};  
  // UINT8 regValue=0;
  // INTN i;
  UINT8 *s_text = "Alasse' aure,";  //《魔戒》精灵语的 “你好，日安”
  UINT8 *s_text1 = "欢迎进入UEFI的世界！";
  UINT8 *s_text2 = "按'ESC'键退出此界面";
  flag = InintGloabalProtocols(GRAPHICS_OUTPUT);
	Print(L"flag=%x\n",flag);
	// WaitKey();
  //图形显示测试
	SwitchGraphicsMode(TRUE);
	SetBKG(&(gColorTable[DEEPBLUE]));
  
  draw_string(s_text, 110, 60, &MyFontArray, &(gColorTable[WHITE]));
  draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[WHITE]));
  draw_string(s_text2, 135, 140, &MyFontArray1, &(gColorTable[YELLOW]));
  //点亮灯
  // regValue = IoRead8(MyIoBaseAddr+1);   //CTRL	
  // for(i=0;i<6;i++)
  // {
  //   regValue = IoRead8(MyIoBaseAddr+1);   //CTRL
  //   regValue |= 0x01; 	
  //   IoWrite8(MyIoBaseAddr+1,regValue);
  //   gBS->Stall(200*1000);     //200ms
  //   regValue &= 0xFE; 
  //   IoWrite8(MyIoBaseAddr+1,regValue);
  //   gBS->Stall(200*1000);     //200ms
  // }
  while(key.ScanCode!=0x17)	//ESC
  {
    GetKey(&key);
    if(key.ScanCode ==1 ) //UP 
      draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[DEEPBLUE]));
    else if(key.ScanCode ==2 ) //DOWN
      draw_string(s_text1, 80, 100, &MyFontArray, &(gColorTable[WHITE]));  
    if(key.UnicodeChar == 0x31)   
      SetLed(MyIoBaseAddr,LED1,LEDON);
    if(key.UnicodeChar == 0x32)   
      SetLed(MyIoBaseAddr,LED2,LEDON);
    if(key.UnicodeChar == 0x33)   
      SetLed(MyIoBaseAddr,LED1,LEDOFF);
    if(key.UnicodeChar == 0x34)   
      SetLed(MyIoBaseAddr,LED2,LEDOFF);

  }
  SetMyMode(OldGraphicsMode);
  SwitchGraphicsMode(FALSE);
}

//
VOID SetLed(UINT16 IoBaseAddress,UINT8 LedNum,UINT8 value)
{
  UINT8 regValue=0;
  if(LedNum==LED1)
  {
    regValue = IoRead8(MyIoBaseAddr+1);   //CTRL 
    if(value)
      regValue |= 0x01;
    else
      regValue &= 0xFE;
    IoWrite8(MyIoBaseAddr+1,regValue);      
  }
  if(LedNum==LED2)
  {
    regValue = IoRead8(MyIoBaseAddr+1);   //CTRL
    if(value)
      regValue |= 0x02;
    else
      regValue &= 0xFD;
    IoWrite8(MyIoBaseAddr+1,regValue);      
  }
}