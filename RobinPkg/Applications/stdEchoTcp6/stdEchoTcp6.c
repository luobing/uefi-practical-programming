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


#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
//#include <Protocol/SimpleTextInEx.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

#include <sys/EfiSysCall.h>
#include <sys/endian.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h> //inet_pton
#include <stdio.h>

#include "Common.h"
#include "KeyBoard.h"
#include "Network.h"

EFI_STATUS tcp6Client(IN int Argc,
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
	// WaitKey();
	//text out test

	//Testnetwork
  tcp6Client(Argc, Argv);
  return 0;
}


EFI_STATUS tcp6Client(IN int Argc,
  IN char **Argv)
{
  EFI_STATUS Status = EFI_SUCCESS;
  struct sockaddr_in6 ServerIp;
  int socketID,recvLen;
  char msgStr[1024],recvStr[1024];

  //1 get the server ip and port
  if(Argc != 3)
  {
    printf("TCP6 Client by robin.Usage: %s <IP> <port>", Argv[1], atoi(Argv[2]));
    Status = EFI_ABORTED;
    return Status;
  }
  else
  {
    printf("Ready to start TCP6 Client. IP=%s,port=%d\n", Argv[1], atoi(Argv[2]));
  }
  //2 create a socket
  if ((socketID = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {      // IPv6
      printf("Socket()");
      Status = EFI_ABORTED;
      return Status;
  }
  //3 connet the server
  memset ( &ServerIp, 0, sizeof ( ServerIp ));
  ServerIp.sin6_family = AF_INET6;
  ServerIp.sin6_port = htons(atoi(Argv[2]));

  if ( inet_pton(AF_INET6, Argv[1], &ServerIp.sin6_addr) < 0 ) {                 // IPv6
      printf(Argv[1]);
      Status = EFI_ABORTED;
      return Status;
    }
  //3 connet the server
   if (connect(socketID, (struct sockaddr *) &ServerIp, sizeof(ServerIp)) != 0) {
      printf("Connect() ");
      Status = EFI_ABORTED;
      return Status;
  }
  
  //4 send message to server and get message from server
  while (1)
  {
    memset(msgStr, 0, 1024);

    printf("Please input message:");
    gets(msgStr); //get string from keyboard
    if (!strcmp(msgStr, "q") || !strcmp(msgStr, "Q"))
      break;
    send(socketID, msgStr, strlen(msgStr), 0);
     
    recvLen = (int)recv(socketID, recvStr, sizeof(recvStr), 0);
    recvStr[recvLen] = '\0';
    printf("Message from server: %s\n", recvStr);
  }
  close(socketID);  
  return Status;
}
