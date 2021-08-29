//code by luobing 2019-6-7 21:52:13
#ifndef _HIIIMAGE_H
#define _HIIIMAGE_H
#include "Common.h"


EFI_STATUS TestHiiImage(EFI_GUID Imgx_GUID, EFI_IMAGE_ID IMGx, UINTN x, UINTN y);   
EFI_STATUS HiiImgpackRegister( IN EFI_GUID Imgx_GUID, OUT EFI_HII_HANDLE *HiiImgHandle);  
EFI_STATUS ShowHiiImage(EFI_HII_HANDLE HiiHandle, EFI_IMAGE_ID IMGx, UINTN x, UINTN y);


#endif