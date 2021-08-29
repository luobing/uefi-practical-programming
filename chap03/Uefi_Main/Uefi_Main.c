/** @file
  This is Sample of UEFI aplication. 
**/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include <Uefi.h>
#include <Library/UefiLib.h>



/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_TIME curTime;
  Print(L"Hello,this is Entry of UefiMain!\n");

  //使用BootService和RuntimeService
  SystemTable->BootServices->Stall(2000);  //延时2秒
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);

  //使用SystemTable
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Test SystemTable...\n\r");


  return EFI_SUCCESS;
}
