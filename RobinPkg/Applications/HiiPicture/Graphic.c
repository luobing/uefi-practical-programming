//code by luobing 
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
	IN	UINT32 HorRes,	//水平
	IN 	UINT32 VerRes,	//垂直
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
/**
 * @brief Put pixel to display screen
 * @param (x,y):  position of pixel
 * @param *color  color(IN)
 * @return  void
 */
VOID putpixel(UINTN x,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,y,1,1,0);
}
/**
 * @brief get pixel from display screen
 * @param (x,y):  position of pixel
 * @param *color  color(OUT)
 * @return  void
 */
VOID getpixel(UINTN x,UINTN y, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoToBltBuffer,x,y,0,0,1,1,0);
}
/**
 * @brief draw line
 * @param (x1,y1) -> (x2,y2)
 * @param *color  color
 * @return  void
 */
/*使用Bresenham算法*/
VOID line(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	INTN d,dx,dy,dx2,dy2,dxy;
	INTN xinc,yinc;

	dx=(INTN)((x2>x1)?(x2-x1):(x1-x2));
	dx2=dx<<1;
	dy=(INTN)((y2>y1)?(y2-y1):(y1-y2));
	dy2=dy<<1;
	xinc=(x2>x1)?1:((x2==x1)?0:(-1));
	yinc=(y2>y1)?1:((y2==y1)?0:(-1));
	putpixel(x1,y1,color);
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
//Function Name:  HLine
//Input: x1,x2,y,color (x1<x2)
//Output: Void
//Description: Draw horizontal line
VOID HLine(UINTN x1,UINTN x2,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,maxx;
  minx=(x1<x2)?x1:x2;
  maxx=(x1>x2)?x1:x2;

  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,minx,y,(maxx-minx+1),1,0);  
}
//Function Name:  VLine
//Input: x,y1,y2,color(y1<y2)
//Output: Void
//Description: Draw vertical line
VOID VLine(UINTN x,UINTN y1,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN miny,maxy;
  miny=(y1<y2)?y1:y2;
  maxy=(y1>y2)?y1:y2;
  
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,miny,1,(maxy-miny+1),0);  
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
//   UINTN left,top,right,bottom,i;
// //  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;

