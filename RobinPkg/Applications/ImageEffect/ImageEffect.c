/** @file
    A simple, basic, EDK II native, "hello" application to verify that
    we can build applications without LibC.

    Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/



#include "common.h"
#include "FileRW.h"
#include "KeyBoard.h"
#include "Graphic.h"
#include "Window.h"
#include "Pictures.h"

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/BaseLib.h>  //字符串转换
#include  <Library/ShellCEntryLib.h>

#include <Pi/PiFirmwareFile.h>


#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeServicesLib.h>

#define MyPic                      L"luobing.bmp"		//where the my bmp located 
// EFI_STATUS ShowBmp (void);


extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
// INTN
// EFIAPI
// ShellAppMain (
//   IN UINTN Argc,
//   IN CHAR16 **Argv
//   )
int
main (
  IN int Argc,
  IN char **Argv
  )
{
  UINT64 flag;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *bltBuffer;
  UINTN jpgWidth,jpgHeight;
  
  // int i;
  // printf("Argc=%d\n", Argc);
  // for (i = 0; i < Argc;i++)
  // {
  //   printf("internal:Argv[%d]= %s\n", i,Argv[i]);
  // }
  // WaitKey();

  if(Argc!=3)
  {
    printf("Syntax: %s N filename\n",Argv[0]);
    printf(">>N={'0'~ '9'}; filename=\"*.jpg\"\n");
    printf(">>Sample: %s 1 robin.jpg\n",Argv[0]);
    return 0;
  }
  flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO);
	Print(L"flag=%x\n",flag);
	WaitKey();
    
  Jpeg2BltBuffer(Argv[2],&bltBuffer,&jpgWidth,&jpgHeight);
    
  //3 显示
  SwitchGraphicsMode(TRUE);
  SetMyMode(0x3);
	SetBKG(&(gColorTable[3]));
  switch((int)(Argv[1][0]))
  {
    case '0':  //sample-0: 灰度转换
      putRectImage(100,100,jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      TransferToGray(100, 100, jpgWidth, jpgHeight);
      WaitKey();
      break;
    case '1': //sample-1: 透明显示
      putRectImage(100,100,jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      AlphaTransparent(200,200,jpgWidth,jpgHeight,bltBuffer,4);
      WaitKey();
      break;
    case '2': //sample-2: 淡入淡出
      SmoothInZone(100,100,jpgWidth,jpgHeight,bltBuffer);
      WaitKey();
      SmoothOutZone(100,100,jpgWidth,jpgHeight,bltBuffer);
      WaitKey();
      break;
    case '3': //sample-3: 镜像显示
      putRectImage(100,100,jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      ReviseImage(100,100,jpgWidth,jpgHeight,bltBuffer,1);
      WaitKey();
      ReviseImage(100,100,jpgWidth,jpgHeight,bltBuffer,2);
      WaitKey();
      break;
    case '4': //sample-4: 水滴效果
      RainFallShow(100, 100, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      SetBKG(&(gColorTable[3]));
      RainAscendShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      break;
    case '5': //sample-5: 拉幕效果
      hOpenCurtain(100, 100, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      SetBKG(&(gColorTable[3]));
      vOpenCurtain(100, 100, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      break;
    case '6': //sample-6:百叶窗效果
      hWindowShadesShow(100, 100, jpgWidth, jpgHeight, bltBuffer);//横向百叶窗
      WaitKey();
      SetBKG(&(gColorTable[3]));
      vWindowShadesShow(100, 100, jpgWidth, jpgHeight, bltBuffer); //纵向百叶窗
      WaitKey();
      break;
    case '7': //sample-7: 时钟效果
      SpiralShow(100, 100, jpgWidth, jpgHeight,  bltBuffer);
      WaitKey();
      SpiralShowClear(100, 100, jpgWidth, jpgHeight,  &(gColorTable[3]));
      WaitKey();
      break;
    case '8': //sample-8: 雨落效果
      RainFallShow(10, 100, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      SetBKG(&(gColorTable[3]));
      RainAscendShow(10, 100, jpgWidth, jpgHeight, bltBuffer);
      WaitKey();
      break;
    default:
      printf("NO Active assignment!\n");
      break;
  }
  
    

        
    //3-1 灰度转换
    // TransferToGray(100, 100, jpgWidth, jpgHeight);
    // WaitKey();
    //3-2 上下翻转、左右翻转、同时上下左右翻转
    // ReviseImage(100, 100, jpgWidth, jpgHeight,1);
    // WaitKey();
    // putRectImage(100, 100, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // ReviseImage(100, 100, jpgWidth, jpgHeight,2);
    // WaitKey();
    // putRectImage(100, 100, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // ReviseImage(100, 100, jpgWidth, jpgHeight,3);
    // WaitKey();
    //3-3 透明演示
    // AlphaTransparent(100,100,jpgWidth,jpgHeight,bltBuffer,10);
    // WaitKey();
    // AlphaTransparent(200,200,jpgWidth,jpgHeight,bltBuffer,5);
    // WaitKey();
    
    //3-4 拉幕效果
    // hOpenCurtain(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // for (index = 0; index < jpgWidth; index++)
    // {
    //   VLine(index, 0, jpgHeight - 1, &(gColorTable[3]));
    //   Delayms(2);
    // }
    // WaitKey();
    // vOpenCurtain(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // for (index = 0; index < jpgHeight; index++)
    // {
    //   HLine(0, jpgWidth-1, index, &(gColorTable[3]));
    //   Delayms(2);
    // }
    // WaitKey();
    //3-5 雨滴效果
    // RainFallShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // RainFallShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // SetBKG(&(gColorTable[3]));
    // RainAscendShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    //3-6 百叶窗显现和消隐
    // SetBKG(&(gColorTable[3]));
    // hWindowShadesShow(100, 100, jpgWidth, jpgHeight, bltBuffer);//横向百叶窗
    // WaitKey();
    // {
    //   //消隐
    //   INTN i, j;
    //   for(j=(INTN)(jpgWidth/10-1);j>=0;j--)
    //     for (i = 0; i < 10;i++)
    //     {
    //         rectblock(i * jpgWidth / 10 + j, 0, i * jpgWidth / 10 + j, jpgHeight - 1,&(gColorTable[3]));
    //         Delayms(2);
    //     }
         
    // }
    // WaitKey();
    // vWindowShadesShow(100, 100, jpgWidth, jpgHeight, bltBuffer); //纵向百叶窗
    // WaitKey();
    // {
    //   //消隐
    //   INTN i, j;
    //   for(j=(INTN)(jpgHeight/10);j>=0;j--)
    //     for (i = 0; i < 10;i++)
    //     {
    //         rectblock(0,i * jpgHeight / 10 + j, jpgWidth-1, i * jpgHeight / 10 + j,&(gColorTable[3]));
    //         Delayms(2);
    //     }
         
    // }
    // WaitKey();
    //3-7 从四角向中间显示
    // CornerToCenter(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // SetBKG(&(gColorTable[3]));
    // CenterToCorner(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    //3-8 交错显示
    // hInterlaceShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // SetBKG(&(gColorTable[3]));
    // vInterlaceShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    //3-8 相互交错显示
    // hInteractiveShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // SetBKG(&(gColorTable[3]));
    // vInteractiveShow(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    //3-9 螺旋显示
    // SetBKG(&(gColorTable[3]));
    // SpiralShow(00, 0, jpgWidth, jpgHeight,  bltBuffer);
    // WaitKey();
    // SpiralShowClear(00, 0, jpgWidth, jpgHeight,  &(gColorTable[3]));
    // WaitKey();
    //3-10 渐隐
    // SetBKG(&(gColorTable[BLACK]));
    // putRectImage(0, 0, jpgWidth, jpgHeight, bltBuffer);
    // WaitKey();
    // DarkSpecifyZone(0,0,jpgWidth, jpgHeight,TRUE);
		// WaitKey();
		// DarkSpecifyZone(0,0,jpgWidth, jpgHeight,FALSE);
		// WaitKey();
    // SmoothInZone(0,0,jpgWidth, jpgHeight);
		// WaitKey();
    // SmoothOutZone(0,0,jpgWidth, jpgHeight);
		// WaitKey();
  FreePool(bltBuffer);
	SetMyMode(0x0);
	SwitchGraphicsMode(FALSE);     
  
  return(0);
}

