/* GopRotate.c
Copyright (c) 2015, Aaron Pop
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "GopRotate.h"
extern EFI_COMPONENT_NAME2_PROTOCOL gGraphicsOutputRotateComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gGraphicsOutputRotateComponentName;

/// Global variable used to keep track of Gop Devices being managed by 
LIST_ENTRY GopDeviceList;

EFI_GUID gGraphicsOutputProtocolRotateProtocolGuid = GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  );

/// Driver Binding instance
EFI_DRIVER_BINDING_PROTOCOL gGraphicsOutputRotateDriverBinding = {
  GraphicsOutputRotateDriverSupported,
  GraphicsOutputRotateDriverStart,
  GraphicsOutputRotateDriverStop,
  0x0,
  NULL,
  NULL
};

/**
 * Driver Binding Supported function to determine if the controller can be managed
 * by the rotate function
 *
 * Graphics Console could be supported if there is a Graphics Output Protocol 
 * instance installed on the controller handle, if there is a device path protocol
 * instance installed on the handle, and if there is not already a graphics output
 * rotate protocol instance already installed.
 *
 * @param  This                Protocol instance pointer.
 * @param  Controller          Handle of device to test.
 * @param  RemainingDevicePath Optional parameter use to pick a specific child
 *                             device to start.
 *
 * @retval EFI_SUCCESS         This driver supports this device.
 * @retval other               This driver does not support this device.
**/
EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *RotateProtocol;

    // Verify that this controller has a GraphicsOutputProtocol installed
    Status = gBS->OpenProtocol( Controller, 
                                &gEfiGraphicsOutputProtocolGuid, 
                                (VOID**)&Gop, 
                                This->DriverBindingHandle, 
                                Controller, 
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;
    else
        gBS->CloseProtocol( Controller,
                            &gEfiGraphicsOutputProtocolGuid,
                            This->DriverBindingHandle,
                            Controller);

    // Verify that this controller has a Device Path (is not virtual)
    Status = gBS->OpenProtocol(Controller, 
                                &gEfiDevicePathProtocolGuid, 
                                (VOID**)&Dp, 
                                This->DriverBindingHandle, 
                                Controller, 
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;
    else
        gBS->CloseProtocol( Controller,
                            &gEfiGraphicsOutputProtocolGuid,
                            This->DriverBindingHandle,
                            Controller);

    // Verify that this controller does not already have a rotate protocol
    Status = gBS->OpenProtocol(Controller, 
                                &gGraphicsOutputProtocolRotateProtocolGuid, 
                                (VOID**)&RotateProtocol, 
                                This->DriverBindingHandle, 
                                Controller, 
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(!EFI_ERROR(Status)) 
    {
        gBS->CloseProtocol( Controller,
                            &gGraphicsOutputProtocolRotateProtocolGuid,
                            This->DriverBindingHandle,
                            Controller);
        return EFI_ALREADY_STARTED;
    }

    return EFI_SUCCESS;
}

/**
 * Driver Binding Start function to start managing a controller by installing
 * a graphics output rotate protocol onto the controller handle
 *
 * When a controller is found that can be managed by the rotate function
 *
 * @param  This                Protocol instance pointer.
 * @param  Controller          Handle of device to start managing.
 * @param  RemainingDevicePath Optional parameter use to pick a specific child
 *                             device to start.
 *
 * @retval EFI_SUCCESS         This driver supports this device.
 * @retval other               This driver does not support this device.
**/
EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
    EFI_STATUS Status;
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    GOP_DEVICE_LIST *NewEntry = NULL;

    // Verify that this controller has a GraphicsOutputProtocol installed
    Status = gBS->OpenProtocol( Controller, 
                                &gEfiGraphicsOutputProtocolGuid, 
                                (VOID**)&Gop, 
                                This->DriverBindingHandle, 
                                Controller, 
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;

    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(GRAPHICS_OUTPUT_ROTATE_PRIVATE), (VOID**)&Private);
    if(!EFI_ERROR(Status))
    {
        Private->Signature = GRAPHICS_OUTPUT_ROTATE_DEV_SIGNATURE;
        Private->Handle = Controller;
        Private->Gop = Gop;
        Private->Blt = Gop->Blt;
        Private->GopRotate.GetRotation = GopRotateGetRotation;
        Private->GopRotate.SetRotation = GopRotateSetRotation;
        Private->Gop->Blt = BltRotate;
        // Private->Rotation = PcdGet8(DefaultGopRotation);
        Private->Rotation = Rotate0;
        Status = gBS->InstallMultipleProtocolInterfaces(&   Controller,
                                                        &gGraphicsOutputProtocolRotateProtocolGuid, &(Private->GopRotate),
                                                        NULL);
        Status = gBS->AllocatePool(EfiBootServicesData, sizeof(GOP_DEVICE_LIST), (VOID**)&NewEntry);
        if(!EFI_ERROR(Status))
        {
            NewEntry->Private = Private;
            InsertTailList((LIST_ENTRY*)&GopDeviceList, (LIST_ENTRY*)&(NewEntry->Link));
        }
    }
    return Status;
}


/**
 * Driver Binding Stop function to stop managing a controller
 *
 * When a device no longer should be managed by this driver, the stop function will be 
 * called to clean up any changes to the controller that were done while taking control
 * of the device
 *
 * @param  This                Protocol instance pointer.
 * @param  Controller          Handle of device to start managing.
 * @param  RemainingDevicePath Optional parameter use to pick a specific child
 *                             device to start.
 *
 * @retval EFI_SUCCESS         This driver supports this device.
 * @retval other               This driver does not support this device.
**/
GRAPHICS_OUTPUT_ROTATE_PRIVATE *GetPrivateFromGop(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop);

EFI_STATUS
EFIAPI
GraphicsOutputRotateDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
    EFI_STATUS Status;
    GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *RotateProtocol;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private;
    GOP_DEVICE_LIST *Entry = NULL;

    Status = gBS->OpenProtocol( Controller,
                                &gGraphicsOutputProtocolRotateProtocolGuid,
                                (VOID **)&RotateProtocol,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(EFI_ERROR (Status))
        return EFI_NOT_STARTED;

    Private = GetPrivateFromGopRotate(RotateProtocol);
    if(Private != NULL)
    {
        Status = gBS->UninstallProtocolInterface(   Controller,
                                                    &gGraphicsOutputProtocolRotateProtocolGuid,
                                                    &Private->GopRotate);
        Status = gBS->OpenProtocol( Controller, 
                                    &gEfiGraphicsOutputProtocolGuid, 
                                    (VOID**)&Gop, 
                                    This->DriverBindingHandle, 
                                    Controller, 
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if(!EFI_ERROR (Status))
            Gop->Blt = Private->Blt;

        for(Entry = (GOP_DEVICE_LIST*)GetFirstNode(&GopDeviceList); 
            !IsNull(&GopDeviceList, &(Entry->Link)); 
            Entry = (GOP_DEVICE_LIST*)GetNextNode(&GopDeviceList, &(Entry->Link))) 
        {
            if(Entry->Private == Private)
                RemoveEntryList((LIST_ENTRY*)Entry);
        }
        gBS->SetMem(Private, sizeof(GRAPHICS_OUTPUT_ROTATE_PRIVATE), 0);
        gBS->FreePool(Private);
    
    }
    return EFI_SUCCESS;
}

/**
 *      Module entry point. Installs a driver binding protocol instance for managing devices that
 *      can be supported to rotate the GOP screen. 
 *
 *      @param  ImageHandle Pointer to the Image handle of this driver
 *      @param  SystemTable Pointer to the EFI_SYSTEM_TABLE 
 **/
EFI_STATUS EFIAPI GopRotateEntryPoint(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    InitializeListHead(&GopDeviceList);

    gGraphicsOutputRotateDriverBinding.ImageHandle = ImageHandle;
    gGraphicsOutputRotateDriverBinding.DriverBindingHandle = ImageHandle;

    Status = gBS->InstallMultipleProtocolInterfaces(&gGraphicsOutputRotateDriverBinding.DriverBindingHandle,
                                                    &gEfiDriverBindingProtocolGuid, &gGraphicsOutputRotateDriverBinding,
                                                    &gEfiComponentNameProtocolGuid, &gGraphicsOutputRotateComponentName,
                                                    &gEfiComponentName2ProtocolGuid, &gGraphicsOutputRotateComponentName2,
                                                    NULL);

    return EFI_SUCCESS;
}