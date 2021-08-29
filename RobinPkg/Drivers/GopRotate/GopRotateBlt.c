/* GopRotateBlt.c
Copyright (c) 2016, Aaron Pop
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

extern LIST_ENTRY GopDeviceList;

EFI_STATUS EFIAPI BltRotate 
(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer, OPTIONAL
    IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
    IN UINTN SourceX,
    IN UINTN SourceY,
    IN UINTN DestinationX,
    IN UINTN DestinationY,
    IN UINTN Width,
    IN UINTN Height,
    IN UINTN Delta OPTIONAL
);


/**
 *      Return a pointer to the Private data structure associated with a passed GOP instance
 *
 *      @param  Gop         Pointer to the graphics output protocol instance to match to a Private
 *                          data structure
 **/
GRAPHICS_OUTPUT_ROTATE_PRIVATE *GetPrivateFromGop(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop)
{
    GOP_DEVICE_LIST *Entry = NULL;
    if(Gop != NULL)
    {
        for(Entry = (GOP_DEVICE_LIST*)GetFirstNode(&GopDeviceList); 
            !IsNull(&GopDeviceList, &(Entry->Link)); 
            Entry = (GOP_DEVICE_LIST*)GetNextNode(&GopDeviceList, &(Entry->Link))) 
        {
            if(Entry->Private->Gop == Gop)
                return Entry->Private;
        }
    }
    return NULL;
}

/**
 *      Return a pointer to the Prviate data structure associated with a passed Graphics Output Protocol Rotate instance
 *
 *      @param  GopRotate   Pointer to the graphics output rotate instance to match to a private data structure
 **/
GRAPHICS_OUTPUT_ROTATE_PRIVATE *GetPrivateFromGopRotate(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *GopRotate)
{
    GOP_DEVICE_LIST *Entry = NULL;
    if(GopRotate != NULL)
    {
        for(Entry = (GOP_DEVICE_LIST*)GetFirstNode(&GopDeviceList); 
            !IsNull(&GopDeviceList, &(Entry->Link)); 
            Entry = (GOP_DEVICE_LIST*)GetNextNode(&GopDeviceList, &(Entry->Link))) 
        {
            if(&(Entry->Private->GopRotate) == GopRotate)
                return Entry->Private;
        }
    }
    return NULL;
}

/**
 *      Protocol member function of the Graphics Output Protocol Rotate Protocol. Returns the current screen
 *      rotation for this device
 *
 *      @param  This        Pointer to the Graphics Output Protocol Rotate Protocol instance assocaited with this device
 *      @param  Rotation    Pointer to a buffer to fill with the current 
 **/
EFI_STATUS EFIAPI GopRotateGetRotation(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *This, ROTATE_SCREEN *Rotation)
{
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private;
    Private = GetPrivateFromGopRotate(This);
    if(Private != NULL)
    {
        *Rotation = Private->Rotation;
    }
    return EFI_SUCCESS;
}

/**
 *      Protocol member function of the Graphics Output Protocol Rotate Protocol. Sets the expected screen rotation
 *      for the passed Graphics Output Protocol Rotate Protocol instance.
 *
 *      @param  This        Pointer to the Graphics Output Protocol Rotate Protocol instance whose rotation should be modified
 *      @param  Rotation    Rotation that should be used for this device
 *
 **/
EFI_STATUS EFIAPI GopRotateSetRotation(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *This, ROTATE_SCREEN Rotation)
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt = NULL;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL BlackPixel = {0, 0, 0, 0};
    UINTN Size = 0;

    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private = GetPrivateFromGopRotate(This);
    if(Private != NULL)
    {
        Size = (UINT32)MultU64x32(MultU64x32(Private->Gop->Mode->Info->HorizontalResolution, Private->Gop->Mode->Info->VerticalResolution), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        Status = gBS->AllocatePool(EfiBootServicesData, Size, &Blt);
        if(!EFI_ERROR(Status))
        {
            gBS->SetMem(Blt, Size, 0);
            Status = Private->Gop->Blt(Private->Gop, Blt, EfiBltVideoToBltBuffer, 0, 0, 0, 0, Private->Gop->Mode->Info->HorizontalResolution, Private->Gop->Mode->Info->VerticalResolution, 0);
        }
        Private->Rotation = Rotation;

        // Call the original Blt Function to fill the entire buffer
        Private->Blt(Private->Gop, &BlackPixel, EfiBltVideoFill, 0, 0, 0, 0, Private->Gop->Mode->Info->HorizontalResolution, Private->Gop->Mode->Info->VerticalResolution, 0);
        if(!EFI_ERROR(Status))
        {
            Status = Private->Gop->Blt(Private->Gop, Blt, EfiBltBufferToVideo, 0, 0, 0, 0, Private->Gop->Mode->Info->HorizontalResolution, Private->Gop->Mode->Info->VerticalResolution, 0);
        }
    }
    return EFI_SUCCESS;
}

