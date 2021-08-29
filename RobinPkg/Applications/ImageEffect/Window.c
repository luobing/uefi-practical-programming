//code by luobing 
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/

#include "Window.h"
#include "Graphic.h"

UINT32 SYGraphicsMode;
UINT32 OldGraphicsMode;
UINT32 SYPixPerScanLine;

//============================== Global color table ================================
EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[]={
  0,    0,    0,    0,        /*  BLACK */\
  255,  255,  255,  0,        /*  WHITE */\
  200,  208,  212,  0,        /*  LIGHT GRAY  */\
  167,  95, 18, 0,        /*  DEEP BLUE */\
  64, 64, 64, 0,        /*  DARK BLACK  */\
  128,  128,  128,  0,        /*  LIGHT BLACK */\
  0,    255,  255,  0,        /*  YELLOW  */\
  0,    153,  0,    0,        /*  GREEN */\
  0,    0,    153,  0,        /*  RED   */\
  192,  0,    0,    0,        /* BLUE   */  
};

//Function Name: SetBKG
//Input: color
//Output: None
//Description: set background
VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  rectblock(0,0,SY_SCREEN_WIDTH-1,SY_SCREEN_HEIGHT-1,color);
}
//Function Name: InSYGraphicsMode
//Input
//Output:
//Description: set mode
VOID InSYGraphicsMode(void)
{
	GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
	SetMyMode(SYGraphicsMode);
}
//Function Name: OutSYGraphicsMode
//Input
//Output:
//Description: set old mode
VOID OutSYGraphicsMode(void)
{
	//GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
	SetMyMode(OldGraphicsMode);
}


//=======实现特效的一些函数，基于Graphic中提供的底层函数实现==========
//Function Name: SimpleTransparent
//Input: x1,y1,Width,Height,*ImagePointer
//Output:
//Description: ImagePointer,指向Width*Height大小的图像缓冲区，其大小必定为Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
//实现最简单的透明效果，即使用Alpha方式，Alpha值为1/2
VOID SimpleTransparent(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 i;
	
	//1 申请同样大小的内存，准备使用
	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );
  //2 要覆盖区域的图像提取
  getRectImage(x,y,Width,Height,BltBuffer); 
  //3 对两段内存进行处理,选取Alpha=0.5   
  //Alpha混合公式如下：(A覆盖在B上)
	//                  R(C)=(1-alpha)*R(B) + alpha*R(A)
	//                  G(C)=(1-alpha)*G(B) + alpha*G(A)
	//                  B(C)=(1-alpha)*B(B) + alpha*B(A)
	for(i=0;i<((UINT32)Width * (UINT32)Height);i++)
	{
		BltBuffer[i].Blue=((BltBuffer[i].Blue)>>1) + ((ImagePointer[i].Blue)>>1);
		BltBuffer[i].Green=((BltBuffer[i].Green)>>1) + ((ImagePointer[i].Green)>>1);
		BltBuffer[i].Red=((BltBuffer[i].Red)>>1) + ((ImagePointer[i]).Red>>1);
		BltBuffer[i].Reserved=((BltBuffer[i].Reserved)>>1) + ((ImagePointer[i].Reserved)>>1);
	}
	//3 显示
	putRectImage(x,y,Width,Height,BltBuffer); 
	
  gBS->FreePool(BltBuffer);
}

//Function Name: AlphaTransparent
//Input: x1,y1,Width,Height,*ImagePointer,Alpha
//Output:
//Description: ImagePointer,指向Width*Height大小的图像缓冲区，其大小必定为Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
//实现透明效果，使用Alpha方式，Alpha值可调,从0-16可调
VOID AlphaTransparent(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer,
									IN UINT16 Alpha)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 i;
	
	//1 申请同样大小的内存，准备使用
	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );
  //2 要覆盖区域的图像提取
  getRectImage(x,y,Width,Height,BltBuffer); 
  //3 对两段内存进行处理,选取Alpha最大分母16,便于计算   
  //Alpha混合公式如下：(A覆盖在B上)
	//                  R(C)=(1-alpha)*R(B) + alpha*R(A)
	//                  G(C)=(1-alpha)*G(B) + alpha*G(A)
	//                  B(C)=(1-alpha)*B(B) + alpha*B(A)
	for(i=0;i<((UINT32)Width * (UINT32)Height);i++)
	{
		BltBuffer[i].Blue=(UINT8)(((16-Alpha)* BltBuffer[i].Blue + Alpha * ImagePointer[i].Blue)>>4);
		BltBuffer[i].Green=(UINT8)(((16-Alpha)* BltBuffer[i].Green + Alpha * ImagePointer[i].Green)>>4);
		BltBuffer[i].Red=(UINT8)(((16-Alpha)* BltBuffer[i].Red + Alpha * ImagePointer[i].Red)>>4);
		// BltBuffer[i].Reserved=(UINT8)(((16-Alpha)* BltBuffer[i].Reserved + Alpha * ImagePointer[i].Reserved)>>4);
		BltBuffer[i].Reserved=0;
	}
	//3 显示
	putRectImage(x,y,Width,Height,BltBuffer); 
	
  gBS->FreePool(BltBuffer);
}

