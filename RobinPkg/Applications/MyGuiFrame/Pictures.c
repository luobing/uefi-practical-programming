/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Graphic.h"
#include "Pictures.h"
#include "FileRW.h"
#include "KeyBoard.h"


extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

/* 内部全局变量定义 */
const int ZIGZAG[64] =
{
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};


/* ==================================== 拷贝bmp相关的数据到此处 begin======================================= */
/* ==================================== 拷贝bmp相关的数据  end======================================= */

//================================================ BMP Functions begin ==============================================
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
//Just for fun 解决了不能使用markcolor的问题
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

//================================================ BMP Functions end ==============================================

//================================================ PCX Functions begin ==============================================
//Function Name: ShowPCX24True
//Input: fileName: bmp file,24 bit true color
//       x,y:      position in display screen
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS ShowPCX24True(CHAR16 *fileName,UINTN x,UINTN y)
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *file;
  PCX_HEADER PCXheader; //当前的pcx文件头
  UINTN bufLength = sizeof(PCX_HEADER);
  UINT32 Width, Height;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *bltBuffer=NULL;
  //1 open file
  Status = OpenFile(&file, fileName, EFI_FILE_MODE_READ);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowPCX24True: OpenFile %r\n",Status);  
    return Status;
  }

  Status=ReadFile(file,&bufLength, &PCXheader);
  if(EFI_ERROR(Status))
  {
    Print(L"ShowPCX24True: ReadFile 1 %r\n",Status);  
    CloseFile(file);
    return Status;
  }
  Width = (PCXheader.xMax - PCXheader.xMin) + 1;
  Height = (PCXheader.yMax - PCXheader.yMin) + 1;

  bltBuffer = AllocateZeroPool(sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)*Width*Height);
  if(bltBuffer == NULL)
  {
    Print(L"ShowPCX24True: Allocate memory fial!\n");  
    CloseFile(file);
    return EFI_BUFFER_TOO_SMALL;
  }
  decompressPCX24bits_fast(file, bltBuffer);
  putRectImage(x, y, Width, Height, bltBuffer);
  CloseFile(file);
  FreePool(bltBuffer);
  return EFI_SUCCESS;
}
//Function Name: decompressPCX24bits
//Input:  *file: Handle to PCX files
//        *BltBuffer: Caller must allocate enough memory 
//Output: *BltBuffer
//Description: 演示用的代码，解压24位真彩色的PCX图
EFI_STATUS decompressPCX24bits(IN EFI_FILE_PROTOCOL *file,OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer)
{
  EFI_STATUS Status;

  UINT32 Width, Height, x, y;  
  INT32 nbytes,count;//pcx数据区字节数
  UINT8 *pcximage, *lptemp, bPixel;
  PCX_HEADER curPCXheader; //当前的pcx文件头
  UINTN bufLength = sizeof(PCX_HEADER);

  SetFilePosition(file, 0); //文件最开始处;
  
  Status=ReadFile(file,&bufLength, &curPCXheader);
  if(Status!=EFI_SUCCESS)
    return Status;
  
  Width = (curPCXheader.xMax - curPCXheader.xMin) + 1;
  Height = (curPCXheader.yMax - curPCXheader.yMin) + 1;
  nbytes = (INT32)((UINT32)curPCXheader.BytesPerLine * (UINT32)curPCXheader.ColorPlanes * (UINT32)Height);

  //1 解码数据到缓冲区
  pcximage = AllocateZeroPool(nbytes);
  if(pcximage  ==  NULL)
    return EFI_ABORTED;

  lptemp = pcximage;
  while (nbytes > 0)
  {
    bufLength = 1;
    ReadFile(file, &bufLength, &bPixel);
  
    if ((bPixel & 0XC0) != 0XC0)
    { // Repeated group
	  		*pcximage++ = bPixel;
	  		--nbytes;
    }
    else
    {
      count = bPixel & 0x3F;   //get count of pixels
      ReadFile(file,&bufLength, &bPixel);
      if(count > nbytes)
      {
        FreePool(lptemp);
        return EFI_ABORTED;   //致命错误，得到的pixel数目大于剩余文件字节
      }

      nbytes -= count;
	  	while (--count >=0) *pcximage++ = bPixel;
    }
  }
  //2 将颜色数据输出到指定的bltBuffer中
  pcximage = lptemp;
  for (y = 0; y < Height; y++)
  {
    for (x = 0; x < Width; x++)
    {
      BltBuffer[y * Width + x].Red = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 0) + x];
      BltBuffer[y * Width + x].Green = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 1) + x]; 
      BltBuffer[y * Width + x].Blue = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 2) + x]; 
    }
  }
  FreePool(pcximage);
  return Status;
}

