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
	IN	UINT32 HorRes,	//??
	IN 	UINT32 VerRes,	//??
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
//==为实现透明、渐隐等特殊效果，特别编写几个需要用到的底层函数===
//Function Name:  getRectImage
//Input: x1,y1,Width,Height,*ImageBuffer 
//Output: Void
//Description: Get image from video,ImageBuffer must greater than Width * Height *sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
VOID getRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	gGraphicsOutput->Blt (gGraphicsOutput,ImageBuffer,EfiBltVideoToBltBuffer,x1,y1,0,0,Width,Height,0);
}
//Function Name:  putRectImage
//Input: x1,y1,Width,Height,*ImageBuffer
//Output: Void
//Description: Get image from video,ImageBuffer must greater than Width * Height *sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
VOID putRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,
									IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
	gGraphicsOutput->Blt (gGraphicsOutput,ImageBuffer,EfiBltBufferToVideo,0,0,x1,y1,Width,Height,0);
}

//===========================================Graphic function end======================================

//================Debug function================
//Name: DisplayModeInformation
//Input:
//Output:
EFI_STATUS DisplayModeInformation(void)
{
	Print(L"Print Current Mode Message:\n");
	Print(L"::MaxMode=%02x\n",gGraphicsOutput->Mode->MaxMode);
	Print(L"::Current Mode=%02x\n",gGraphicsOutput->Mode->Mode);
	Print(L"::SizeOfInfo=%x\n",gGraphicsOutput->Mode->SizeOfInfo);
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
	Print(L"Print Mode %d Message:\n",ModeNumber);
	Print(L"::Info->Version=%d\n",Info->Version);
	Print(L"::Info->HorizontalResolution=%d\n",Info->HorizontalResolution);
	Print(L"::Info->VerticalResolution=%d\n",Info->VerticalResolution);
	Print(L"::Info->PixelsPerScanLine=%d\n",Info->PixelsPerScanLine);
	
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