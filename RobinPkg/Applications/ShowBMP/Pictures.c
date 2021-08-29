/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Graphic.h"
#include "Pictures.h"
#include "FileRW.h"
#include <IndustryStandard/Bmp.h> //for bmp

extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

/* ==================================== 拷贝bmp相关的数据到此处 begin======================================= */
/* ==================================== 拷贝bmp相关的数据  end======================================= */

//===================================== BMP Functions begin ==============================================
//Function Name: ShowBMP24True
//Input: fileName: bmp file,24 bit true color
//       x,y:      position in display screen
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS ShowBMP24True(CHAR16 *fileName,UINTN x,UINTN y)
{
  EFI_STATUS Status;
  VOID *buffBMP = NULL;   //存储图像的缓冲器
  BMP_IMAGE_HEADER imageHeader;
  EFI_FILE_PROTOCOL *file;
  UINTN bufLength = sizeof(BMP_IMAGE_HEADER);
  UINTN i, j, maxX, maxY, index;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
  UINT8 *pic=NULL;

  //1 open file
  Status = OpenFile(&file, fileName, EFI_FILE_MODE_READ);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP24True: OpenFile %r\n",Status);  
    return Status;
  }
  //2 get datas to buffer
  Status=ReadFile(file,&bufLength, &imageHeader);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP24True: ReadFile 1 %r\n",Status);  
    CloseFile(file);
    return Status;
  }
  buffBMP = AllocateZeroPool(imageHeader.Size);
  if(buffBMP == NULL)
  {
    Print(L"ShowBMP24True: Allocate memory fial!\n");  
    CloseFile(file);
    return EFI_BUFFER_TOO_SMALL;
  }
  SetFilePosition(file, 0); //文件最开始处;
  bufLength = (UINTN)(imageHeader.Size);
  Status=ReadFile(file,&bufLength, buffBMP); //bmp 数据读入
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP24True: ReadFile 2 %r\n",Status);  
    CloseFile(file);
    return Status;
  }
  
  pic=(UINT8 *)buffBMP;
  maxX = imageHeader.PixelWidth-1;
  maxY = imageHeader.PixelHeight-1;
    
  //从缓冲区中读入像素
  index = imageHeader.ImageOffset;
  for(i=0;i<=maxY;i++)
    for(j=x;j<=maxX+x;j++)
    {
      color.Blue = pic[index];
      color.Green = pic[index+1];
      color.Red = pic[index+2];
      color.Reserved = 0;
      putpixel(j,maxY+y-i,&color);
      index+=3;
    }
  FreePool(buffBMP);
  CloseFile(file);
  return EFI_SUCCESS;
}
//Function Name: ShowBMP256
//Input: fileName: bmp file,256 color
//       x,y:      position in display screen
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS ShowBMP256(CHAR16 *fileName,UINTN x,UINTN y)
{
  EFI_STATUS Status;
  VOID *buffBMP = NULL;   //存储图像的缓冲器
  BMP_IMAGE_HEADER imageHeader;
  EFI_FILE_PROTOCOL *file;
  UINTN bufLength = sizeof(BMP_IMAGE_HEADER);
  UINTN index,j,middleValue;
  UINT8 *middleBuff,*bmpdata,*bmpplatte;
  // UINT8 *bmpdata;
  // UINT8 *bmpplatte;

  //1 open file
  Status = OpenFile(&file, fileName, EFI_FILE_MODE_READ);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP256: OpenFile %r\n",Status);  
    return Status;
  }
  //2 get datas to buffer
  Status=ReadFile(file,&bufLength, &imageHeader);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP256: ReadFile 1 %r\n",Status);  
    CloseFile(file);
    return Status;
  }
  buffBMP = AllocateZeroPool(imageHeader.Size);
  if(buffBMP == NULL)
  {
    Print(L"ShowBMP256: Allocate memory fial!\n");  
    CloseFile(file);
    return EFI_BUFFER_TOO_SMALL;
  }
  SetFilePosition(file, 0); //文件最开始处;
  // SetFilePosition(file, imageHeader.ImageOffset); //点阵数据最开始处;
  bufLength = (UINTN)(imageHeader.Size);
  Status=ReadFile(file,&bufLength, buffBMP); //bmp 数据读入
  if(EFI_ERROR(Status))
  {
    Print(L"ShowBMP256: ReadFile 2 %r\n",Status);  
    CloseFile(file);
    return Status;
  }
  
  middleValue = (imageHeader.PixelHeight / 2);
  bmpdata=(UINT8 *)buffBMP + imageHeader.ImageOffset;
  bmpplatte=(UINT8 *)buffBMP + sizeof(BMP_IMAGE_HEADER);
  middleBuff = AllocateZeroPool(imageHeader.PixelWidth);
  for(j=0; j<middleValue; j++)
  {
    index = imageHeader.PixelHeight - 1 -j; 
    CopyMem(middleBuff,bmpdata+j*imageHeader.PixelWidth,imageHeader.PixelWidth);
    CopyMem(bmpdata+j*imageHeader.PixelWidth,bmpdata+index*imageHeader.PixelWidth,imageHeader.PixelWidth);
    CopyMem(bmpdata+index*imageHeader.PixelWidth,middleBuff,imageHeader.PixelWidth);
  }
  putBMP256(x,y,imageHeader.PixelWidth,imageHeader.PixelHeight,
            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)bmpplatte ,bmpdata,0);
  FreePool(middleBuff);
  FreePool(buffBMP);
  CloseFile(file);
  return EFI_SUCCESS;
}
//Function Name: putBMP256
//Input: x,y,Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,MaskColor=not display color in Picture
//Output: None
//可以根据图像的情况，选择一个不显示的颜色，以适应显示logo的情况
VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor)
{
  UINTN i,j;
  UINT32 index=0;
  UINT8 color_number;
  
  for(j=y;j<Height+y;j++)
    for(i=x;i<Width+x;i++)
    {
      color_number=Picture[index];
      if(color_number!=MaskColor)
        putpixel(i,j,&(ColorTable[color_number]));
      ++index;
    }
}