//   /* Location */
//   left=(x1<x2)?x1:x2;
//   right=(x1>x2)?x1:x2;
//   top=(y1<y2)?y1:y2;
//   bottom=(y1>y2)?y1:y2;
//  right=(right<maxX)?right:maxX;
//  bottom=(bottom<maxY)?bottom:maxY;       /* Cut off the graphic which out of screen */
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    
//   for(i=left;i<right;i++)putpixel(i,top,color);
//   for(i=top;i<bottom;i++)putpixel(right,i,color);
//   for(i=right;i>left;i--)putpixel(i,bottom,color);
//   for(i=bottom;i>top;i--)putpixel(left,i,color);
	UINTN minx,miny,maxx,maxy;
  
	minx=(x1<x2)?x1:x2;
	miny=(y1<y2)?y1:y2;
	maxx=(x1>x2)?x1:x2;
	maxy=(y1>y2)?y1:y2;

	HLine(minx,maxx,miny,color);
	VLine(minx,miny,maxy,color);
	HLine(minx,maxx,maxy,color);
	VLine(maxx,miny,maxy,color);

}
//Function Name:  circle
//Input: centerx,centery,radius,color
//Output: Void
//Description: Draw Rectangle
VOID  circle(UINTN centerx,UINTN centery,UINTN radius,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  INTN x,y,d;
 
  if(!radius)return;
  y=(INTN)(radius);
  d=3-((INTN)(radius)<<1);
  
  
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
//Function Name: SetBKG
//Input: color
//Output: None
//Description: set background
//VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
//{
//  rectblock(0,0,SY_SCREEN_WIDTH-1,SY_SCREEN_HEIGHT-1,color);
//}
//===========================================Graphic function end======================================

//================Debug function================
//Name: DisplayModeInformation
//Input:
//Output:
EFI_STATUS DisplayModeInformation(void)
{
	Print(L"Print Current Mode Message:\n");
	Print(L"::MaxMode=0x%02x\n",gGraphicsOutput->Mode->MaxMode);
	Print(L"::Current Mode=0x%02x\n",gGraphicsOutput->Mode->Mode);
	Print(L"::SizeOfInfo=0x%x\n",gGraphicsOutput->Mode->SizeOfInfo);
	Print(L"::FrameBufferBase=0x%lx\n",gGraphicsOutput->Mode->FrameBufferBase);
	Print(L"::FrameBufferSize=%x\n",gGraphicsOutput->Mode->FrameBufferSize);
	Print(L"::Info->Version=%x\n",gGraphicsOutput->Mode->Info->Version);
	Print(L"::Info->HorizontalResolution=%x\n",gGraphicsOutput->Mode->Info->HorizontalResolution);
	Print(L"::Info->VerticalResolution=%x\n",gGraphicsOutput->Mode->Info->VerticalResolution);
	Print(L"::Info->PixelsPerScanLine=%x\n",gGraphicsOutput->Mode->Info->PixelsPerScanLine);
	
	return EFI_SUCCESS;
}
//Name: DisplaySpecifyModeMessage(void)
//Input:
//Output:
EFI_STATUS DisplaySpecifyModeMessage(UINT32 ModeNumber)
{
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;
	EFI_STATUS                         Status;

	Status=gGraphicsOutput->QueryMode(gGraphicsOutput,ModeNumber,&SizeOfInfo,&Info);
	if(EFI_ERROR(Status))
	{
		Print(L"QueryMode(): %r\n",Status);
	}
	else
	{
		Print(L"Print Mode %d Message:\n",ModeNumber);
		Print(L"::Info->Version=%d\n",Info->Version);
		Print(L"::Info->HorizontalResolution=%d\n",Info->HorizontalResolution);
		Print(L"::Info->VerticalResolution=%d\n",Info->VerticalResolution);
		Print(L"::Info->PixelsPerScanLine=%d\n",Info->PixelsPerScanLine);
		Print(L"::Info->PixelFormat= %d\n",Info->PixelFormat);
		Print(L"::Info->PixelInformation:\n ");
		Print(L"0x%x  ",Info->PixelInformation.RedMask);
		Print(L"0x%x  ",Info->PixelInformation.GreenMask);
		Print(L"0x%x  ",Info->PixelInformation.BlueMask);
		Print(L"0x%x  ",Info->PixelInformation.ReservedMask);
		Print(L"\n ");
		gBS->FreePool(Info);
	}
		
	return Status;
}
//Name: DisplayTextModeInformation(void)
//Input:
//Output:
EFI_STATUS DisplayTextModeInformation(void)
{
	Print(L"Print SimPleTextMode Message:\n");
	Print(L"::MaxMode=%d\n",gST->ConOut->Mode->MaxMode);
	Print(L"::Current Mode=%d\n",gST->ConOut->Mode->Mode);
	Print(L"::Attribute=%d\n",gST->ConOut->Mode->Attribute);
	Print(L"::CursorColumn=%d\n",gST->ConOut->Mode->CursorColumn);
	Print(L"::CursorRow=%d\n",gST->ConOut->Mode->CursorRow);
	Print(L"::CursorVisible=%d\n",gST->ConOut->Mode->CursorVisible);
	
	return EFI_SUCCESS;	
}

//Name: DisplaySpecifyTextModeMessage(void)
//Input:
//Output:
EFI_STATUS DisplaySpecifyTextModeMessage(UINTN ModeNumber)
{
	EFI_STATUS                         Status;
	UINTN Columns;
	UINTN Rows;
	
	Status=gST->ConOut->QueryMode(gST->ConOut,ModeNumber,&Columns,&Rows);
	Print(L"Print Text Mode %d Message:\n",ModeNumber);
	Print(L"::Column=%d\n",Columns);
	Print(L"::Row=%d\n",Rows);
	
	return Status;
}