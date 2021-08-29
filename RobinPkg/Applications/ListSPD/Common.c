/** @file
  Member functions of global protocol and functions for creation,
  code by luobing  
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Common.h"

// EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
// EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
// EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
// EFI_PCI_IO_PROTOCOL *gPCIIO;
// EFI_SIMPLE_POINTER_PROTOCOL *gMouse;
// EFI_SERIAL_IO_PROTOCOL *gSerialIO;
EFI_SMBUS_HC_PROTOCOL *gSmbusHcProtocol;
// EFI_PEI_SMBUS_PPI *gSmbusPpi;

//================  Member function =======================================
//Name:InintGloabalProtocols
//Input:
//Output: 0: 所有protocol都被获取
//				非零: 所对应位标志的protocol没有被正确获取
//        
//Descriptor:
UINT64 InintGloabalProtocols(UINT64 flag)
{
	EFI_STATUS                        Status;
	UINT64	retVal=0;
	
	// //Locate SimpleTextInputEx protocol
	// if((flag&S_TEXT_INPUT_EX) == S_TEXT_INPUT_EX)
	// {
	// 	Status = LocateSimpleTextInputEx();
	// 	if (EFI_ERROR (Status)) 
	// 		retVal|=S_TEXT_INPUT_EX;
	// }
	
	// //Locate GraphicsOutput protocol
	// if((flag&GRAPHICS_OUTPUT) == GRAPHICS_OUTPUT)
	// {
	// 	Status = LocateGraphicsOutput();
	// 	if (EFI_ERROR (Status)) 
	// 		retVal|=GRAPHICS_OUTPUT;
	// }
	
	// //Locate PCIRootBridgeIO protocol
	// if((flag&PCI_ROOTBRIDGE_IO) == PCI_ROOTBRIDGE_IO)
	// {
	// 	Status = LocatePCIRootBridgeIO();
	// 	if (EFI_ERROR (Status)) 
	// 		retVal|=PCI_ROOTBRIDGE_IO;
	// }
	
	// //Locate PCIIO protocol
	// if((flag&PCI_IO) == PCI_IO)
	// {
	// 	Status = LocatePCIIO();
	// 	if (EFI_ERROR (Status)) 
	// 		retVal|=PCI_IO;
	// }
	
	//Locate SmbusHc protocol
	if((flag&SMBUSHC) == SMBUSHC)
	{
		Status = LocateSmbusHc();
		if (EFI_ERROR (Status)) 
			retVal|=SMBUSHC;
	}

	// //Locate SmbusPpi protocol
	// if((flag&SMBUSPPI) == SMBUSPPI)
	// {
	// 	Status = LocateSmbusHc();
	// 	if (EFI_ERROR (Status)) 
	// 		retVal|=SMBUSPPI;
	// }
	return retVal;
}

//Name: LocateSmbusHc
//Input: 
//Output: *gSmbusHcProtocol
//Descriptor:
EFI_STATUS LocateSmbusHc(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *SmbusHcHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiSmbusHcProtocolGuid,
		NULL,
		&HandleCount,
		&SmbusHcHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	Print(L"Find SMBus HC Handle:%d\n",HandleCount);
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			SmbusHcHandleBuffer[HandleIndex],
			&gEfiSmbusHcProtocolGuid,
			(VOID**)&gSmbusHcProtocol);
		if (EFI_ERROR(Status))	continue;
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}


//Function Name: Delayms
//Input: ms-milliseconds
//Output: none
//Description: delay by microsecond
VOID Delayms(IN UINTN ms)
{
  gBS->Stall(ms*1000);
}