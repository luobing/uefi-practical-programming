/** @file
  This is Sample of UEFI library. 
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
 * Some functions of my library: 2020-06-13 12:14:35 robin
 **/

VOID LibFunction(VOID)
{
  Print(L"LibFunction() is called!\n");
}


RETURN_STATUS
EFIAPI
MyLibConstructor (VOID )
{
  Print(L"MyLibConstructor() is called!\n");
}

RETURN_STATUS
EFIAPI
MyLibDestructor (VOID)
{
  Print(L"MyLibConstructor() is called!\n");
}