//Function Name: DarkSpecifyZone
//Input: x1,y1,Width,Height,flag
//Output:
//Description:  Make the specify zone dark if flag=TRUE, otherwise,Light the zone
VOID DarkSpecifyZone(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,BOOLEAN flag)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 i;
	UINT8 value;
	
	if(flag)
		value=0;
	else
		value=255;
	//1 申请符合大小的内存，准备使用
	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBuffer
                  );
  //2 要覆盖区域的图像提取
  getRectImage(x,y,Width,Height,BltBuffer); 
  //3 对内存进行暗化处理  
  
	for(i=0;i<((UINT32)Width * (UINT32)Height);i+=2)
	{
		BltBuffer[i].Blue=value;
		BltBuffer[i].Green=value;
		BltBuffer[i].Red=value;
		BltBuffer[i].Reserved=value;
	}
	//4 显示
	putRectImage(x,y,Width,Height,BltBuffer); 
	
  gBS->FreePool(BltBuffer);
}

//Function Name: SmoothOutZone
//Input: x1,y1,Width,Height
//Output:
//Description: 逐渐变黑，实现渐隐效果
VOID SmoothOutZone(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINT32                        BltBufferSize;
	UINT32 i;
	UINT16 j;
	//1 申请符合大小的内存，准备使用
  	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	BltBuffer = AllocateZeroPool(BltBufferSize);
  	// gBS->AllocatePool (
    //               EfiRuntimeServicesData,
    //               BltBufferSize,
    //               (VOID **) &BltBuffer
    //               );
	//2 要覆盖区域的图像提取
	//   getRectImage(x,y,Width,Height,BltBuffer); 
	CopyMem(BltBuffer,ImagePointer,BltBufferSize);
	//3 对内存像素进行暗化处理,同时显示  
	for(j=0;j<256;j++)
	{
		for(i=0;i<((UINT32)Width * (UINT32)Height);i++)
		{
			if(BltBuffer[i].Blue>0)--BltBuffer[i].Blue;
			if(BltBuffer[i].Green>0)--BltBuffer[i].Green;
			if(BltBuffer[i].Red>0)--BltBuffer[i].Red;
			if(BltBuffer[i].Reserved>0)--BltBuffer[i].Reserved;
		}
		putRectImage(x,y,Width,Height,BltBuffer); 
		Delayms(5);
	}	
//   gBS->FreePool(BltBuffer);
	FreePool(BltBuffer);
}

// VOID SmoothOutZone(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height)
// {
// 	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
// 	UINT32                        BltBufferSize;
// 	UINT32 i;
// 	UINT16 j;
// 	//1 申请符合大小的内存，准备使用
// 	BltBufferSize = Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
//   gBS->AllocatePool (
//                   EfiRuntimeServicesData,
//                   BltBufferSize,
//                   (VOID **) &BltBuffer
//                   );
//   //2 要覆盖区域的图像提取
//   getRectImage(x,y,Width,Height,BltBuffer); 
//   //3 对内存像素进行暗化处理,同时显示  
//   for(j=0;j<256;j++)
//   {
// 		for(i=0;i<((UINT32)Width * (UINT32)Height);i++)
// 		{
// 			if(BltBuffer[i].Blue>0)--BltBuffer[i].Blue;
// 			if(BltBuffer[i].Green>0)--BltBuffer[i].Green;
// 			if(BltBuffer[i].Red>0)--BltBuffer[i].Red;
// 			if(BltBuffer[i].Reserved>0)--BltBuffer[i].Reserved;
// 		}
// 		putRectImage(x,y,Width,Height,BltBuffer); 
// 		Delayms(5);
// 	}	
//   gBS->FreePool(BltBuffer);
// }

