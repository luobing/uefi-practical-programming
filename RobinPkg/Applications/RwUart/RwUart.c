/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "common.h"
#include "Keyboard.h"
#include "SerialPort.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#include <IndustryStandard/Bmp.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>


/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int
main (
  IN int Argc,
  IN char **Argv
  )
{

	//串口测试
	EFI_STATUS Status;
	UINT8 lbtest[]="luobing,test...";
	UINT8 readData[1024];
	UINTN i,readLength=4;
  UINT32 controlBits;
  Status =		LocateSerialIO();  //gSeriaoIO定位到第一个串口
	readData[0]=0;
	
	DisplayAllSerialMode();
	if (EFI_ERROR (Status)) 
	{
		Print(L"Call LocateSerialIO, Can't find device!\n");
	}
	// WaitKey();
	Status=SendDataToSerial(sizeof(lbtest)/sizeof(UINT8),lbtest);
	WaitKey();
	Status=SendDataToSerial(sizeof(lbtest)/sizeof(UINT8),lbtest);
	WaitKey();
  
	while(readData[0]!='Q')
	{
    Delayms(2);
    Status = GetSerialControlBits(&controlBits);
    if (!EFI_ERROR (Status)) 
    {
      if ((controlBits & EFI_SERIAL_INPUT_BUFFER_EMPTY) != 0)
      {
        Status = EFI_NOT_READY;
      } 
      else 
      {
        // Status = EFI_SUCCESS;
        //获取数据
        if ((controlBits & EFI_SERIAL_DATA_SET_READY) != EFI_SERIAL_DATA_SET_READY)
          Delayms(2);
        readLength = 1000;
        SetMem(readData, 1024, 0);
        Status=GetDataFromSerial(&readLength,readData);
        Print(L"First char:0x%x(hex), %c(ascii)\n",readData[0],readData[0]);
        Print(L"All data: ");
        for (i = 0; i < readLength; i++)
          Print(L"%c", readData[i]);
        Print(L"\n");
      }
    } 
   
    
  }
  ResetSerialPort();
  Delayms(1000);
   // Print(L"Exit Serial IO sample...!");
			
	return 0;
}



