/** @file
  Member functions of global protocol and functions for creation,
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Common.h"

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
EFI_PCI_IO_PROTOCOL *gPCIIO;
EFI_SIMPLE_POINTER_PROTOCOL *gMouse;
EFI_SERIAL_IO_PROTOCOL *gSerialIO;

EFI_SERIAL_IO_PROTOCOL *gSerialIOArray[256];
UINTN nSerialIO = 0;

//================  Member function =======================================
//Name:InintGloabalProtocols
//Input:
//Output: 0: ����protocol������ȡ
//				����: ����Ӧλ��־��protocolû�б���ȷ��ȡ
//        
//Descriptor:
UINT64 InintGloabalProtocols(UINT64 flag)
{
	EFI_STATUS                        Status;
	UINT64	retVal=0;
	
	//Locate SimpleTextInputEx protocol
	if((flag&S_TEXT_INPUT_EX) == S_TEXT_INPUT_EX)
	{
		Status = LocateSimpleTextInputEx();
		if (EFI_ERROR (Status)) 
			retVal|=S_TEXT_INPUT_EX;
	}
	
	//Locate GraphicsOutput protocol
	if((flag&GRAPHICS_OUTPUT) == GRAPHICS_OUTPUT)
	{
		Status = LocateGraphicsOutput();
		if (EFI_ERROR (Status)) 
			retVal|=GRAPHICS_OUTPUT;
	}
	
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
//Name:LocateSimpleTextInputEx
//Input:
//Output: EFI_STATUS=EFI_SUCCESS, *SimpleTextInputEx
//				EFI_STATUS=
//Descriptor: Get the handle of SimpleTextInputEx
EFI_STATUS LocateSimpleTextInputEx(void)
{
	EFI_STATUS                        Status;
  EFI_HANDLE                        *Handles;
  UINTN                             HandleCount;
  UINTN                             HandleIndex;

  
  Status = gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiSimpleTextInputExProtocolGuid,
              NULL,
              &HandleCount,
              &Handles
              );
  if(EFI_ERROR (Status))
  	return Status;

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &gSimpleTextInputEx);
    
    if (EFI_ERROR(Status))	
    	continue;
		else
		{
			return EFI_SUCCESS;
  	}
  }	 
	return Status;
}
//Name: LocateGraphicsOutput
//Input: 
//Output: *GraphicsOutput
//Descriptor:
EFI_STATUS LocateGraphicsOutput (void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *GraphicsOutputControllerHandles = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports GraphicsOutputProtocol
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiGraphicsOutputProtocolGuid,
		NULL,
		&HandleCount,
		&GraphicsOutputControllerHandles
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			GraphicsOutputControllerHandles[HandleIndex],
			&gEfiGraphicsOutputProtocolGuid,
			(VOID**)&gGraphicsOutput);
		if (EFI_ERROR(Status))	continue;

		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
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
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiPciRootBridgeIoProtocolGuid,
		NULL,
		&HandleCount,
		&PciHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
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
//Output: *gPCIIO
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
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			PciHandleBuffer[HandleIndex],
			&gEfiPciIoProtocolGuid,
			(VOID**)&gPCIIO);
		if (EFI_ERROR(Status))	continue;
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}

//Name: LocateMouse
//Input: 
//Output: *gMouse
//Descriptor:
EFI_STATUS LocateMouse(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *PointerHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiSimplePointerProtocolGuid,
		NULL,
		&HandleCount,
		&PointerHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			PointerHandleBuffer[HandleIndex],
			&gEfiSimplePointerProtocolGuid,
			(VOID**)&gMouse);
		if (EFI_ERROR(Status))	continue;
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}


//Name: LocateSerialIO
//Input: 
//Output: *gSerialIO
//Descriptor:�ҵ���һ�����ھ�ֹͣ��ʵ���ϲ�ֹһ��
EFI_STATUS LocateSerialIO(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *SerialHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiSerialIoProtocolGuid,
		NULL,
		&HandleCount,
		&SerialHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	nSerialIO = HandleCount;		//���洮����Ŀ
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			SerialHandleBuffer[HandleIndex],
			&gEfiSerialIoProtocolGuid,
			(VOID**)&(gSerialIOArray[HandleIndex]));
		if (EFI_ERROR(Status))	continue;
		// else
		// {
		// 	return EFI_SUCCESS;
		// }
	}
	gSerialIO = gSerialIOArray[1];
	
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