/** @file
  function definitions for internal to common functions.
	code by luobing 2020-3-19 9:47:01
**/

#ifndef _COMMON_H
#define _COMMON_H
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
#include <Protocol/PciIo.h>						//获取PciIO protocol所需
#include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
#include <IndustryStandard/Pci.h>  //pci访问所需的头文件，包含pci22.h,pci23.h...

#include <guid/FileInfo.h> //FileInfo
#include <Library/FileHandleLib.h>  //各种文件信息处理

#include <Protocol/SimpleFileSystem.h> // 
#include <IndustryStandard/Bmp.h> //for bmp
#include <Library/MemoryAllocationLib.h> //AllocatePool function...
#include <Protocol/HiiFont.h>
#include <Protocol/HiiImage.h>
#include <Protocol/HiiImageEx.h>
#include <Protocol/HiiImageDecoder.h>
#include "Protocol/HiiDatabase.h"

#define S_TEXT_INPUT_EX  		0x0000000000000001
#define GRAPHICS_OUTPUT  		0x0000000000000002
#define PCI_ROOTBRIDGE_IO		0x0000000000000004
#define PCI_IO							0x0000000000000008
#define FILE_IO             0x0000000000000010  //2019-06-10 19:53:27 luobing
#define HII_FONT						0x0000000000000020  //2019-6-8 7:39:47 luobing
#define HII_IMAGE           0x0000000000000040  //2020-9-27 16:32:22 luobing
#define HII_IMAGE_EX        0x0000000000000080  //2020-9-27 16:32:23 luobing
#define HII_IMAGE_DECODER   0x0000000000000100  //2020-9-27 16:32:24 luobing

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

//
//extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
//extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
//extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
//extern EFI_PCI_IO_PROTOCOL *gPCIIO;
//

#endif