//Function Name: SmoothInZone
//Input: x1,y1,Width,Height
//Output:
//Description: 将指定区域逐渐从黑变为实际图形，实现渐现效果
//注意，当前函数的原理：读取屏幕->把屏幕变黑->渐显为原来图案。明显这是个测试用函数，实际
//  需要的时候要改造它。
VOID SmoothInZone(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	// EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer1;
	UINT32 BltBufferSize;
	UINT32 i;
	UINT16 j;
	//1 申请符合大小的内存，准备使用
  	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	BltBuffer = AllocateZeroPool(BltBufferSize);
// 	BltBufferSize = Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
//   gBS->AllocatePool (
//                   EfiRuntimeServicesData,
//                   BltBufferSize,
//                   (VOID **) &BltBuffer
//                   );
//   gBS->AllocatePool (
//                   EfiRuntimeServicesData,
//                   BltBufferSize,
//                   (VOID **) &BltBuffer1
//                   );
//   gBS->SetMem(BltBuffer1,BltBufferSize,0);
  //2 要覆盖区域的图像提取,并把区域变黑
//   getRectImage(x,y,Width,Height,BltBuffer);
//   putRectImage(x,y,Width,Height,BltBuffer1); 
	
	//3 对内存像素进行暗化处理,同时显示  
	for(j=0;j<256;j++)
	{
		for(i=0;i<((UINT32)Width * (UINT32)Height);i++)
		{
			if(BltBuffer[i].Blue < ImagePointer[i].Blue) ++BltBuffer[i].Blue;
			if(BltBuffer[i].Green < ImagePointer[i].Green) ++BltBuffer[i].Green;
			if(BltBuffer[i].Red < ImagePointer[i].Red) ++BltBuffer[i].Red;
			if(BltBuffer[i].Reserved < ImagePointer[i].Reserved) ++BltBuffer[i].Reserved;
		}
		putRectImage(x,y,Width,Height,BltBuffer); 
		Delayms(10);
	}	
  FreePool(BltBuffer);
}
//Function Name: TransferToGray
//Input: x1,y1,Width,Height
//Output:
//Description:将指定区域的图像转换为灰度
VOID TransferToGray(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferSrc;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferDst;
	UINT32                        BltBufferSize;
	UINT32 i;
	UINT8 temp;
	// UINT16 j;
	//1 申请符合大小的内存，准备使用
	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  	gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBufferSrc
                  );
  	gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &BltBufferDst
                  );
 	gBS->SetMem(BltBufferDst,BltBufferSize,0);
 	//2 要覆盖区域的图像提取
 	getRectImage(x,y,Width,Height,BltBufferSrc);
 	
 	//3 对内存像素进行灰度处理,同时显示
	for (i = 0; i < (Width * Height); i++)
	{
		//一般的经验值灰度转换为：灰度值=0.3x红色分量  + 0.59x绿色分量 + 0.11x蓝色分量
		//修改一下:Red*32/128    Green*64/128   Blue*16/128
		//也即:  Red>>2   Green>>1   Blue>>3
		temp = ((BltBufferSrc[i].Red) >> 2) + ((BltBufferSrc[i].Green) >> 1) + ((BltBufferSrc[i].Blue) >> 3);
		BltBufferDst[i].Red = temp;
		BltBufferDst[i].Green = temp;
		BltBufferDst[i].Blue = temp;
		// BltBufferDst[i].Red = ((BltBufferSrc[i].Red) >> 2);
		// BltBufferDst[i].Green = ((BltBufferSrc[i].Green) >> 1);
		// BltBufferDst[i].Blue = ((BltBufferSrc[i].Blue) >> 3);
		BltBufferDst[i].Reserved = 0;
		
	}
	putRectImage(x, y, Width, Height, BltBufferDst);
			
 	gBS->FreePool(BltBufferSrc);
 	gBS->FreePool(BltBufferDst);
}	
//Function Name: ReviseImage
//Input: x1,y1,Width,Height,Flag
//Output:
//Description:将指定区域的图像颠倒 Flag=1:上下颠倒；Flag=2:左右颠倒；Flag=3:上下左右全部颠倒
VOID ReviseImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer,UINT8 Flag)
{
	// EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferSrc;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferDst;
	UINT32                        BltBufferSize;
	INTN xx, yy;   //这里在开发的时候犯错了
	UINT32 position;
	// UINT8 temp;
	// UINT16 j;
	//1 申请符合大小的内存，准备使用
	BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	BltBufferDst = AllocateZeroPool(BltBufferSize);
  	// gBS->AllocatePool (
    //               EfiRuntimeServicesData,
    //               BltBufferSize,
    //               (VOID **) &BltBufferSrc
    //               );
  	// gBS->AllocatePool (
    //               EfiRuntimeServicesData, 
    //               BltBufferSize,
    //               (VOID **) &BltBufferDst
    //               );
 	// gBS->SetMem(BltBufferDst,BltBufferSize,0); //defined in C:\MyWorkspace\MdePkg\Include\Library\BaseMemoryLib.h
 	//2 要覆盖区域的图像提取
 	// getRectImage(x,y,Width,Height,BltBufferSrc);
	position = 0;
	if((Flag & 0x1) == 0x1)  //上下颠倒
	{
		for (yy = (INTN)Height - 1; yy >= 0; yy--)
		{
			//copy src image to dst image
			CopyMem((BltBufferDst + position),(ImagePointer + (yy * Width)),Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
			position += (UINT32)(Width) ;
		}
		// CopyMem(ImagePointer,BltBufferDst,BltBufferSize);
	}
	
	if((Flag & 0x2) == 0x2) //左右颠倒
	{
		//gBS->SetMem(BltBufferDst,BltBufferSize,0); //defined in C:\MyWorkspace\MdePkg\Include\Library\BaseMemoryLib.h
		for (yy = 0; yy < (INTN)Height; yy++)
			for (xx = (INTN)Width - 1; xx >= 0; xx--)
				CopyMem(&(BltBufferDst[yy * Width + Width - 1 - xx]), &(ImagePointer[yy * Width + xx]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));	
	}
	putRectImage(x, y, Width, Height, BltBufferDst);			
 	// gBS->FreePool(BltBufferSrc);
 	FreePool(BltBufferDst);
}
//Function Name: hOpenCurtain
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:从中央水平打开图像，类似幕布效果
VOID hOpenCurtain(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i;

	for (i = 0; i < Width / 2;i++)
	{
		putPartRectImage(x, y, Width,Height, Width / 2 + i, 0, 1, Height, ImagePointer);
		putPartRectImage(x, y, Width, Height, Width / 2 - i -1, 0, 1, Height, ImagePointer);
		Delayms(6);
	}
}

//Function Name: hOpenCurtain
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:从中央垂直打开图像，类似幕布效果
VOID vOpenCurtain(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i;

	for (i = 0; i < Height / 2;i++)
	{
		putPartRectImage(x, y, Width,Height, 0, Height/2 + i, Width, 1, ImagePointer);
		putPartRectImage(x, y, Width, Height, 0,Height / 2 - i -1, Width, 1, ImagePointer);
		Delayms(6);
	}
}
//Function Name: RainFallShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:图像以下雨的效果显示，从上到下
// VOID RainFallShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
// {
// 	INTN i, j;

// 	// if((i=(INTN)(y+Height-1)) > (INTN)(Height-1))
// 	// 	i = (INTN)(Height - 1);
// 	if((i=(INTN)(y+Height-1)) > SY_SCREEN_HEIGHT)
// 		i = (INTN)(SY_SCREEN_HEIGHT - 1);
	
// 	// for (; i > (INTN)y; i--)
// 	// 	for (j = (INTN)y; j <= i;j++)
// 	// 		putPartRectImage(x, j, Width, Height, 0, i - y, Width, 1, ImagePointer);
// 	for (; i > (INTN)y; i--)
// 		for (j = (INTN)y; j <= i;j++)
// 			putPartRectImage(x, j, Width, Height, 0, i - y, Width, 1, ImagePointer);
// }
VOID RainFallShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN index,j;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINTN BltBufferSize = Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  	// gBS->AllocatePool (
    //               EfiRuntimeServicesData,
    //               BltBufferSize,
    //               (VOID **) &BltBuffer
    //               );
	BltBuffer = AllocateZeroPool(BltBufferSize);
	for(index = (INTN)(Height-1); index>=0; index--) //外层控制取数据
	{
		CopyMem(BltBuffer, &(ImagePointer[index*Width]), BltBufferSize);
		for(j=0; j<=index; j++)  //内层控制显示
		{
			// if(((j%2)==0) || (j==index))
			putRectImage(x,y+j,Width,1,BltBuffer);
		}
	}
	// gBS->FreePool(BltBuffer);
	FreePool(BltBuffer);
}
//Function Name: RainAscendShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:图像以下雨的效果显示，从上到下
VOID RainAscendShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN index,j;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	UINTN BltBufferSize = Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  	// gBS->AllocatePool (
    //               EfiRuntimeServicesData,
    //               BltBufferSize,
    //               (VOID **) &BltBuffer
    //               );
	BltBuffer = AllocateZeroPool(BltBufferSize);
	for(index = 0; index < (INTN)Height; index++) //外层控制取数据
	{
		CopyMem(BltBuffer, &(ImagePointer[index*Width]), BltBufferSize);
		for(j=(INTN)(Height-1); j>=index; j--)  //内层控制显示
		{
			putRectImage(x,y+j,Width,1,BltBuffer);
		}
	}
	// gBS->FreePool(BltBuffer);
	FreePool(BltBuffer);
}
// VOID RainAscendShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
// {
// 	INTN i, j;

