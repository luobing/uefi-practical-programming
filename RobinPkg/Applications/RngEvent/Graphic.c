/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/


#include "Graphic.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

//===========================================Graphic function begin====================================
//Name: GetGraphicModeNumber
//Input:
//Output:
EFI_STATUS GetGraphicModeNumber(
	IN	UINT32 HorRes,	// 
	IN 	UINT32 VerRes,	// 
	OUT UINT32 *ModeNum,
	OUT UINT32 *CurrentModeNum,
	OUT UINT32 *PixelsPerScanLine
)
{
	EFI_STATUS Status;
	UINT32 ModeIndex;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;

	*CurrentModeNum=gGraphicsOutput->Mode->Mode;	//store current mode 
	for(ModeIndex=0;ModeIndex<gGraphicsOutput->Mode->MaxMode;ModeIndex++)
	{
		Status=gGraphicsOutput->QueryMode(gGraphicsOutput,ModeIndex,&SizeOfInfo,&Info);
		if(!EFI_ERROR(Status))
		{
			if((Info->HorizontalResolution == HorRes) && (Info->VerticalResolution == VerRes))
			{
				*ModeNum=ModeIndex;
				return Status;
			}
		}
	}
	return EFI_NOT_FOUND;
}
//Name: SwitchGraphicsMode(BOOLEAN flag)
//Input:
//Output:
EFI_STATUS SwitchGraphicsMode(BOOLEAN flag)
{
	EFI_STATUS                         Status;
//	if(flag)
//	{
//		Status=gST->ConOut->SetMode(gST->ConOut,EfiConsoleControlScreenGraphics);
//	}
//	else
//	{
//		Status=gST->ConOut->SetMode(gST->ConOut,EfiConsoleControlScreenText);
//	}
	if(flag)
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, FALSE);
	}
	else
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, TRUE);
	}
	return Status;
}
//Function Name:  SetMyMode
//Input: ModeNumber
//Output: Void
//Description: Set Specify mode
VOID SetMyMode(UINT32 ModeNumber)
{
   gGraphicsOutput->SetMode(gGraphicsOutput,ModeNumber);
   return;
}
VOID putpixel(UINTN x,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
//  
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,y,1,1,0);
}
//Function Name:  HLine
//Input: x1,x2,y,color (x1<x2)
//Output: Void
//Description: Draw horizontal line
VOID HLine(UINTN x1,UINTN x2,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,maxx;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
  minx=(x1<x2)?x1:x2;
  maxx=(x1>x2)?x1:x2;
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,minx,y,(maxx-minx+1),1,0);  
}
//Function Name:  VLine
//Input: x,y1,y2,color(y1<y2)
//Output: Void
//Description: Draw vertical line
VOID VLine(UINTN x,UINTN y1,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN miny,maxy;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
  miny=(y1<y2)?y1:y2;
  maxy=(y1>y2)?y1:y2;
  
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,miny,1,(maxy-miny+1),0);  
}

//Function Name:  Line
//Input: x1,y1,x2,y2,color(y1<y2)
//Output: Void
//Description: Draw  line
VOID Line(UINTN x1, UINTN y1, UINTN x2, UINTN y2, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	/*使用Bresenham算法*/
	INTN d,dx,dy,dx2,dy2,dxy;
	INTN xinc,yinc;

	
	/* 参数准备 */
	dx = (INTN)((x2 > x1) ? (x2 - x1) : (x1 - x2));
	dx2 = dx << 1;
	dy = (INTN)((y2 > y1) ? (y2 - y1) : (y1 - y2));
	dy2 = dy << 1;
	xinc =(INTN) (x2 > x1) ? 1 : ((x2 == x1) ? 0 : (-1));
	yinc = (INTN)(y2 > y1) ? 1 : ((y2 == y1) ? 0 : (-1));
	putpixel(x1, y1, color);
	if(dx>=dy){
		d=dy2-dx;
		dxy=dy2-dx2;
		while(dx--){
			if(d<=0)d+=dy2;
			else{
				d+=dxy;
				y1+=yinc;
			}
			// if(((x1+xinc)<=maxX)&&(y1<=maxY))			/* 对超出屏幕范围的点不予显示 */
				putpixel(x1+=xinc,y1,color);
		}
	}
	else{
		d=dx2-dy;
		dxy=dx2-dy2;
		while(dy--){
			if(d<=0)d+=dx2;
			else{
				d+=dxy;
				x1+=xinc;
			}
			// if((x1<=maxX)&&((y1+yinc)<=maxY))			/* 对超出屏幕范围的点不予显示 */
				putpixel(x1,y1+=yinc,color);
		}
	}
}


