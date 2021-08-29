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
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <wchar.h>
#include "Common.h"
#include "KeyBoard.h"
#include "Network.h"

EFI_STATUS TCP4Test(IN int Argc,
                    IN char **Argv);
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
	// UINT64 flag;
	
	// flag = InintGloabalProtocols(S_TEXT_INPUT_EX | GRAPHICS_OUTPUT | PCI_ROOTBRIDGE_IO | PCI_IO | FILE_IO | RNG_OUT);
	// Print(L"flag=%x\n",flag);
	
  TCP4Test(Argc,Argv);
  return 0;
}

EFI_STATUS TCP4Test(IN int Argc,
  IN char **Argv)
{
  EFI_STATUS Status = 0;
  UINTN myfd;
  CHAR8 *RecvBuffer = (CHAR8*) malloc(1024);
  UINTN recvLen = 0;
  UINT32 ServerIPAddr[4];
  UINT16 ServerPort;
  char msgStr[1024];
  //1 get the server ip and port
  if(Argc != 3)
  {
    printf("UEFI TCP Client by robin. Usage: %a <ServerIP> <port>\n", Argv[0]);
    Status = EFI_ABORTED;
    return Status;
  }
  else
  {
    sscanf(Argv[1], "%d.%d.%d.%d", &ServerIPAddr[0], &ServerIPAddr[1], &ServerIPAddr[2], &ServerIPAddr[3]);
    sscanf(Argv[2], "%d", &ServerPort);
  }
  //2 connect server
  myfd = CreateTCP4Socket();
  Status = ConnectTCP4Socket(myfd, MYIPV4(ServerIPAddr[0],ServerIPAddr[1],ServerIPAddr[2],ServerIPAddr[3]), ServerPort);
  
  //3 echo test
  while (1)
  {
    //4 send message to server and get message from server
    memset(msgStr, 0, 1024);

    printf("Please input message:");
    gets(msgStr); //get string 
    if (!strcmp(msgStr, "q") || !strcmp(msgStr, "Q"))
      break;
    Status = SendTCP4Socket(myfd, msgStr, AsciiStrLen(msgStr));
    Status = RecvTCP4Socket(myfd, RecvBuffer, 1024, &recvLen);
    RecvBuffer[recvLen] = '\0';
    printf("Message from server: %s\n", RecvBuffer);
  }

  Status = CloseTCP4Socket(myfd);
  if(EFI_ERROR(Status))
    Print(L"close socket, %r\n", Status);

  free(RecvBuffer);

  return Status;
}