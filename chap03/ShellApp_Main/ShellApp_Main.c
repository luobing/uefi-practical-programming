/** @file
  This is Sample of UEFI aplication. 
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

#include <Library/UefiBootServicesTableLib.h>  //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT
/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  EFI_TIME curTime;
  Print(L"Hello,this is Entry of ShellAppMain!\n");

  //使用BootService和RuntimeService
  // gST->BootServices->Stall(2000);  //延时2秒
  gBS->Stall(2000);
  // gST->RuntimeServices->GetTime(&curTime,NULL);
  gRT->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);

  //使用SystemTable
  gST->ConOut->OutputString(gST->ConOut,L"Test SystemTable...\n\r");

  return(0);
}