/**
 *      Helper function that is called to translate to the selected rotation of the device
 *
 *      @param  This                   Protocol instance pointer.
 *      @param  BltBuffer              Buffer containing data to blt into video buffer.
 *      @param  BltOperation           Operation to perform on BltBuffer and/or video memory  
 *      @param  SourceX                Horizontal offset into the source (Buffer or Video) where
 *                                      the selected operation should begin
 *      @param  SourceX                Vertical offset into the source (Buffer or Video) where
 *                                      the selected operation should begin
 *      @param  DestinationX           Horizontal offset into the destination (Buffer or Video) where
 *                                      the selected operation should begin
 *      @param  DestinationY           Vertical offset into the destination (Buffer or Video) where
 *                                      the selected operation should begin
 *      @param  Width                  Width of the data for the selected operation. Not to be confused
 *                                      with the Width of the BltBuffer, which may or maynot match the
 *                                      operation width
 *      @param  Height                 Height of the data for the selected operation. Not to be confused
 *                                      with the Height of the BltBuffer, which may or maynot match the
 *                                      operation Height
 *      @param  Delta                  The size, in bytes, that corresponds to each row of the BltBuffer. 
 *      @param  Private                Pointer to the structure used to track the current settings for this
 *                                      instance of the Gop Rotate device
 *
 *
 *      @retval EFI_SUCCESS            The palette is updated with PaletteArray.
 *      @retval EFI_INVALID_PARAMETER  BltOperation is not valid.
 *      @retval EFI_DEVICE_ERROR       A hardware error occurred writing to the video buffer
 **/
