/** @file
    A simple, basic, application showing how the Hello application could be
    built using the "Standard C Libraries" from StdLib.

    Robin's uefi framework application, 2020-3-20
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
//#include  <Uefi.h>
//#include  <Library/UefiLib.h>
//#include  <Library/ShellCEntryLib.h>
#include  <Uefi.h>
#include  <Library/UefiLib.h>
// #include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>
// #include <errno.h>
// #include <netdb.h>
// #include <string.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/socket.h>

// #include <stdlib.h>
// #include <wchar.h>
#include "Common.h"
#include "KeyBoard.h"
#include "Network.h"

EFI_STATUS TCP4Test(UINT32 Ip32, UINT16 Port);
EFI_STATUS UDP4Test(UINT32 Ip32,UINT16 Port);
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
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	UINT64 flag;
	
	flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO | RNG_OUT);
	Print(L"flag=%x\n",flag);
	// WaitKey();
	//text out test

	//Testnetwork
  UDP4Test(MYIPV4(192, 168, 1, 42), 8888);
  return 0;
}

EFI_STATUS UDP4Test(UINT32 Ip32,UINT16 Port)
{
  EFI_STATUS Status = 0;
  UINTN myfd;
  CHAR8 SendStr[] = "Hello,I'm a UDP client of UEFI.";
  CHAR8 *RecvBuffer1 = (CHAR8*) AllocatePool(1024);
  CHAR8 *RecvBuffer2 = (CHAR8*) AllocatePool(1024);
  UINTN recvLen = 0;

  myfd = CreateUDP4Socket();
  Status = ConfigUDP4Socket(myfd, Ip32, Port);
  
  Status = SendUDP4Socket(myfd, SendStr, AsciiStrLen(SendStr));
  Print(L"UDP4Test: SendUDP4Socket, %r\n", Status);
  gBS->Stall(1000);
  Print(L"UDP4Test: Length of SendStr is %d \n", AsciiStrLen(SendStr));
  Status = RecvUDP4Socket(myfd, RecvBuffer1, RecvBuffer2,1024, &recvLen, Ip32, Port);
  Print(L"UDP4Test: RecvUDP4Socket, %r\n", Status);
 
  Print(L"UDP4Test Recv: %d bytes\n", recvLen);
  // AsciiPrint("Recv raw data:%c %c %c %c \n", RecvBuffer[0],RecvBuffer[1],RecvBuffer[2],RecvBuffer[3]);
  // if(recvLen>100)
  //   RecvBuffer[100] = '\0';
  // RecvBuffer[recvLen] = '\0';
  // Print(L"UDP4Test: Recv data is: %a\n", RecvBuffer);
  CloseUDP4Socket(myfd);
  
  // if(EFI_ERROR(Status))
  //   gST->ConOut->OutputString(gST->ConOut,L"Close udp4 error!\n\r");

  FreePool(RecvBuffer1);
  FreePool(RecvBuffer2);

  return Status;
}

EFI_STATUS TCP4Test(UINT32 Ip32,UINT16 Port)
{
  EFI_STATUS Status = 0;
  UINTN myfd;
  CHAR8 SendStr[] = "Hello,I'm a TCP client of UEFI.";
  CHAR8 *RecvBuffer = (CHAR8*) AllocatePool(1024);
  UINTN recvLen = 0;

  myfd = CreateTCP4Socket();
  Status = ConnectTCP4Socket(myfd, Ip32, Port);
  
  Status = SendTCP4Socket(myfd, SendStr, AsciiStrLen(SendStr));
  Print(L"TCP4Test: SendTCP4Socket, %r\n", Status);
  gBS->Stall(1000);
  Print(L"TCP4Test: Length of SendStr is %d \n", AsciiStrLen(SendStr));
  Status = RecvTCP4Socket(myfd, RecvBuffer, 1024, &recvLen);
  Print(L"TCP4Test: RecvTCP4Socket, %r\n", Status);
 
  Print(L"TCP4Test Recv: %d bytes\n", recvLen);
  // AsciiPrint("Recv raw data:%c %c %c %c \n", RecvBuffer[0],RecvBuffer[1],RecvBuffer[2],RecvBuffer[3]);
  RecvBuffer[recvLen] = '\0';
  Print(L"TCP4Test: Recv data is: %a\n", RecvBuffer);
  Status = CloseTCP4Socket(myfd);
  if(EFI_ERROR(Status))
    gST->ConOut->OutputString(gST->ConOut,L"Close tcp4 error!\n\r");

  FreePool(RecvBuffer);

  return Status;
}