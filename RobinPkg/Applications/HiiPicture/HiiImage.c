//code by luobing 2019-6-7 21:52:13
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "HiiImage.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutput;
extern EFI_HII_FONT_PROTOCOL *gHiiFont;
extern EFI_HII_IMAGE_PROTOCOL *gHiiImage;
extern EFI_HII_IMAGE_EX_PROTOCOL *gHiiImageEx;
extern EFI_HII_IMAGE_DECODER_PROTOCOL *gHiiImageDecoder;

//============================== Hii Function begin==============================================
//Function Name: HiiImgpackRegister
//Input: Imgx_GUID: GUID for packagelist
//       HiiImgHandle:     handle of packagelist
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS HiiImgpackRegister( IN EFI_GUID Imgx_GUID, OUT EFI_HII_HANDLE *HiiImgHandle)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *MyHandle = NULL; 
    EFI_HII_HANDLE PackageListHandle = NULL; 
    MyHandle = HiiGetHiiHandles(&Imgx_GUID);
    if(MyHandle == 0)
    {
        PackageListHandle = HiiAddPackages(&Imgx_GUID, gImageHandle, HiiPictureImages, NULL);
        if(PackageListHandle == NULL)
            return EFI_LOAD_ERROR;
        *HiiImgHandle = PackageListHandle;
    }
    else
    {
        FreePool(MyHandle);
        return EFI_LOAD_ERROR; 
    }
    return EFI_SUCCESS;
}
//Function Name: ShowHiiImage
//Input: HiiHandle:  handle of packagelist
//       IMGx:       Image ID
//       x,y:        position in display screen
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS ShowHiiImage(EFI_HII_HANDLE HiiHandle, EFI_IMAGE_ID IMGx, UINTN x, UINTN y)
{
    EFI_STATUS Status = 0;
    EFI_IMAGE_INPUT *MyImage=NULL;
    EFI_IMAGE_OUTPUT MyImageInfo[1];
    UINTN ImageBufferSize=0;
    EFI_IMAGE_OUTPUT gSystemFrameBuffer;
    EFI_IMAGE_OUTPUT *pSystemFrameBuffer = &gSystemFrameBuffer;

    gSystemFrameBuffer.Width = (UINT16)gGraphicsOutput->Mode->Info->HorizontalResolution;
    gSystemFrameBuffer.Height = (UINT16)gGraphicsOutput->Mode->Info->VerticalResolution;
    gSystemFrameBuffer.Image.Screen = gGraphicsOutput;

    //1 Get Image information by ImageID
    Status = gHiiImageEx->GetImageInfo(
        gHiiImageEx,
        HiiHandle,
        IMGx,
        MyImageInfo
    );
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    ImageBufferSize = (UINTN)(MyImageInfo[0].Height)  * (UINTN)(MyImageInfo[0].Width);
    Status = gBS->AllocatePool(EfiBootServicesData, ImageBufferSize, (void **)&MyImage);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    //2 Get Image data
    Status = gHiiImageEx->GetImageEx(
        gHiiImageEx,
        HiiHandle,
        IMGx,
        MyImage
    );
    //2 Show image
    if(MyImage)
    {
        Status = gHiiImageEx->DrawImageEx(
            gHiiImageEx,
            EFI_HII_DIRECT_TO_SCREEN,
            MyImage,
             &pSystemFrameBuffer,
             x,
             y
        );
         gBS->FreePool(MyImage);
    }
   
    return 0;
}