//Function Name: decompressPCX24bits_fast
//Input:  *file: Handle to PCX files
//        *BltBuffer: Caller must allocate enough memory 
//Output: *BltBuffer
//Description: 演示用的代码，解压24位真彩色的PCX图
EFI_STATUS decompressPCX24bits_fast(IN EFI_FILE_PROTOCOL *file,OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer)
{
   EFI_STATUS Status;

  UINT8 *fileimage=NULL;
  UINTN fileLength;
  EFI_FILE_INFO *fileInfo;

  PCX_HEADER curPCXheader; //当前的pcx文件头
  UINT32 Width, Height, x, y; 
  INT32 nbytes;//pcx数据区字节数
  UINT8 *pcximage=NULL;
  UINTN bufLength = sizeof(PCX_HEADER);

  UINT8 *tempPcxImage, *tempFileImage;
  UINT8 bPixel;
  INT32 count;

  //1 读取文件信息，申请足够的内存，准备将数据直接读取到内存中
  fileInfo = FileHandleGetInfo(file);  //EFI_FILE_HANDLE为EFI_FILE_PROTOCOL指针
  fileLength = (UINTN)(fileInfo->FileSize);
  // Print(L"lbdebug1 decodePCX24bits: fileLength=0x%x\n", fileLength);
  fileLength -= sizeof(PCX_HEADER);
  // Print(L"lbdebug2 decodePCX24bits: fileLength=0x%x\n", fileLength);
  fileimage = AllocateZeroPool(fileLength);

   if(fileimage  ==  NULL)
  {
    // Print(L"lbdebug3 decodePCX24bits: Allocate fileimage error!\n");
    FreePool(fileInfo);
    return EFI_ABORTED;
  }

  //2 读取PCX文件头信息，了解图像尺寸
  SetFilePosition(file, 0); //文件最开始处;
  
  Status=ReadFile(file,&bufLength, &curPCXheader);
  if(Status!=EFI_SUCCESS)
    return Status;
  Width = (curPCXheader.xMax - curPCXheader.xMin) + 1;
  Height = (curPCXheader.yMax - curPCXheader.yMin) + 1;
  nbytes = (INT32)((UINT32)curPCXheader.BytesPerLine * (UINT32)curPCXheader.ColorPlanes * (UINT32)Height);

  pcximage = AllocateZeroPool(nbytes);
  if(pcximage  ==  NULL)
  {
    FreePool(fileimage);
    FreePool(fileInfo);
    return EFI_ABORTED;
  }

  //3 将压缩图像先读入内存
  ReadFile(file, &fileLength, fileimage);
  // Print(L"lbdebug4 decodePCX24bits fileimage: 0x%x  0x%x  0x%x  0x%x \n", fileimage[0], fileimage[1], fileimage[2], fileimage[3]);
  // WaitKey();
  //4 解压图像，并存入指定的内存
  tempFileImage = fileimage;
  tempPcxImage = pcximage;
  while (nbytes > 0)
  {
    //  bPixel = tempFileImage[0];
    bPixel = *tempFileImage++; 

    if ((bPixel & 0XC0) != 0XC0)
    { // Repeated group
       *tempPcxImage++ = bPixel;
       --nbytes;
    }
    else
    {
      count = bPixel & 0x3F;   //get count of pixels
      bPixel = *tempFileImage++;

      if(count > nbytes)
      {
        FreePool(fileInfo);
        FreePool(fileimage);
        FreePool(pcximage);
        return EFI_ABORTED;   //致命错误，得到的pixel数目大于剩余文件字节
      }

      nbytes -= count;
	  	// while (--count >=0) *tempPcxImage++ = bPixel;
      SetMem(tempPcxImage, count, bPixel);
      tempPcxImage += count;
    }
  }
  //5 将颜色数据输出到指定的bltBuffer中
  for (y = 0; y < Height; y++)
  {
    for (x = 0; x < Width; x++)
    {
      BltBuffer[y * Width + x].Red = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 0) + x];
      BltBuffer[y * Width + x].Green = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 1) + x]; 
      BltBuffer[y * Width + x].Blue = pcximage[(UINT32)curPCXheader.BytesPerLine * (3 * y + 2) + x]; 
    }
  }
  
  
  FreePool(fileInfo);
  FreePool(fileimage);
  FreePool(pcximage);
}
//Function Name: putPCX256
//Input:  x,y,Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,MaskColor=not display color in Picture
//Output: None
//Description: display the 256 color pcx
//注意，这种显示方法速度有点慢，采用fast函数较好
VOID putPCX256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor)
{
	UINT32 index,count;
	UINT16 num_bytes;
	UINT8 pcxdata;
	UINTN putX,putY;
	
	index=0;	//图像数据的指针
	count=0;	//用来定位像素位置的计数器
	//压缩区数据处理
	while(count<=(((UINT32)Width) * ((UINT32)Height)))
	{
		// get the first piece of data
		pcxdata = Picture[index];
		++index;
		// is this a rle?
		if(pcxdata>=192)
		{
			// how many bytes in run?
			num_bytes = pcxdata-192;
			// get the actual data for the run
			pcxdata  = Picture[index];;
			++index;
			// replicate data in buffer num_bytes times
			while(num_bytes-->0)
			{
				putX=x+(count%Width);
				putY=y+(count/Width);
				if(pcxdata!=MaskColor)	//Mask颜色不显示
					putpixel(putX,putY,&(ColorTable[pcxdata]));
				++count;
			} // end while
		} // end if rle
		else
		{
			// actual data, just copy it into buffer at next location
			putX=x+(count%Width);
			putY=y+(count/Width);
			if(pcxdata!=MaskColor)	//Mask颜色不显示
				putpixel(putX,putY,&(ColorTable[pcxdata]));
			++count;
		} // end else not rle
	} // end while 
}

//Function Name: putPCX256_fast
//Input:  x,y,Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,MaskColor=not display color in Picture
//Output: None
//Description: display the 256 color pcx
//注意，MaskColor在这个函数中是没有作用的，当然也可以实现，但是
//   当前的函数应该可以满足不同环境下的要求，比如用内存拷贝来实现屏幕显示
//   之类的。需要实现时参考BMP256.c中的函数
VOID putPCX256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
//	UINT32 index,count;
//	UINT16 num_bytes;
//	UINT8 pcxdata;
	// EFI_STATUS Status;
	