EFI_STATUS PerformTranslations
(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL **BltBuffer,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
    UINTN *SourceX,
    UINTN *SourceY,
    UINTN *DestinationX,
    UINTN *DestinationY,
    UINTN *Width,
    UINTN *Height,
    UINTN *Delta,
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private
)
{
    EFI_STATUS Status;

    UINTN ClippedX = 0;
    UINTN ClippedY = 0;
    
    switch(Private->Rotation)
    {
        case Rotate0:
        {
            Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
            break;
        }
        case Rotate90:
        {
            UINTN OriginalWidth = *Width;
            UINTN OriginalHeight = *Height;

            UINTN OriginalDestinationX = *DestinationX;
            UINTN OriginalDestinationY = *DestinationY;

            UINTN OriginalSourceX = *SourceX;
            UINTN OriginalSourceY = *SourceY;

            UINTN OriginalDelta = *Delta;

            // Specification says delta is optional.  If a zero value delta is passed, update delta
            //  to be the Width
            if(*Delta == 0)
            {
                *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                OriginalDelta = *Delta;
            }

            if(This->Mode->Info->VerticalResolution < (*DestinationX + *Width))
                ClippedY = *DestinationX + *Width - This->Mode->Info->VerticalResolution;

            if(This->Mode->Info->HorizontalResolution < (*DestinationY + *Height))
                ClippedX = *DestinationY + *Height - This->Mode->Info->HorizontalResolution;

            switch(BltOperation)
            {
                case EfiBltBufferToVideo:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;

                    UINTN Size = 0;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;

                    Size = (UINTN)MultU64x32(MultU64x32(OriginalWidth, (UINT32)OriginalHeight), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        UINTN j; 
                        UINTN i;
                        for(j = 0; j < OriginalHeight; j++)
                        {
                            for(i = 0; i < OriginalWidth; i++)
                            {
                                Index = MultU64x32(i, (UINT32)(*Height)) + j;

                                TranslatedIndex = MultU64x32( DivU64x32(OriginalDelta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)(OriginalSourceY+j)) + (OriginalSourceX + OriginalWidth - i - 1);

                                gBS->CopyMem(&(MyBuffer[(UINTN)Index]), &((*BltBuffer)[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }
                    }

                    *DestinationX = OriginalDestinationY;
                    if(OriginalDestinationX < This->Mode->Info->VerticalResolution)
                        *DestinationY = This->Mode->Info->VerticalResolution - OriginalWidth - OriginalDestinationX + ClippedY;
                    else
                        *DestinationY = 0;

                    *SourceX = 0;
                    if( (OriginalDestinationX + OriginalWidth) > This->Mode->Info->VerticalResolution)
                        *SourceY = OriginalDestinationX + OriginalWidth - This->Mode->Info->VerticalResolution;
                    else
                        *SourceY = 0;

                    *Width = OriginalHeight;
                    if( (OriginalDestinationX + OriginalWidth) >= This->Mode->Info->VerticalResolution)
                        *Height = This->Mode->Info->VerticalResolution - OriginalDestinationX;
                    else
                        *Height = OriginalWidth - ClippedY;

                    *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                    Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);                    

                    gBS->FreePool(MyBuffer);
                    break;
                }
                case EfiBltVideoFill:
                {
                    *DestinationX = OriginalDestinationY;
                    *DestinationY = This->Mode->Info->VerticalResolution - OriginalWidth - OriginalDestinationX + ClippedY;

                    *Width = OriginalHeight;
                    *Height = OriginalWidth - ClippedY;

                    Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    break;
                }
                case EfiBltVideoToVideo:
                {
                    *DestinationX = OriginalDestinationY;
                    *DestinationY = This->Mode->Info->VerticalResolution - OriginalWidth - OriginalDestinationX + ClippedY;

                    *SourceX = OriginalSourceY;
                    *SourceY = This->Mode->Info->VerticalResolution - OriginalWidth - OriginalSourceX + ClippedY;

                    *Width = OriginalHeight;
                    *Height = OriginalWidth - ClippedY;

                    Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    break;
                }
                case EfiBltVideoToBltBuffer:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;
                    UINTN i;
                    UINTN j;
                    UINTN Size = 0;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;

                    Size = (UINTN)MultU64x32( MultU64x32(OriginalWidth, (UINT32)OriginalHeight), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        *SourceX = OriginalSourceY;
                        if( (OriginalSourceX + OriginalWidth) > This->Mode->Info->VerticalResolution)
                            break;
                        else
                            *SourceY = This->Mode->Info->VerticalResolution - OriginalWidth - OriginalSourceX + ClippedY;

                        *Width = OriginalHeight;
                        *Height = OriginalWidth - ClippedY;

                        *Delta = (UINTN)MultU64x32(OriginalHeight, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                        Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, 0, 0, *Width, *Height, *Delta);

                        for(j = 0; j < (OriginalHeight-ClippedY); j++)
                        {
                            for(i = 0; i < (OriginalWidth - ClippedX); i++)
                            {
                                Index = MultU64x32(DivU64x32(OriginalDelta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)(OriginalDestinationY + j)) + OriginalDestinationX + i;
                                TranslatedIndex = MultU64x32(*Width, (UINT32)(*Height - i - 1)) + j;
                                gBS->CopyMem(&((*BltBuffer)[(UINTN)Index]), &(MyBuffer[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        
        case Rotate180:
        {
            if(*Delta == 0)
                *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

            switch(BltOperation)
            {
                case EfiBltBufferToVideo:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;
                    UINTN i;
                    UINTN j = 0;
                    UINTN Size = 0;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;
                    Size = (UINTN)MultU64x32(MultU64x32(*Height, (UINT32)(*Width)), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        for(j = *SourceY; j < *Height; j++)
                        {
                            for(i = *SourceX; i < *Width; i++)
                            {
                                Index = MultU64x32(*Width, (UINT32)(*Height - j - 1)) + (*Width - i - 1);
                                TranslatedIndex = (UINTN)MultU64x32( DivU64x32(*Delta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)j) + i;
                                gBS->CopyMem( &(MyBuffer[(UINTN)Index]), &((*BltBuffer)[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }
                        *BltBuffer = MyBuffer;

                        *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                        *SourceX = 0;
                        *SourceY = 0;

                        *DestinationX = This->Mode->Info->HorizontalResolution - *Width - *DestinationX;
                        *DestinationY = This->Mode->Info->VerticalResolution - *Height - *DestinationY;

                        Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                        gBS->FreePool(MyBuffer);
                    }
                    break;
                }
                case EfiBltVideoFill:
                case EfiBltVideoToVideo:
                {
                    *SourceX = This->Mode->Info->HorizontalResolution - *Width - *SourceX;
                    *SourceY = This->Mode->Info->VerticalResolution - *Height - *SourceY;

                    *DestinationX = This->Mode->Info->HorizontalResolution - *Width - *DestinationX;
                    *DestinationY = This->Mode->Info->VerticalResolution - *Height - *DestinationY;
                    Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    break;
                }
                case EfiBltVideoToBltBuffer:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;
                    UINTN i;
                    UINTN j;
                    UINTN Size = 0;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;

                    Size = (UINTN)MultU64x32( MultU64x32(*Height, (UINT32)(*Width)), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        *SourceX = This->Mode->Info->HorizontalResolution - *Width - *SourceX;
                        *SourceY = This->Mode->Info->VerticalResolution - *Height - *SourceY;

                        Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, 0, 0, *Width, *Height, (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
                        *SourceX = This->Mode->Info->HorizontalResolution - *Width - *SourceX;
                        *SourceY = This->Mode->Info->VerticalResolution - *Height - *SourceY;

                        for(j = 0; j < *Height; j++)
                        {
                            for(i = 0; i < *Width; i++)
                            {
                                Index = (UINTN)MultU64x32(DivU64x32(*Delta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)(*DestinationY + j)) + *DestinationX + i;
                                TranslatedIndex = MultU64x32(*Width, (UINT32)(*Height - j - 1)) + *Width - i - 1;
                                gBS->CopyMem( &((*BltBuffer)[(UINTN)Index]), &(MyBuffer[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }

                        gBS->FreePool(MyBuffer);

                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case Rotate270:
        {
            UINTN OriginalWidth = *Width;
            UINTN OriginalHeight = *Height;

            UINTN OriginalDestinationX = *DestinationX;
            UINTN OriginalDestinationY = *DestinationY;

            UINTN OriginalSourceX = *SourceX;
            UINTN OriginalSourceY = *SourceY;

            UINTN OriginalDelta = *Delta;
            if(*Delta == 0)
            {
                *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                OriginalDelta = *Delta;
            }
            if(This->Mode->Info->VerticalResolution < (*DestinationX + *Width))
                ClippedY = *DestinationX + *Width - This->Mode->Info->VerticalResolution;

            if(This->Mode->Info->HorizontalResolution < (*DestinationY + *Height))
                ClippedX = *DestinationY + *Height - This->Mode->Info->HorizontalResolution;

            switch(BltOperation)
            {
                case EfiBltBufferToVideo:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;

                    UINTN Size;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;

                    Size = (UINTN)MultU64x32(MultU64x32(OriginalWidth, (UINT32)OriginalHeight), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        UINTN j; 
                        UINTN i;
                        for(j = 0; j < OriginalHeight; j++)
                        {
                            for(i = 0; i < OriginalWidth; i++)
                            {
                                TranslatedIndex = (UINTN)MultU64x32( DivU64x32(OriginalDelta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)(OriginalSourceY + OriginalHeight - j - 1)) + OriginalSourceX + i;
                                Index = (UINTN)MultU64x32(*Height, (UINT32)i) + j;
                                gBS->CopyMem( &(MyBuffer[(UINTN)Index]), &((*BltBuffer)[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }
                    }

                    *DestinationX = This->Mode->Info->HorizontalResolution - OriginalHeight - OriginalDestinationY + ClippedX;
                    if( OriginalDestinationX < This->Mode->Info->VerticalResolution )
                        *DestinationY = OriginalDestinationX;
                    else
                        break;

                    *SourceX = 0;
                    *SourceY = 0;

                    *Width = OriginalHeight - ClippedX;
                    if( (OriginalDestinationX + OriginalWidth) > This->Mode->Info->VerticalResolution)
                        *Height = This->Mode->Info->VerticalResolution - OriginalDestinationX ;
                    else
                        *Height = OriginalWidth;

                    *Delta = (UINTN)MultU64x32(*Width, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                    Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    gBS->FreePool(MyBuffer);
                    break;
                }
                case EfiBltVideoFill:
                {
                    *DestinationX = This->Mode->Info->HorizontalResolution - OriginalHeight - OriginalDestinationY + ClippedX;
                    if( OriginalDestinationX < This->Mode->Info->VerticalResolution )
                        *DestinationY = OriginalDestinationX;
                    else
                        break;

                    *Width = OriginalHeight - ClippedX;
                    if( (OriginalDestinationX + OriginalWidth) > This->Mode->Info->VerticalResolution)
                        *Height = This->Mode->Info->VerticalResolution - OriginalDestinationX ;
                    else
                        *Height = OriginalWidth;

                    Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    break;
                }
                case EfiBltVideoToVideo:
                {
                    *DestinationX = This->Mode->Info->HorizontalResolution - OriginalHeight - OriginalDestinationY + ClippedX;
                    if( OriginalDestinationX < This->Mode->Info->VerticalResolution )
                        *DestinationY = OriginalDestinationX;
                    else
                        break;

                    *SourceX = This->Mode->Info->HorizontalResolution - OriginalHeight - OriginalSourceY + ClippedX;
                    if( OriginalSourceX < This->Mode->Info->VerticalResolution )
                        *SourceY = OriginalSourceX;
                    else
                        break;

                    *Width = OriginalHeight - ClippedX;
                    if( (OriginalDestinationX + OriginalWidth) > This->Mode->Info->VerticalResolution)
                        *Height = This->Mode->Info->VerticalResolution - OriginalDestinationX ;
                    else
                        *Height = OriginalWidth;

                    Status = Private->Blt(This, *BltBuffer, BltOperation, *SourceX, *SourceY, *DestinationX, *DestinationY, *Width, *Height, *Delta);
                    break;
                }
                case EfiBltVideoToBltBuffer:
                {
                    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MyBuffer = NULL;
                    UINTN i;
                    UINTN j;

                    UINTN Size;
                    UINT64 TranslatedIndex = 0;
                    UINT64 Index = 0;

                    Size = (UINTN)MultU64x32( MultU64x32(OriginalWidth, (UINT32)OriginalHeight), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&MyBuffer);
                    if(!EFI_ERROR(Status))
                    {
                        *SourceX = This->Mode->Info->HorizontalResolution - OriginalHeight - OriginalSourceY + ClippedX;
                        *SourceY = OriginalSourceX;

                        *Width = OriginalHeight - ClippedX;
                        *Height = OriginalWidth;

                        *Delta = (UINTN)MultU64x32(OriginalHeight, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

                        Status = Private->Blt(This, MyBuffer, BltOperation, *SourceX, *SourceY, 0, 0, *Width, *Height, *Delta);

                        for(j = 0; j < (OriginalHeight-ClippedY); j++)
                        {
                            for(i = 0; i < (OriginalWidth - ClippedX); i++)
                            {
                                TranslatedIndex = MultU64x32(*Width, (UINT32)i) + *Width - j - 1;

                                Index = (UINTN)MultU64x32( DivU64x32(OriginalDelta, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)), (UINT32)(OriginalDestinationY + j)) + i + OriginalDestinationX;

                                gBS->CopyMem( &((*BltBuffer)[(UINTN)Index]), &( MyBuffer[(UINTN)TranslatedIndex]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return EFI_SUCCESS;
}

/**
  Blt pixels from the rectangle (Width X Height) formed by the BltBuffer
  onto the graphics screen starting a location (X, Y). (0, 0) is defined as
  the upper left hand side of the screen. (X, Y) can be outside of the
  current screen geometry and the BltBuffer will be cliped when it is
  displayed. X and Y can be negative or positive. If Width or Height is
  bigger than the current video screen the image will be clipped.

  @param  This                   Protocol instance pointer.
  @param  BltBuffer              Buffer containing data to blt into video buffer.
                                 This  buffer has a size of
                                 Width*Height*Delta
  @param  BltOperation           Operation to perform on BltBuffer and video memory  
  @param  SourceX                If the BltOperation is a EfiCopyBlt this is the
                                 source of the copy. For other BLT operations this
                                 argument is not used.
  @param  SourceX                If the BltOperation is a EfiCopyBlt this is the
                                 source of the copy. For other BLT operations this
                                 argument is not used.
  @param  DestinationX           X location on graphics screen.
  @param  DestinationY           Y location on the graphics screen.
  @param  Width                  Width of BltBuffer.
  @param  Height                 Hight of BltBuffer
  @param  Delta                  Optionally provided width of the BltBuffer


  @retval EFI_SUCCESS            The palette is updated with PaletteArray.
  @retval EFI_INVALID_PARAMETER  BltOperation is not valid.
  @retval EFI_DEVICE_ERROR       A hardware error occurred writing to the video
                                 buffer.

**/
EFI_STATUS EFIAPI BltRotate 
(
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *BltBuffer,     OPTIONAL
    IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION   BltOperation,
    IN  UINTN                               SourceX,
    IN  UINTN                               SourceY,
    IN  UINTN                               DestinationX,
    IN  UINTN                               DestinationY,
    IN  UINTN                               Width,
    IN  UINTN                               Height,
    IN  UINTN                               Delta           OPTIONAL
)
{
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private = NULL;

    Private = GetPrivateFromGop(This);
    if(Private != NULL)
        return PerformTranslations(This, &BltBuffer, BltOperation, &SourceX, &SourceY, &DestinationX, &DestinationY, &Width, &Height, &Delta, Private);
    return EFI_DEVICE_ERROR;
}
