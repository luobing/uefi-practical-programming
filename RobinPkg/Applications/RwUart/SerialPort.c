/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "SerialPort.h"

extern EFI_SERIAL_IO_PROTOCOL *gSerialIO;
//Name: DisplayAllSerialMode
//Input: None
//Output: None
//Description:
EFI_STATUS DisplayAllSerialMode(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *SerialHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	EFI_SERIAL_IO_PROTOCOL *iSerialIO;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiSerialIoProtocolGuid,
		NULL,
		&HandleCount,
		&SerialHandleBuffer
		);
	if (EFI_ERROR(Status))	return Status;		//unsupport
	Print(L"Print All SerialPort(%d):\n",HandleCount);
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			SerialHandleBuffer[HandleIndex],
			&gEfiSerialIoProtocolGuid,
			(VOID**)&iSerialIO);
		if (EFI_ERROR(Status))	continue;
		else
		{
			//枚举所有设备的信息
			Print(L"Numer:%d, Handle=0x%x\n",HandleIndex,SerialHandleBuffer[HandleIndex]);
			Print(L"::ControlMask=0x%x\n",iSerialIO->Mode->ControlMask);
			Print(L"::Timeout=%d\n",iSerialIO->Mode->Timeout);
			Print(L"::BaudRate=%d\n",iSerialIO->Mode->BaudRate);
			Print(L"::ReceiveFifoDepth=%d\n",iSerialIO->Mode->ReceiveFifoDepth);
			Print(L"::DataBits=%d\n",iSerialIO->Mode->DataBits);
			Print(L"::Parity=%d\n",iSerialIO->Mode->Parity);
			Print(L"::StopBits=%d\n",iSerialIO->Mode->StopBits);
		}
	}

	return EFI_SUCCESS;
}

//Name: DisplaySerialMode
//Input: gSerialIO
//Output: None
//Description:
EFI_STATUS DisplaySerialMode(void)
{
	Print(L"Print Current Mode of SerialPort:\n");
	Print(L"::ControlMask=0x%x\n",gSerialIO->Mode->ControlMask);
	Print(L"::Timeout=%d\n",gSerialIO->Mode->Timeout);
	Print(L"::BaudRate=%d\n",gSerialIO->Mode->BaudRate);
	Print(L"::ReceiveFifoDepth=%d\n",gSerialIO->Mode->ReceiveFifoDepth);
	Print(L"::DataBits=%d\n",gSerialIO->Mode->DataBits);
	Print(L"::Parity=%d\n",gSerialIO->Mode->Parity);
	Print(L"::StopBits=%d\n",gSerialIO->Mode->StopBits);
	
	return EFI_SUCCESS;
}
//Name: GetSerialControlBits
//Input: gSerialIO,control
//Output: control
//Description:
EFI_STATUS GetSerialControlBits(UINT32 *control)
{
	UINT32 bitControl;
	EFI_STATUS Status;

	Status = gSerialIO->GetControl(gSerialIO, &bitControl);
	*control = bitControl;


	return Status;

}
//Name: SendDataToSerial
//Input: 
//Output: @retval EFI_SUCCESS      
//  @retval EFI_UNSUPPORTED  The serial device does not support this operation.
//  @retval EFI_DEVICE_ERROR The serial device is not functioning correctly.
//Description:
EFI_STATUS SendDataToSerial(UINTN length,VOID *buffer)
{
	EFI_STATUS Status;
	UINTN BufferSize;
	
	BufferSize=length;
	Status = gSerialIO->Write(gSerialIO,&BufferSize,buffer);
	return Status;
}

//Name: GetDataFromSerial
//Input: 
//Output: @retval EFI_SUCCESS      
//  @retval EFI_UNSUPPORTED  The serial device does not support this operation.
//  @retval EFI_DEVICE_ERROR The serial device is not functioning correctly.
//Description:
EFI_STATUS GetDataFromSerial(UINTN *length,VOID *buffer)
{
	EFI_STATUS Status;
	
	Status = gSerialIO->Read(gSerialIO,length,buffer);
	return Status;
}

//Name: SetSerialPortAttrib
//Input: gSerialIO
//      Parity:DefaultParity,NoParity,EvenParity,OddParity,MarkParity,SpaceParity
//			StopBits:DefaultStopBits,OneStopBit,OneFiveStopBits,TwoStopBits
//Output: EFI_SUCCESS: The new attributes were set on the serial device
//				EFI_INVALID_PARAMETRE: One or more of the attributes has an unsupported value
//				EFI_DEVICE_ERROR: The serial device is not functioning correctly
//Description:
EFI_STATUS SetSerialPortAttrib(UINT64 BaudRate, EFI_PARITY_TYPE Parity,UINT8 DataBits, EFI_STOP_BITS_TYPE StopBits)
{
	return gSerialIO->SetAttributes(gSerialIO,BaudRate,0,0,Parity,DataBits,StopBits);
}
//Name: ResetSerialPort
//Input: gSerialIO
//Output: None
//Description:
EFI_STATUS ResetSerialPort(void)
{
	EFI_STATUS Status;

	Status = gSerialIO->Reset(gSerialIO);
}