//	index=0;	//图像数据的指针
//	count=0;	//用来定位像素位置的计数器
	BltBufferSize = ((UINT32)Width *  (UINT32)Height * (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
	gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );//最大可以容纳2^30的256色pcx数据，即32678x32678的图，不会溢出

	//1 将数据解压到缓冲区中
	decompressPCX256(Width,Height,ColorTable,Picture,BltBuffer);

	//2 图像直接写屏
	gGraphicsOutput->Blt (gGraphicsOutput,BltBuffer,EfiBltBufferToVideo,0,0,x,y,Width,Height,0);
//	FreePool(BltBuffer);
	gBS->FreePool(BltBuffer);
}

//Function Name: decompressPCX256
//Input:  Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,Buffer=store the decompressed datas
//Output: None
//Description: 
//注意，Buffer必须足够大，可以容纳需要解压的数据
VOID decompressPCX256(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer)
{
	UINT32 index,count;
	UINT16 num_bytes;
	UINT8 pcxdata;
	
	index=0;	//图像数据的指针
	count=0;	//用来定位像素位置的计数器
	// 将数据解压到缓冲区中
	while(count<(((UINT32)Width) * ((UINT32)Height)))
	{
		// get the first piece of data
		pcxdata = Picture[index];
		++index;
		// is this a rle?
		if(pcxdata>=192)
		{
			// how many bytes in run?
			num_bytes = pcxdata-192;
			// get the actual data for the run
			pcxdata  = Picture[index];;
			++index;
			// replicate data in buffer num_bytes times
			while(num_bytes-->0)
			{
				BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
				BltBuffer[count].Green =ColorTable[pcxdata].Green;
				BltBuffer[count].Red =ColorTable[pcxdata].Red;
				BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;
				count+=1;
			} // end while
		} // end if rle
		else
		{
			// actual data, just copy it into buffer at next location
			BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
			BltBuffer[count].Green =ColorTable[pcxdata].Green;
			BltBuffer[count].Red =ColorTable[pcxdata].Red;
			BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;
			count+=1;
		} // end else not rle
	} // end while 
}
//Function Name: decompressPCX256_spec
//Input:  Width,Height,ColorTable=Pointer to logo color table,
//       Picture=picture of logo,Buffer=store the decompressed datas
//Output: None
//Description: 
//注意，Buffer必须足够大，可以容纳需要解压的数据
//特别为透明处理特制的解压函数，当为MaskColor时，将对应EFI_GRAPHICS_OUTPUT_BLT_PIXEL
//的Reserved置为0xAA,在透明处理时还原为0
VOID decompressPCX256_special(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
              IN UINT8 MaskColor)
{
	UINT32 index,count;
	UINT16 num_bytes;
	UINT8 pcxdata;
	
	index=0;	//图像数据的指针
	count=0;	//用来定位像素位置的计数器
	// 将数据解压到缓冲区中
	while(count<(((UINT32)Width) * ((UINT32)Height)))
	{
		// get the first piece of data
		pcxdata = Picture[index];
		++index;
		// is this a rle?
		if(pcxdata>=192)
		{
			// how many bytes in run?
			num_bytes = pcxdata-192;
			// get the actual data for the run
			pcxdata  = Picture[index];;
			++index;
			// replicate data in buffer num_bytes times
			while(num_bytes-->0)
			{
				BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
				BltBuffer[count].Green =ColorTable[pcxdata].Green;
				BltBuffer[count].Red =ColorTable[pcxdata].Red;
				if(pcxdata==MaskColor)	//Mask颜色不显示
						BltBuffer[count].Reserved=0xAA;
				else
					BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;
				count+=1;
			} // end while
		} // end if rle
		else
		{
			// actual data, just copy it into buffer at next location
			BltBuffer[count].Blue =ColorTable[pcxdata].Blue;
			BltBuffer[count].Green =ColorTable[pcxdata].Green;
			BltBuffer[count].Red =ColorTable[pcxdata].Red;
			if(pcxdata==MaskColor)	//Mask颜色不显示
				BltBuffer[count].Reserved=0xAA;
			else
				BltBuffer[count].Reserved =ColorTable[pcxdata].Reserved;
			count+=1;
		} // end else not rle
	} // end while 
}
//================================================ PCX Functions end ==============================================
//================================================  JPEG Begin ================================================
//Function Name: ShowBMP24True
//Input: fileName: bmp file,24 bit true color
//       x,y:      position in display screen
//Output: EFI_SUCCESS: success
//        Other value: fail
EFI_STATUS ShowJPEG24True(char *fileName,UINTN x_,UINTN y_)
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *bltBuffer;
  void *jfif = NULL;
  BMP_FORJPG   bmp  = {0};
  int ret;
  int x, y;
  // int bytesPerLine;
  UINT32 position, index, jpgWidth,jpgHeight;
  UINT8 *bmpdata;

  //1 打开jpeg文件，并转换到对应的bmp文件中
  jfif = jfif_load(fileName);
  ret=jfif_decode(jfif, &bmp);
  //  printf("ret of jfif_decode=%d\n", ret);
  //  printf("bmp.stride=%d\n", bmp.stride);
  //  printf("bmp.width=%d\n", bmp.width);
  //  WaitKey();
  jfif_free(jfif);

  //2 将对应的图转换到bltBuffer中
  //24位bmp每行的字节数能整除4，颜色以3个字节Bleue  Green  Red依次存储
  // if((bmp.width * 3) % 4 ==0)
  //   bytesPerLine = bmp.width * 3;
  // else
  //   bytesPerLine = bmp.width * 3 + 4 - ((bmp.width * 3) % 4);

  bltBuffer = AllocateZeroPool(sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)* (bmp.height) * (bmp.width));
  index = 0;
  bmpdata = (UINT8 *)(bmp.pdata);
  for (y = 0; y < bmp.height; y++)
  {
    position = y * (bmp.stride);
    for (x = 0; x < bmp.width; x++)
    {
      bltBuffer[index].Blue = bmpdata[position++];
      bltBuffer[index].Green = bmpdata[position++];
      bltBuffer[index].Red = bmpdata[position++];
      bltBuffer[index].Reserved = 0;
      ++index;
    }
  }
  jpgHeight = bmp.height;
  jpgWidth = bmp.width;
  bmp_free(&bmp);
  //3  显示
  putRectImage(x_, y_, jpgWidth, jpgHeight, bltBuffer);
  FreePool(bltBuffer);
  return EFI_SUCCESS;
}
//==============================以下代码来自于ffjpeg begin================================
//Function Name: jfif_load
//Input:  *file: 文件名,char型
//Output: 读取之后的Jpeg信息及内容
//Description: 
void* jfif_load(char *file)
{
    JFIF *jfif   = NULL;
    FILE *fp     = NULL;
    int   header = 0;
    int   type   = 0;
    UINT32  size   = 0;
    UINT8 *buf    = NULL;
    int   ret    =-1;
    long  offset = 0;
    int   i;

    jfif = calloc(1, sizeof(JFIF));
    buf  = calloc(1, 0x10000);
    if (!jfif || !buf) goto done;

    fp = fopen(file, "rb");
    if (!fp) goto done;

    while (1) {
        do { header = fgetc(fp); } while (header != EOF && header != 0xff); // get header
        do { type   = fgetc(fp); } while (type   != EOF && type   == 0xff); // get type
        if (header == EOF || type == EOF) {
            printf("file eof !\n");
            break;
        }

        if ((type == 0xd8) || (type == 0xd9) || (type == 0x01) || (type >= 0xd0 && type <= 0xd7)) {
            size = 0;
        } else {
            size  = fgetc(fp) << 8;
            size |= fgetc(fp) << 0;
            size -= 2;
        }

        fread(buf, size, 1, fp);
        switch (type) {
        case 0xc0: // SOF0
            jfif->width    = (buf[3] << 8) | (buf[4] << 0);
            jfif->height   = (buf[1] << 8) | (buf[2] << 0);
            jfif->comp_num = buf[5];
            for (i=0; i<jfif->comp_num; i++) {
                jfif->comp_info[i].id = buf[6 + i * 3];
                jfif->comp_info[i].samp_factor_v = (buf[7 + i * 3] >> 0) & 0x0f;
                jfif->comp_info[i].samp_factor_h = (buf[7 + i * 3] >> 4) & 0x0f;
                jfif->comp_info[i].qtab_idx      =  buf[8 + i * 3];
            }
            break;

        case 0xda: // SOS
            jfif->comp_num = buf[0];
            for (i=0; i<jfif->comp_num; i++) {
                jfif->comp_info[i].id = buf[1 + i * 2];
                jfif->comp_info[i].htab_idx_ac = (buf[2 + i * 2] >> 0) & 0x0f;
                jfif->comp_info[i].htab_idx_dc = (buf[2 + i * 2] >> 4) & 0x0f;
            }
            offset = ftell(fp);
            ret    = 0;
            goto read_data;
            break;

        case 0xdb: { // DQT
                UINT8 *dqt = buf;
                while (size >= 65) {
                    int idx = dqt[0] & 0x0f;
                    int f16 = dqt[0] & 0xf0;
                    if (!jfif->pqtab[idx]) jfif->pqtab[idx] = malloc(64 * sizeof(int));
                    if (!jfif->pqtab[idx]) break;
                    if (f16) { // 16bit
                        for (i=0; i<64; i++) {
                            jfif->pqtab[idx][ZIGZAG[i]] = (dqt[1 + i * 2] << 8) | (dqt[2 + i * 2] << 0);
                        }
                    } else { // 8bit
                        for (i=0; i<64; i++) {
                            jfif->pqtab[idx][ZIGZAG[i]] = dqt[1 + i];
                        }
                    }
                    size -= 1 + 64 * (f16 ? 2 : 1);
                    dqt  += 1 + 64 * (f16 ? 2 : 1);
                }
            }
            break;

        case 0xc4: { // DHT
                UINT8 *dht = buf;
                while (size >= 17) {
                    int idx  = dht[0] & 0x0f;
                    int fac  = dht[0] & 0xf0;
                    int len  = 0;
                    for (i=1; i<1+16; i++) len += dht[i];
                    if (fac) {
                        if (!jfif->phcac[idx]) jfif->phcac[idx] = calloc(1, sizeof(HUFCODEC));
                        if ( jfif->phcac[idx]) memcpy(jfif->phcac[idx]->huftab, &dht[1], 16 + len);
                    } else {
                        if (!jfif->phcdc[idx]) jfif->phcdc[idx] = calloc(1, sizeof(HUFCODEC));
                        if ( jfif->phcdc[idx]) memcpy(jfif->phcdc[idx]->huftab, &dht[1], 16 + len);
                    }
                    size -= 17 + len;
                    dht  += 17 + len;
                }
            }
            break;
        }
    }

 read_data:
    fseek(fp, 0, SEEK_END);
    jfif->datalen = ftell(fp) - offset;
    jfif->databuf = malloc(jfif->datalen);
    if (jfif->databuf) {
        fseek(fp, offset, SEEK_SET);
        fread(jfif->databuf, jfif->datalen, 1, fp);
    }

 done:
    if (buf) free  (buf);
    if (fp ) fclose(fp );
    if (ret == -1) {
        jfif_free(jfif);
        jfif = NULL;
    }
    return jfif;
}
//Function Name: jfif_free
//Input:  *ctxt
//Output: 释放申请的一些内存
//Description: 
void jfif_free(void *ctxt)
{
    JFIF *jfif = (JFIF*)ctxt;
    int   i;
    if (!jfif) return;
    for (i=0; i<16; i++) {
        if (jfif->pqtab[i]) free(jfif->pqtab[i]);
        if (jfif->phcac[i]) free(jfif->phcac[i]);
        if (jfif->phcdc[i]) free(jfif->phcdc[i]);
    }
    if (jfif->databuf) free(jfif->databuf);
    free(jfif);
}

