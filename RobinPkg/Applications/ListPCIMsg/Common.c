/** @file
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Common.h"

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
EFI_PCI_IO_PROTOCOL *gPCIIOArray[256];
UINTN gPCIIO_Count;

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
	
	//Locate PCIRootBridgeIO protocol
	if((flag&PCI_ROOTBRIDGE_IO) == PCI_ROOTBRIDGE_IO)
	{
		Status = LocatePCIRootBridgeIO();
		if (EFI_ERROR (Status)) 
			retVal|=PCI_ROOTBRIDGE_IO;
	}
	
	//Locate PCIIO protocol
	if((flag&PCI_IO) == PCI_IO)
	{
		Status = LocatePCIIO();
		if (EFI_ERROR (Status)) 
			retVal|=PCI_IO;
	}
	
	return retVal;
}
//Name: LocatePCIRootBridgeIO
//Input: 
//Output: *gPCIRootBridgeIO
//Descriptor:
EFI_STATUS LocatePCIRootBridgeIO(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *PciHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiPciRootBridgeIoProtocolGuid,
		NULL,
		&HandleCount,
		&PciHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		
	Print(L"Find PCI Root Bridge I/O Protocol: %d\n",HandleCount);
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			PciHandleBuffer[HandleIndex],
			&gEfiPciRootBridgeIoProtocolGuid,
			(VOID**)&gPCIRootBridgeIO);
		if (EFI_ERROR(Status))	continue;
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}
//Name: LocatePCIIO
//Input: 
//Output: *gPCIIOArray[]
//Descriptor:
EFI_STATUS LocatePCIIO(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *PciHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiPciIoProtocolGuid,
		NULL,
		&HandleCount,
		&PciHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	gPCIIO_Count = HandleCount;
	Print(L"Find PCI I/O Protocol: %d\n",HandleCount);
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			PciHandleBuffer[HandleIndex],
			&gEfiPciIoProtocolGuid,
			(VOID**)&(gPCIIOArray[HandleIndex])); 
	}
	return Status;
}