// 	for (i = (INTN)y; i < (INTN)(y + Height);i++)
// 		for (j = (INTN)(y + Height - 1); j >= i;j--)
// 			putPartRectImage(x, j, Width, Height, 0, i - y, Width, 1, ImagePointer);
// }
//Function Name: hWindowShadesShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:横向显示百叶窗效果
VOID hWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	for (j = 0; j < Width / 10+1;j++) //Width不一定能被10整除，多余的会无法显示，所以应该先判断一下，或者这里+1，参考vWindowShadesShow
		for (i = 0; i < 10;i++)
		{
			//putPartRectImage(i * Width / 10 + j, 0, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			putPartRectImage(x, y, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			Delayms(2);
		}
}

//Function Name: vWindowShadesShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:纵向显示百叶窗效果
VOID vWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	for (j = 0; j < Height / 10+1;j++)
		for (i = 0; i < 10;i++)
		{
			//putPartRectImage(i * Width / 10 + j, 0, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			putPartRectImage(x, y, Width, Height, 0, i*Height/10+j, Width, 1, ImagePointer);
			Delayms(2);
		}
}
//Function Name: CornerToCenter
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description: 从四角向中间显示图案
VOID CornerToCenter(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	if(Width > Height)
	{
		for(i=0;i<=Height/2;i++)
		{
			putPartRectImage(x,y,Width,Height,0,0,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-i*Width/Height,0,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,0,Height-1-i,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-i*Width/Height,Height-1-i,i*Width/Height,i,ImagePointer);
			Delayms(2);
		}
	}
	else
	{
		for(j=0;j<=Width/2;i++)
		{
			putPartRectImage(x,y,Width,Height,0,0,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,0,Height-1-j*Height/Width,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-j,0,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-j,Height-1-i*Height/Width,j,j*Height/Width,ImagePointer);
			Delayms(2);
		}
	}		
}
//Function Name: CenterToCorner
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description: 从中间向四角显示图案 （未完成，有点累了，暂时不写这个函数）
VOID CenterToCorner(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN xx, yy, i;

	for (xx = (INTN)(Width / 2), yy = (INTN)(Height / 2), i = 1; i <= (INTN)(Height / 2);xx--,yy--,i++)
	{
		putPartRectImage(x, y, Width, Height, xx, yy, (Width-Height) + 2 * i, 2 * i, ImagePointer);
		Delayms(2);
	}
		
}
//Function Name: hInterlaceShow
//Input: x,y,Width,Height,ImagePointer
//Output: 
//Description:水平交错显示
VOID hInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	for (i = 1; i < Width - 1; i++)
		for (j = 0; j < Height - 1; j += 2)
		{
			putPartRectImage(x, y, Width, Height, Width - 1 - i, j, i, 1, ImagePointer);
			putPartRectImage(x, y, Width, Height, 0, j + 1, i, 1, ImagePointer);	
		}
}