//Function Name: init_dct_module
//Input:   
//Output:  
//Description: 
static int g_inited         =  0;
static int g_fdctfactor[64] = {0};
static int g_idctfactor[64] = {0};
#define DCTSIZE  8
/* 内部全局变量定义 */
static const float AAN_DCT_FACTOR[DCTSIZE] = {
    1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
    1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
};
void init_dct_module(void)
{
    int   i, j;
    float factor[64];

    // check inited
    if (g_inited) return;
    else g_inited = 1;

    // fdct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        g_fdctfactor[i] = FLOAT2FIX(factor[i]);
    }

    // idct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        g_idctfactor[i] = FLOAT2FIX(factor[i]);
    }
}
//Function Name: init_fdct_ftab
//Input:   
//Output:  
//Description:
void init_fdct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] / qtab[i]);
    }
}
//Function Name: ALIGN
//Input:   
//Output:  
//Description:
static int ALIGN(int x, int y) {
    // y must be a power of 2.
    return (x + y - 1) & ~(y - 1);
}

//Function Name: bitstr_open
//Input:   
//Output:  
//Description:
enum {
    BITSTR_MEM = 0,
    BITSTR_FILE,
};
void* bitstr_open(int type, char *file, char *mode)
{
    switch (type) {
    case BITSTR_MEM : return mbitstr_open((void*)file, (int  )mode);
    case BITSTR_FILE: return fbitstr_open((char*)file, (char*)mode);
    }
    return NULL;
}
//Function Name: mbitstr_open
//Input:   
//Output:  
//Description:
static void* mbitstr_open(void *buf, int len)
{
    MBITSTR *context = calloc(1, sizeof(MBITSTR));
    if (!context) return NULL;
    context->type   = BITSTR_MEM;
    context->membuf = buf;
    context->memlen = len;
    return context;
}
//Function Name: fbitstr_open
//Input:   
//Output:  
//Description:
static void* fbitstr_open(char *file, char *mode)
{
    FBITSTR *context = calloc(1, sizeof(FBITSTR));
    if (!context) return NULL;

    context->type = BITSTR_FILE;
    context->fp   = fopen(file, mode);
    if (!context->fp) {
        free(context);
        return NULL;
    }
    else return context;
}
//Function Name: bitstr_get_bits
//Input:   
//Output:  
//Description:
int bitstr_get_bits(void *stream, int n)
{
    int buf = 0;
    while (n--) {
        buf <<= 1;
        buf  |= bitstr_getb(stream);
    }
    return buf;
}
//Function Name: bitstr_getc
//Input:   
//Output:  
//Description:
int bitstr_getc(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case BITSTR_MEM : return mbitstr_getc(stream);
    case BITSTR_FILE: return fbitstr_getc(stream);
    }
    return EOF;
}
//Function Name: mbitstr_getc
//Input:   
//Output:  
//Description:
static int mbitstr_getc(void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context || context->curpos >= context->memlen) return EOF;
    return context->membuf[context->curpos++];
}
//Function Name: fbitstr_getc
//Input:   
//Output:  
//Description:
static int fbitstr_getc(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    return fgetc(context->fp);
}