//Function Name: putBMP256_fast
//Input: x,y,Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,MaskColor=not display color in Picture
//Output: None
//注意1，实际上，MaskColor在当前的函数中是无效的。因为直接缓冲区写屏，导致有些不想写
//     的像素无法滤过。当然，如果底色是纯色的，可以添加代码去替代这部分。或者，添加
//     读屏的函数，将屏幕上的像素填充需要掩盖的位置，出来的效果也很好。这部分代码
//      暂时没有添加。 ---luobing 2013-5-18 21:54:40
//注意2，程序员必须保证图像的位置不能超过屏幕本身，否则...blt不会显示任何东西出来
VOID putBMP256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 index,count;
//	UINT16 num_bytes;
	UINT32 pcxdata;
	EFI_STATUS Status;
//	CHAR8 StrBuffer[30];
	
	index=0;	//图像数据的指针
	count=0;	//用来定位像素位置的计数器
	BltBufferSize = ((UINT32)Width *  (UINT32)Height * (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
	Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );//最大可以容纳2^30的256色pcx数据，即32678x32678的图，不会溢出
	//1 将数据拷贝到缓冲区中
	while(count<((UINT32)Width *  (UINT32)Height))
	{
		pcxdata =(UINT32)(Picture[index]);
		++index;
		BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
		BltBuffer[count].Green =ColorTable[pcxdata].Green;
		BltBuffer[count].Red =ColorTable[pcxdata].Red;
		BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;
		++count;
	}
	//2 图像直接写屏
	gGraphicsOutput->Blt (gGraphicsOutput,BltBuffer,EfiBltBufferToVideo,0,0,x,y,Width,Height,0);
	gBS->FreePool(BltBuffer);
}