//Function Name: vInterlaceShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description: 垂直交错显示
VOID vInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	for (j = 1; j < Height - 1;j++)
		for (i = 0; i < Width - 1;i+=8)
		{
			putPartRectImage(x, y, Width, Height, i,Height - 1 - j, 4, j, ImagePointer);
			putPartRectImage(x, y, Width, Height, i + 4, 0, 4, j, ImagePointer);
		}			
}
//Function Name: hInteractiveShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description: 
VOID hInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	//水平交错互补式显示,以lines行为单位
	for (i = 0; i < Width - 1; i++)
	{
		for (j = 0; j < (Height - 1) / 3; j += 2)  //for (j = 0; j < Height - 1 / lines; j += 2)
			putPartRectImage(x, y, Width, Height, i, j * 3, 1, 3, ImagePointer); //putPartRectImage(x, y, Width, Height, i, j * lines, 1, lines, ImagePointer);
		
	}

	for (i = 0; i < Width - 1; i++)
	{
		for (j = 1; j < (Height - 1) / 3; j += 2) //for (j = 1; j < (Height - 1) / lines; j += 2)
			putPartRectImage(x, y, Width, Height, Width - 1 - i, j * 3, 1, 3, ImagePointer); //putpartImage(x, y, Width, Height, Width - 1 - i, j * lines, 1, lines, ImagePointer);	
	}
}