//Function Name: bitstr_getb
//Input:   
//Output:  
//Description:
#define USE_JPEG_BITSTR  1
int bitstr_getb(void *stream)
{
    int bit, flag = 0;
    FBITSTR *context = (FBITSTR*)stream;
    if (!context) return EOF;

    if (context->bitnum == 0) {
#if USE_JPEG_BITSTR
        do {
            context->bitbuf = bitstr_getc(stream);
            if (context->bitbuf == 0xff) flag = 1;
        } while (context->bitbuf != EOF && context->bitbuf == 0xff);
        if (flag && context->bitbuf == 0) context->bitbuf = 0xff;
#else
        context->bitbuf = bitstr_getc(stream);
#endif
        context->bitnum = 8;
        if (context->bitbuf == EOF) {
            return EOF;
        }
    }

    bit = (context->bitbuf >> 7) & (1 << 0);
    context->bitbuf <<= 1;
    context->bitnum--;
    return bit;
}
//Function Name: bitstr_close
//Input:   
//Output:  
//Description:
int bitstr_close(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case BITSTR_MEM : return mbitstr_close(stream);
    case BITSTR_FILE: return fbitstr_close(stream);
    }
    return EOF;
}
//Function Name: mbitstr_close
//Input:   
//Output:  
//Description:
static int mbitstr_close(void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context) return EOF;
    free(context);
    return 0;
}
//Function Name: fbitstr_close
//Input:   
//Output:  
//Description:
static int fbitstr_close(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    fclose(context->fp);
    free  (context);
    return 0;
}
//Function Name: huffman_decode_init
//Input:   
//Output:  
//Description:
void huffman_decode_init(HUFCODEC *phc)
{
    int i;

    /* 根据哈夫曼表构造 first 表和 index 表
       first[i] 表示长度为 i+1 的第一个码字的值
       index[i] 表示长度为 i+1 的第一个码字的索引 */
    phc->first[0] = 0 ;
    phc->index[0] = MAX_HUFFMAN_CODE_LEN;
    for (i=1; i<MAX_HUFFMAN_CODE_LEN; i++)
    {
        phc->first[i] = (phc->first[i-1] + phc->huftab[i-1]) << 1;
        phc->index[i] =  phc->index[i-1] + phc->huftab[i-1];
    }
}
//Function Name: huffman_decode_step
//Input:   
//Output:  
//Description:
int huffman_decode_step(HUFCODEC *phc)
{
    int bit;
    int code = 0;
    int len  = 0;
    int idx  = 0;

    /* 检查输入输出数据流的有效性 */
    if (!phc->input) return EOF;

    /* 从输入流读取码字 */
    while (1) {
        bit = bitstr_getb(phc->input);
        if ( bit == EOF) return EOF;
//      printf("%d, first = %d, len = %d\n", bit ? 1 : 0, phc->first[len], len);
        code <<= 1; code |= bit;
        if (code - phc->first[len] < phc->huftab[len]) break;
        if (++len == MAX_HUFFMAN_CODE_LEN) return EOF;
    }

    idx = phc->index[len] + (code - phc->first[len]);
//  printf("get code:%c len:%d, idx:%d\n\n", phc->huftab[idx], len, idx);
    return idx < MAX_HUFFMAN_CODE_LEN + 256 ? phc->huftab[idx] : EOF;
}
//Function Name: huffman_decode_done
//Input:   
//Output:  
//Description:
void huffman_decode_done(HUFCODEC *phc)
{
    // do nothing
}

