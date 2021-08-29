/** @file
  function definitions for internal to common functions.
	code by luobing 2019-3-19 9:47:01
**/

#ifndef _COMMON_H
#define _COMMON_H

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <wchar.h>

//所有包含的目录到mdepkg下去找
#include  <Uefi.h>
#include  <Library/UefiLib.h>
// #include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>
//#include <Library/PciLib.h>
//#include <Library/PciExpressLib.h>
//#include <IndustryStandard/Pci.h>
#include <Protocol/UsbIo.h>        //USBio
#include <Protocol/UsbHostController.h> //USB2Hc
#include <Protocol/UsbFunctionIo.h>    //UsbFn
#include <Protocol/PciIo.h>						//获取PciIO protocol所需
#include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
#include <IndustryStandard/Pci.h>  //pci访问所需的头文件，包含pci22.h,pci23.h...
#include <Protocol/Rng.h>  //Random Number Generator Protocol 2019-08-31 11:32:03 robin
#include <Protocol/SimplePointer.h>		//Mouse

#include <Protocol/SimpleFileSystem.h> //文件系统访问
#include <IndustryStandard/Bmp.h> //for bmp
#include <Library/MemoryAllocationLib.h> //AllocatePool function...
#include <guid/FileInfo.h> //FileInfo
#include <Library/FileHandleLib.h>  //文件信息处理
#include <Protocol/HiiFont.h>
#include <Protocol/HiiImage.h>
#include <Protocol/HiiImageEx.h>
#include <Protocol/HiiImageDecoder.h>
#include <Protocol/HiiDatabase.h>

#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>  //gHiiString

#define S_TEXT_INPUT_EX  		0x0000000000000001
#define GRAPHICS_OUTPUT  		0x0000000000000002
#define PCI_ROOTBRIDGE_IO		0x0000000000000004
#define PCI_IO							0x0000000000000008
#define FILE_IO             0x0000000000000010  //2019-06-10 19:53:27 luobing
#define HII_FONT						0x0000000000000020  //2019-6-8 7:39:47 luobing
#define HII_IMAGE           0x0000000000000040  //2020-9-27 16:32:22 luobing
#define HII_IMAGE_EX        0x0000000000000080  //2020-9-27 16:32:23 luobing
#define HII_IMAGE_DECODER   0x0000000000000100  //2020-9-27 16:32:24 luobing
#define RNG_OUT             0x0000000000000200  //2019-08-31 11:33:12 robin
#define SIMPLE_POINTER      0x0000000000000400  //2020-09-31 13:31:18 robin
#define USB_IO              0x0000000000000800  //2020-11-19 21:11:05 robin
#define USB2_HC             0x0000000000001000  //2020-11-21 21:03:44 robin
#define USBFN_IO            0x0000000000002000  //2020-11-22 11:21:51 robin
UINT64 InintGloabalProtocols(UINT64 flag);
EFI_STATUS LocateSimpleTextInputEx(void);
EFI_STATUS LocateGraphicsOutput (void);
EFI_STATUS LocatePCIRootBridgeIO(void);
EFI_STATUS LocatePCIIO(void);
EFI_STATUS LocateFileRoot(void);

EFI_STATUS LocateHiiFont(void);  //2019-6-8 7:38:56 luobing
EFI_STATUS LocateHiiImage(void);
EFI_STATUS LocateHiiImageEx(void);
EFI_STATUS LocateHiiImageDecoder(void) ;

EFI_STATUS LocateRNGOut(void);

EFI_STATUS LocateMouse(void);

EFI_STATUS LocateUsbIO(void);
EFI_STATUS LocateUsb2HC(void);
EFI_STATUS LocateUsbFnIO(void);
VOID Delayms(IN UINTN ms);
void delay_ms(unsigned short nms);

//
//extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
//extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
//extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
//extern EFI_PCI_IO_PROTOCOL *gPCIIO;
//

#endif