//Function Name:  rectblock
//Input: x1,y1,x2,y2,color(y1<y2)
//Output: Void
//Description: Draw block rect
VOID  rectblock(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,miny,maxx,maxy;
  
  minx=(x1<x2)?x1:x2;
  miny=(y1<y2)?y1:y2;
  maxx=(x1>x2)?x1:x2;
  maxy=(y1>y2)?y1:y2;
  
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,minx,miny,(maxx-minx+1),(maxy-miny+1),0);   
}

//another function
//Function Name:  rectangle
//Input: x1,y1,x2,y2,color_number: Pointer to ColorTable(y1<y2)
//Output: Void
//Description: Draw Rectangle
VOID  rectangle(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN left,top,right,bottom,i;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;

  /* Location */
  left=(x1<x2)?x1:x2;
  right=(x1>x2)?x1:x2;
  top=(y1<y2)?y1:y2;
  bottom=(y1>y2)?y1:y2;
//  right=(right<maxX)?right:maxX;
//  bottom=(bottom<maxY)?bottom:maxY;       /* Cut off the graphic which out of screen */
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    
  for(i=left;i<right;i++)putpixel(i,top,color);
  for(i=top;i<bottom;i++)putpixel(right,i,color);
  for(i=right;i>left;i--)putpixel(i,bottom,color);
  for(i=bottom;i>top;i--)putpixel(left,i,color);
}
//Function Name:  circle
//Input: centerx,centery,radius,color
//Output: Void
//Description: Draw Rectangle
VOID  circle(UINTN centerx,UINTN centery,UINTN radius,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  signed int x,y,d;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
  
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if(!radius)return;
  y=(signed int)(radius);
  d=3-((signed int)(radius)<<1);
  
  
  /*see foxdisk3.01 by luobing*/
  putpixel(centerx,centery+y,color);        /*0,-1*/
  putpixel(centerx,centery-y,color);        /*0,1*/
  putpixel(centerx+y,centery,color);
  putpixel(centerx-y,centery,color);
  
  for(x=1;x<y;x++)
  {
    putpixel(centerx+x,centery+y,color);
    putpixel(centerx+x,centery-y,color);
    putpixel(centerx-x,centery-y,color);
    putpixel(centerx-x,centery+y,color);
    putpixel(centerx+y,centery+x,color);
    putpixel(centerx+y,centery-x,color);
    putpixel(centerx-y,centery-x,color);
    putpixel(centerx-y,centery+x,color);
    if(d<0)d+=((x<<2)+6);           
    else d+=(((x-(y--))<<2)+10);        
  }
  /* x=y */
  putpixel(centerx+y,centery+y,color);
  putpixel(centerx+y,centery-y,color);
  putpixel(centerx-y,centery-y,color);
  putpixel(centerx-y,centery+y,color); 
}   
//============================== 为实现透明、渐隐等特殊效果，特别编写几个需要用到的底层函数 ===========================
//Function Name:  getRectImage
//Input: x1,y1,Width,Height,*ImageBuffer 
//Output: Void
//Description: Get image from video,ImageBuffer must greater than Width * Height *sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
VOID getRectImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	gGraphicsOutput->Blt (gGraphicsOutput,ImageBuffer,EfiBltVideoToBltBuffer,x,y,0,0,Width,Height,0);
}
//Function Name:  putRectImage
//Input: x1,y1,Width,Height,*ImageBuffer
//Output: Void
//Description: Set image to video,ImageBuffer must greater than Width * Height *sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
VOID putRectImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
									IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	gGraphicsOutput->Blt (gGraphicsOutput,ImageBuffer,EfiBltBufferToVideo,0,0,x,y,Width,Height,0);
}
//Function Name:  putRectImage/putPartRectImage_FOOL
//Input: x1,y1,Width,Height,distanceX,distanceY,partWidth,partHeight,*ImageBuffer
//Output: Void
//Description: x1,y1：屏幕上显示图像左上角的起始位置； Width,Height: 图像本身的大小；
//             distanceX,distanceY:距离图像左上角的位置的坐标；partWidth,partHeight:需要显示部分图像的大小
//             ImageBuffer: 图像像素的内容
VOID putPartRectImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
					  IN UINTN imageX, IN UINTN imageY, IN UINTN partWidth, IN UINTN partHeight,
					  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	UINTN realPartWidth,realPartHeight;
	if(imageX>Width-1)
		return;
	if(imageY>Height-1)	//超过图像范围了，不显示
		return;
	if((imageX+partWidth) > (Width-1))
		realPartWidth = Width-1 - imageX + 1;
	else
		realPartWidth = partWidth;
	
	if((imageY + partHeight)>(Height-1))
		realPartHeight = Height-1 -imageY + 1;
	else
		realPartHeight = partHeight;
	
	//最后一个参数要注意
	gGraphicsOutput->Blt (gGraphicsOutput,ImageBuffer,EfiBltBufferToVideo,imageX,imageY,x+imageX,y+imageY,
											realPartWidth,realPartHeight,Width* sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

}
//下面这个是傻瓜版本，写完才记起来blt有更好的操作方法,见上一个函数
VOID putPartRectImage_FOOL(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
					  IN UINTN distanceX, IN UINTN distanceY, IN UINTN partWidth, IN UINTN partHeight,
					  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	UINTN y;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *partImageBuffer;
	UINTN realPartWidth,realPartHeight;
	UINT32                        BltBufferSize;
	
	if(distanceX>Width-1)
		return;
	if(distanceY>Height-1)	//超过图像范围了，不显示
		return;
	if((distanceX+partWidth-1) > (Width-1))
		realPartWidth = Width-1 - distanceX + 1;
	else
		realPartWidth = partWidth;
	
	if((distanceY + partHeight-1)>(Height-1))
		realPartHeight = Height-1 -distanceY + 1;
	else
		realPartHeight = partHeight;

	BltBufferSize = (UINT32)(realPartHeight*realPartWidth*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  	gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  BltBufferSize,
                  (VOID **) &partImageBuffer
                  );

	for(y=0;y<realPartHeight;y++)
	{
		CopyMem(&(partImageBuffer[y * realPartWidth]),&(ImageBuffer[(distanceY+y) * Width + distanceX]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * realPartWidth);
	}

	gGraphicsOutput->Blt (gGraphicsOutput,partImageBuffer,EfiBltBufferToVideo,0,0,x1+distanceX,y1+distanceY,realPartWidth,realPartHeight,0);

	FreePool(partImageBuffer);
}
//Function Name:  putImagePixel
//Input: x,y:   图像左上角像素在屏幕上的位置
//       Width,Height: 图像宽度和高度
//       imgX,imgY:    需要绘制点相对于图像左上角的位置
//       ImageBuffer:  图像的数据
//Output: Void
//Description:对图像缓冲器的指定位置点进行绘图，imgX,imgY是相对于图像左上角位置的坐标
VOID putImagePixel(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, 
					IN UINTN imgX, UINTN imgY, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;

	// if(imgX>Width-1)
	// 	return;
	// if(imgY>Height-1)
	// 	return;

	color = ImageBuffer[imgY * Width + imgX];
	putpixel(x + imgX, y + imgY, &color);
}
//Function Name:  putImageLine
//Input: x,y,Width,Height,imgX1,imgY1,imgX2,imgY2,maxX,maxY,ImageBuffer
//Output: Void
//Description:图像内画线函数
VOID putImageLine(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
			IN UINTN imageX1,IN UINTN imageY1, IN UINTN imageX2,IN UINTN imageY2,
			IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
		/*使用Bresenham算法*/
	INT16 d,dx,dy,dx2,dy2,dxy;
	INT16 xinc,yinc;
	INT16 x1, x2, y1, y2;

	x1 = (INT16)(imageX1);
	y1 = (INT16)(imageY1);
	x2 = (INT16)(imageX2);
	y2 = (INT16)(imageY2);

	/* 参数准备 */
	dx = (INT16)((x2 > x1) ? (x2 - x1) : (x1 - x2));
	dx2 = dx << 1;
	dy = (INT16)((y2 > y1) ? (y2 - y1) : (y1 - y2));
	dy2 = dy << 1;
	xinc =(INT16) (x2 > x1) ? 1 : ((x2 == x1) ? 0 : (-1));
	yinc = (INT16)(y2 > y1) ? 1 : ((y2 == y1) ? 0 : (-1));
	putImagePixel(x, y, Width,Height,x1,y1,ImageBuffer);
	if(dx>=dy){
		d=dy2-dx;
		dxy=dy2-dx2;
		while(dx--){
			if(d<=0)d+=dy2;
			else{
				d+=dxy;
				y1+=yinc;
			}
			// if(((x1+xinc)<=(INTN)(maxX))&&(y1<=(INTN)maxY))			/* 对超出屏幕范围的点不予显示 */
				putImagePixel(x, y, Width,Height,x1+=xinc,y1,ImageBuffer);
		}
	}
	else{
		d=dx2-dy;
		dxy=dx2-dy2;
		while(dy--){
			if(d<=0)d+=dx2;
			else{
				d+=dxy;
				x1+=xinc;
			}
			// if((x1<=(INTN)maxX)&&((y1+yinc)<=(INTN)maxY))			/* 对超出屏幕范围的点不予显示 */
				putImagePixel(x, y, Width,Height,x1,y1+=yinc,ImageBuffer);
		}
	}
}


//===========================================Graphic function end======================================
