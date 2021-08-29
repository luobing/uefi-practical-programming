/** @file
  function definitions for internal to common functions.
 
**/

#ifndef _COMMON_H
#define _COMMON_H
//所有包含的目录到mdepkg下去找
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>
//#include <Library/PciLib.h>
//#include <Library/PciExpressLib.h>
//#include <IndustryStandard/Pci.h>
#include <Protocol/SimplePointer.h>		//获取鼠标?
#include <Protocol/SerialIo.h>	//串口？
#include <Protocol/PciIo.h>						//获取PciIO protocol所需
#include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
#include <IndustryStandard/Pci.h>  //pci访问所需的头文件，包含pci22.h,pci23.h...

#define S_TEXT_INPUT_EX  		0x0000000000000001
#define GRAPHICS_OUTPUT  		0x0000000000000002
#define PCI_ROOTBRIDGE_IO		0x0000000000000004
#define PCI_IO							0x0000000000000008

UINT64 InintGloabalProtocols(UINT64 flag);
EFI_STATUS LocateSimpleTextInputEx(void);
EFI_STATUS LocateGraphicsOutput (void);
EFI_STATUS LocatePCIRootBridgeIO(void);
EFI_STATUS LocatePCIIO(void);

EFI_STATUS LocateMouse(void);
VOID Delayms(IN UINTN ms);

//调试使用
EFI_STATUS LocateSerialIO(void);
//
//extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
//extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
//extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
//extern EFI_PCI_IO_PROTOCOL *gPCIIO;
//

#endif