//Function Name: putBMP256_fun
//Input: x,y,Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,MaskColor=not display color in Picture
//Output: None
//Just for fun 解决了不能使用maskcolor的问题
VOID putBMP256_fun(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 index,count;
	//	UINT16 num_bytes;
	UINT32 pcxdata;
	EFI_STATUS Status;
	//	CHAR8 StrBuffer[30];
	
	index=0;	//图像数据的指针
	count=0;	//用来定位像素位置的计数器
	BltBufferSize = ((UINT32)Width *  (UINT32)Height * (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
	Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );//最大可以容纳2^30的256色pcx数据，即32678x32678的图，不会溢出

	getRectImage(x,y,Width,Height,BltBuffer);  
	//1 将数据拷贝到缓冲区中
	while(count<((UINT32)Width *  (UINT32)Height))
	{
		pcxdata =(UINT32)(Picture[index]);
		if(MaskColor!=(UINT8)pcxdata)
		{
			BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
			BltBuffer[count].Green =ColorTable[pcxdata].Green;
			BltBuffer[count].Red =ColorTable[pcxdata].Red;
			BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;	
		}
		++index;
		++count;
	}
	//2 图像直接写屏
	gGraphicsOutput->Blt (gGraphicsOutput,BltBuffer,EfiBltBufferToVideo,0,0,x,y,Width,Height,0);
	gBS->FreePool(BltBuffer);
}

/**
  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  BmpImageSize  Number of bytes in BmpImage
  @param  GopBlt        Buffer containing GOP version of BmpImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
  @param  PixelWidth    Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  UINT32                        DataSizePerLine;
  BOOLEAN                       IsAllocated;
  UINT32                        ColorMapNum;

  if (sizeof (BMP_IMAGE_HEADER) > BmpImageSize) {
    return EFI_INVALID_PARAMETER;
  }

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }
  //
  // Doesn't support compress.
  //
  if (BmpHeader->CompressionType != 0) {
    return EFI_UNSUPPORTED;
  }
  //
  // Only support BITMAPINFOHEADER format.
  // BITMAPFILEHEADER + BITMAPINFOHEADER = BMP_IMAGE_HEADER
  //
  if (BmpHeader->HeaderSize != sizeof (BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize)) {
    return EFI_UNSUPPORTED;
  }

  //
  // The data size in each line must be 4 byte alignment.
  //
  DataSizePerLine = ((BmpHeader->PixelWidth * BmpHeader->BitPerPixel + 31) >> 3) & (~0x3);
  BltBufferSize = MultU64x32 (DataSizePerLine, BmpHeader->PixelHeight);
  if (BltBufferSize > (UINT32) ~0) {
    return EFI_INVALID_PARAMETER;
  }

	//  if ((BmpHeader->Size != BmpImageSize) ||
	//      (BmpHeader->Size < BmpHeader->ImageOffset) ||
	//      (BmpHeader->Size - BmpHeader->ImageOffset != BmpHeader->PixelHeight * DataSizePerLine)) //此处计算发现错误,是我的图出现问题了,不知道为什么多了两个字节
	//  {
	//    return EFI_INVALID_PARAMETER;
	//   }
if ((BmpHeader->Size != BmpImageSize) ||
     (BmpHeader->Size < BmpHeader->ImageOffset) )
 {
   return EFI_INVALID_PARAMETER;
  }

  // Print(L"lbdebug end\n");
  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));
  if (BmpHeader->ImageOffset < sizeof (BMP_IMAGE_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmpHeader->ImageOffset > sizeof (BMP_IMAGE_HEADER)) {
    switch (BmpHeader->BitPerPixel) {
      case 1:
        ColorMapNum = 2;
        break;
      case 4:
        ColorMapNum = 16;
        break;
      case 8:
        ColorMapNum = 256;
        break;
      default:
        ColorMapNum = 0;
        break;
      }
    if (BmpHeader->ImageOffset - sizeof (BMP_IMAGE_HEADER) != sizeof (BMP_COLOR_MAP) * ColorMapNum) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
 
  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *PixelWidth   = BmpHeader->PixelWidth;
  *PixelHeight  = BmpHeader->PixelHeight;

  // //luobing add 2019-06-14 15:34:46 lbdebug
  // Print(L"ConvertBmpToGopBlt *PixelWidth=0x%x,*PixelHeight=0x%x\n", *PixelWidth, *PixelHeight);
  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;
  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        if (IsAllocated) {
          FreePool (*GopBlt);
          *GopBlt = NULL;
        }
        return EFI_UNSUPPORTED;
        break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}

//===================================== BMP Functions end ==============================================