//Function Name: vInteractiveShow
//Input: x,y,Width,Height,ImagePointer
//Output: 
//Description:
VOID vInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	//垂直交错互补式显示,以lines行为单位
	for (i = 0; i < Height - 1; i++)
	{
		for (j = 0; j < (Width - 1) / 3; j += 2)  //for (j = 0; j < Height - 1 / lines; j += 2)
			putPartRectImage(x, y, Width, Height, j * 3, i,  3, 1, ImagePointer); //putPartRectImage(x, y, Width, Height, j * lines, i,  lines, 1, ImagePointer); 
		
	}

	for (i = 0; i < Height - 1; i++)
	{
		for (j = 1; j < (Width - 1) / 3; j += 2) //for (j = 1; j < (Width - 1) / lines; j += 2)
			putPartRectImage(x, y, Width, Height, j * 3, Height - 1 - i, 3, 1, ImagePointer); //putPartRectImage(x, y, Width, Height, j * lines, Height - 1 - i, lines, 1, ImagePointer); 	
	}
}
//Function Name: SpiralShow
//Input: 
//Output: 
//Description: 螺旋显示
VOID SpiralShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN i;

	for (i = (INTN)((Width - 1) >> 1); i > 0; i--)
		putImageLine(x, y, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1, ImagePointer);
	for (i = 0; i < (INTN)(Height - 1); i++)
		putImageLine(x, y, Width, Height, 0, i, (Width - 1) >> 1, (Height - 1) >> 1,  ImagePointer);
	for (i = 0; i < (INTN)(Width - 1); i++)
		putImageLine(x, y, Width, Height, i, Height - 1 - 1, (Width - 1) >> 1, (Height - 1) >> 1, ImagePointer);
	for (i = (INTN)(Height - 1 - 1); i > 0; i--)
		putImageLine(x, y, Width, Height, Width - 1 - 1, i, (Width - 1) >> 1, (Height - 1) >> 1,  ImagePointer);
	for (i = (INTN)(Width - 1 - 1); i > (INTN)((Width - 1) >> 1); i--)
		putImageLine(x, y, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1,  ImagePointer);
}
//Function Name: SpiralShowClear
//Input: 
//Output: 
//Description: 螺旋显示清除
VOID SpiralShowClear(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	INTN i;

	for (i = (INTN)((Width - 1) >> 1); i < (INTN)(Width - 1); i++)
		Line(i+x, 0+y, ((Width - 1) >> 1) +x, ((Height - 1) >> 1)+y, color);
	for (i = 0; i < (INTN)(Height - 1); i++)
		Line(Width - 1+x, i+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
	for (i = (INTN)(Width - 1); i > 0; i--)
		Line(i+x, Height - 1+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
	for (i = (INTN)(Height - 1); i > 0; i--)
		Line(0+x, i+y, ((Width - 1) >> 1) +x, ((Height - 1) >> 1)+y,  color);
	for (i = 0; i < (INTN)((Width - 1) >> 1); i++)
		Line(i+x, 0+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
}