//Function Name: category_decode
//Input:   
//Output:  
//Description:;
static int category_decode(int code, int  size)
{
    return code >= (1 << (size - 1)) ? code : code - (1 << size) + 1;
}

//Function Name: zigzag_decode
//Input:   
//Output:  
//Description:;
// const int ZIGZAG[64] =
// {
//      0,  1,  8, 16,  9,  2,  3, 10,
//     17, 24, 32, 25, 18, 11,  4,  5,
//     12, 19, 26, 33, 40, 48, 41, 34,
//     27, 20, 13,  6,  7, 14, 21, 28,
//     35, 42, 49, 56, 57, 50, 43, 36,
//     29, 22, 15, 23, 30, 37, 44, 51,
//     58, 59, 52, 45, 38, 31, 39, 46,
//     53, 60, 61, 54, 47, 55, 62, 63,
// };
void zigzag_decode(int *data)
{
    int buf[64], i;
    for (i=0; i<64; i++) buf [ZIGZAG[i]] = data[i];
    for (i=0; i<64; i++) data[i] = buf[i];
}
//Function Name: init_idct_ftab
//Input:   
//Output:  
//Description:
void init_idct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] * qtab[i]);
    }
}
//Function Name: idct2d8x8
//Input:   
//Output:  
//Description:
#define FIXQ          11
void idct2d8x8(int *data, int *ftab)
{
    int  tmp0,  tmp1,  tmp2,  tmp3;
    int  tmp4,  tmp5,  tmp6,  tmp7;
    int  tmp10, tmp11, tmp12, tmp13;
    int  z5, z10, z11, z12, z13;
    int *dataptr;
    int  ctr;

    ftab = ftab ? ftab : g_idctfactor;
    for (ctr=0; ctr<64; ctr++) {
        data[ctr] *= ftab[ctr];
    }

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        if ( dataptr[1] + dataptr[2] + dataptr[3] + dataptr[4]
           + dataptr[5] + dataptr[6] + dataptr[7] == 0 )
        {
            dataptr[1] = dataptr[0];
            dataptr[2] = dataptr[0];
            dataptr[3] = dataptr[0];
            dataptr[4] = dataptr[0];
            dataptr[5] = dataptr[0];
            dataptr[6] = dataptr[0];
            dataptr[7] = dataptr[0];

            dataptr += DCTSIZE;
            continue;
        }

        /* Even part */
        tmp0 = dataptr[0];
        tmp1 = dataptr[2];
        tmp2 = dataptr[4];
        tmp3 = dataptr[6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= FLOAT2FIX(1.414213562f); tmp12 >>= FIXQ;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[1];
        tmp5 = dataptr[3];
        tmp6 = dataptr[5];
        tmp7 = dataptr[7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= FLOAT2FIX(1.414213562f); tmp11 >>= FIXQ;

        z5 = (z10 + z12) * FLOAT2FIX(1.847759065f) >> FIXQ;  /*  2 * c2 */
        tmp10 = (FLOAT2FIX( 1.082392200f) * z12 >> FIXQ) - z5; /*  2 * (c2 - c6) */
        tmp12 = (FLOAT2FIX(-2.613125930f) * z10 >> FIXQ) + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[0] = tmp0 + tmp7;
        dataptr[7] = tmp0 - tmp7;
        dataptr[1] = tmp1 + tmp6;
        dataptr[6] = tmp1 - tmp6;
        dataptr[2] = tmp2 + tmp5;
        dataptr[5] = tmp2 - tmp5;
        dataptr[4] = tmp3 + tmp4;
        dataptr[3] = tmp3 - tmp4;

        dataptr += DCTSIZE;
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        /* Even part */
        tmp0 = dataptr[DCTSIZE * 0];
        tmp1 = dataptr[DCTSIZE * 2];
        tmp2 = dataptr[DCTSIZE * 4];
        tmp3 = dataptr[DCTSIZE * 6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= FLOAT2FIX(1.414213562f); tmp12 >>= FIXQ;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[DCTSIZE * 1];
        tmp5 = dataptr[DCTSIZE * 3];
        tmp6 = dataptr[DCTSIZE * 5];
        tmp7 = dataptr[DCTSIZE * 7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= FLOAT2FIX(1.414213562f); tmp11 >>= FIXQ;

        z5 = (z10 + z12) * FLOAT2FIX(1.847759065f) >> FIXQ;  /*  2 * c2 */
        tmp10 = (FLOAT2FIX( 1.082392200f) * z12 >> FIXQ) - z5; /*  2 * (c2 - c6) */
        tmp12 = (FLOAT2FIX(-2.613125930f) * z10 >> FIXQ) + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[DCTSIZE * 0] = tmp0 + tmp7;
        dataptr[DCTSIZE * 7] = tmp0 - tmp7;
        dataptr[DCTSIZE * 1] = tmp1 + tmp6;
        dataptr[DCTSIZE * 6] = tmp1 - tmp6;
        dataptr[DCTSIZE * 2] = tmp2 + tmp5;
        dataptr[DCTSIZE * 5] = tmp2 - tmp5;
        dataptr[DCTSIZE * 4] = tmp3 + tmp4;
        dataptr[DCTSIZE * 3] = tmp3 - tmp4;

        dataptr++; /* advance pointers to next column */
    }
}
//Function Name: bmp_create
//Input:   
//Output:  
//Description:
int bmp_create(BMP_FORJPG *pb, int w, int h)
{
    pb->width  = w;
    pb->height = h;
    pb->stride = ALIGN(w * 3, 4);
    pb->pdata  = malloc(pb->stride * h);
    return pb->pdata ? 0 : -1;
}
//Function Name: bmp_free
//Input:   
//Output:  
//Description:
void bmp_free(BMP_FORJPG *pb)
{
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
    pb->width  = 0;
    pb->height = 0;
    pb->stride = 0;
}
//Function Name: yuv_to_rgb
//Input:   
//Output:  
//Description:
/*
    y = ( 0.2990f * r + 0.5870f * g + 0.1140f * b - 128);
    u = (-0.1678f * r - 0.3313f * g + 0.5000f * b + 0  );
    v = ( 0.5000f * r - 0.4187f * g - 0.0813f * b + 0  );

    r = (y                + 1.40200f * v);
    g = (y - 0.34414f * u - 0.71414f * v);
    b = (y + 1.77200f * u               );
*/
// yuv - 32bit signed fixed q12
// rgb - 8bits unsigned integer
void yuv_to_rgb(int y, int u, int v, UINT8 *r, UINT8 *g, UINT8 *b)
{
    int tr, tg, tb;

    y += 128 << FIXQ;
    tr = (y + (FLOAT2FIX(1.40200f) * v >> FIXQ)) >> FIXQ;
    tg = (y - (FLOAT2FIX(0.34414f) * u >> FIXQ) - (FLOAT2FIX(0.71414f) * v >> FIXQ)) >> FIXQ;
    tb = (y + (FLOAT2FIX(1.77200f) * u >> FIXQ)) >> FIXQ;

    /* 饱和处理 */
    *r =(UINT8) (tr < 0 ? 0 : tr < 255 ? tr : 255);
    *g =(UINT8) (tg < 0 ? 0 : tg < 255 ? tg : 255);
    *b =(UINT8) (tb < 0 ? 0 : tb < 255 ? tb : 255);
}
//Function Name: jfif_decode
//Input:   
//Output:  
//Description: 
int jfif_decode(void *ctxt, BMP_FORJPG *pb)
{
    JFIF *jfif    = (JFIF*)ctxt;
    void *bs      = NULL;
    int  *ftab[16]= {0};
    int   dc[4]   = {0};
    int   mcuw, mcuh, mcuc, mcur, mcui, jw, jh;
    int   i, j, c, h, v, x, y;
    int   sfh_max = 0;
    int   sfv_max = 0;
    int   yuv_stride[3] = {0};
    int   yuv_height[3] = {0};
    int  *yuv_datbuf[3] = {0};
    int  *idst, *isrc;
    int  *ysrc, *usrc, *vsrc;
    UINT8 *bdst;
    int   ret = -1;

    if (!ctxt || !pb) {
        printf("invalid input params !\n");
        WaitKey();
        return -1;
    }

    // init dct module
    init_dct_module();

    //++ init ftab
    for (i=0; i<16; i++) {
        if (jfif->pqtab[i]) {
            ftab[i] = malloc(64 * sizeof(int));
            if (ftab[i]) {
                init_idct_ftab(ftab[i], jfif->pqtab[i]);
            } else {
                goto done;
            }
        }
    }
    //-- init ftab

    //++ calculate mcu info
    for (c=0; c<jfif->comp_num; c++) {
        if (sfh_max < jfif->comp_info[c].samp_factor_h) {
            sfh_max = jfif->comp_info[c].samp_factor_h;
        }
        if (sfv_max < jfif->comp_info[c].samp_factor_v) {
            sfv_max = jfif->comp_info[c].samp_factor_v;
        }
    }
    mcuw = sfh_max * 8;
    mcuh = sfv_max * 8;
    jw = ALIGN(jfif->width , mcuw);
    jh = ALIGN(jfif->height, mcuh);
    mcuc = jw / mcuw;
    mcur = jh / mcuh;
    //-- calculate mcu info

    // create yuv buffer for decoding
    yuv_stride[0] = jw;
    yuv_stride[1] = jw * jfif->comp_info[1].samp_factor_h / sfh_max;
    yuv_stride[2] = jw * jfif->comp_info[2].samp_factor_h / sfh_max;
    yuv_height[0] = jh;
    yuv_height[1] = jh * jfif->comp_info[1].samp_factor_v / sfv_max;
    yuv_height[2] = jh * jfif->comp_info[2].samp_factor_v / sfv_max;
    yuv_datbuf[0] = malloc(yuv_stride[0] * yuv_height[0] * sizeof(int));
    yuv_datbuf[1] = malloc(yuv_stride[1] * yuv_height[1] * sizeof(int));
    yuv_datbuf[2] = malloc(yuv_stride[2] * yuv_height[2] * sizeof(int));
    if (!yuv_datbuf[0] || !yuv_datbuf[1] || !yuv_datbuf[2]) {
      printf("create yuv buffer fail!\n");
      WaitKey();
        goto done;
    }

    // open bit stream
    bs = bitstr_open(BITSTR_MEM, (char*)jfif->databuf, (char*)jfif->datalen);
    if (!bs) {
        printf("failed to open bitstr for jfif_decode !");
        WaitKey();
        return -1;
    }

    // init huffman codec
    for (i=0; i<16; i++) {
        if (jfif->phcac[i]) {
            jfif->phcac[i]->input = bs;
            huffman_decode_init(jfif->phcac[i]);
        }
        if (jfif->phcdc[i]) {
            jfif->phcdc[i]->input = bs;
            huffman_decode_init(jfif->phcdc[i]);
        }
    }

    for (mcui=0; mcui<mcuc*mcur; mcui++) {
        for (c=0; c<jfif->comp_num; c++) {
            for (v=0; v<jfif->comp_info[c].samp_factor_v; v++) {
                for (h=0; h<jfif->comp_info[c].samp_factor_h; h++) {
                    HUFCODEC *hcac = jfif->phcac[jfif->comp_info[c].htab_idx_ac];
                    HUFCODEC *hcdc = jfif->phcdc[jfif->comp_info[c].htab_idx_dc];
                    int       fidx = jfif->comp_info[c].qtab_idx;
                    int size, znum, code;
                    int du[64] = {0};

                    //+ decode dc
                    size = huffman_decode_step(hcdc) & 0xf;
                    if (size) {
                        code = bitstr_get_bits(bs  , size);
                        code = category_decode(code, size);
                    }
                    else {
                        code = 0;
                    }
                    dc[c] += code;
                    du[0]  = dc[c];
                    //- decode dc

                    //+ decode ac
                    for (i=1; i<64; ) {
                        code = huffman_decode_step(hcac);
                        if (code <= 0) break;
                        size = (code >> 0) & 0xf;
                        znum = (code >> 4) & 0xf;
                        i   += znum;
                        code = bitstr_get_bits(bs  , size);
                        code = category_decode(code, size);
                        if (i < 64) du[i++] = code;
                    }
                    //- decode ac

                    // de-zigzag
                    zigzag_decode(du);

                    // idct
                    idct2d8x8(du, ftab[fidx]);

                    // copy du to yuv buffer
                    x    = ((mcui % mcuc) * mcuw + h * 8) * jfif->comp_info[c].samp_factor_h / sfh_max;
                    y    = ((mcui / mcuc) * mcuh + v * 8) * jfif->comp_info[c].samp_factor_v / sfv_max;
                    idst = yuv_datbuf[c] + y * yuv_stride[c] + x;
                    isrc = du;
                    for (i=0; i<8; i++) {
                        memcpy(idst, isrc, 8 * sizeof(int));
                        idst += yuv_stride[c];
                        isrc += 8;
                    }
                }
            }
        }
    }

    // close huffman codec
    for (i=0; i<16; i++) {
        if (jfif->phcac[i]) huffman_decode_done(jfif->phcac[i]);
        if (jfif->phcdc[i]) huffman_decode_done(jfif->phcdc[i]);
    }

    // close bit stream
    bitstr_close(bs);

    // create bitmap, and convert yuv to rgb
    bmp_create(pb, jfif->width, jfif->height);
    bdst = (UINT8 *)pb->pdata;
    ysrc = yuv_datbuf[0];
    for (i=0; i<jfif->height; i++) {
        int uy = i * jfif->comp_info[1].samp_factor_v / sfv_max;
        int vy = i * jfif->comp_info[2].samp_factor_v / sfv_max;
        for (j=0; j<jfif->width; j++) {
            int ux = j * jfif->comp_info[1].samp_factor_h / sfh_max;
            int vx = j * jfif->comp_info[2].samp_factor_h / sfh_max;
            usrc = yuv_datbuf[1] + uy * yuv_stride[1] + ux;
            vsrc = yuv_datbuf[2] + vy * yuv_stride[2] + vx;
            yuv_to_rgb(*ysrc, *vsrc, *usrc, bdst + 0, bdst + 1, bdst + 2);
            bdst += 3;
            ysrc += 1;
        }
        bdst -= jfif->width * 3;
        bdst += pb->stride;
        ysrc -= jfif->width * 1;
        ysrc += yuv_stride[0];
    }

    // success
    ret = 0;

done:
    if (yuv_datbuf[0]) free(yuv_datbuf[0]);
    if (yuv_datbuf[1]) free(yuv_datbuf[1]);
    if (yuv_datbuf[2]) free(yuv_datbuf[2]);
    //++ free ftab
    for (i=0; i<16; i++) {
        if (ftab[i]) {
            free(ftab[i]);
        }
    }
    //-- free ftab
    return ret;
}

//Function Name: bmp_save
//Input:   
//Output:  
int bmp_save(BMP_FORJPG *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    UINT8         *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + pb->stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biPlanes   = 1;
    header.biBitCount = 24;
    header.biSizeImage= pb->stride * pb->height;

    fp = fopen(file, "wb");
    if (fp) {
        fwrite(&header, sizeof(header), 1, fp);
        pdata = (UINT8*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fwrite(pdata, pb->stride, 1, fp);
        }
        fclose(fp);
    }

    return fp ? 0 : -1;
}
//==============================以下代码来自于ffjpeg end================================