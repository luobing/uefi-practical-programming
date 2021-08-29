/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include "MyProtocol.h"

// EFI_MYSAMPLE_PROTOCOL RobinSampleProtocol;

//-------------------------------------------------- Private data --------------------------
#define MY_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('U', 'E', 'F', 'I')


#define MY_PRIVATE_DATA_FROM_THIS(a) CR (a, MY_PRIVATE_DATA, myProtocol, MY_PRIVATE_DATA_SIGNATURE)
MY_PRIVATE_DATA gMyData;

//--------------------------------------------------  GUID data  ---------------------------
#define EFI_MYSAMPLE_PROTOCOL_GUID \
{ \
    0xce345181, 0xabad, 0x11e2, {0x8e, 0x5f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
}

EFI_GUID gEfiMYSampleProtocolGUID = EFI_MYSAMPLE_PROTOCOL_GUID ;

//------------------------------------------  Defintion of protocols -------------------------
EFI_STATUS
EFIAPI
MySample_In(
        IN EFI_MYSAMPLE_PROTOCOL* This,
        IN CHAR16* UserString
        )
{
    MY_PRIVATE_DATA *mydata;
    mydata = MY_PRIVATE_DATA_FROM_THIS(This);
    mydata->myWord = 0x11;
    mydata->myDword = 0x1122;

    Print(L"MySample_In.\n");  
    Print(L"User's string:%s\n",UserString);
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MySample_Out(
        IN EFI_MYSAMPLE_PROTOCOL* This 
        )
{
    MY_PRIVATE_DATA *mydata;
    mydata = MY_PRIVATE_DATA_FROM_THIS(This);
    Print(L"mydata: myWord=0x%x,myDowrd=0x%x\n",mydata->myWord,mydata->myDword);
    Print(L"MySample_Out.\n");
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MySample_DoSth(
        IN EFI_MYSAMPLE_PROTOCOL* This,
        IN CHAR16* UserString
        )
{
    MY_PRIVATE_DATA *mydata;
    mydata = MY_PRIVATE_DATA_FROM_THIS(This);
    mydata->myWord = 0xAA;
    mydata->myDword = 0x55AA;

    Print(L"User's string:%s\n",UserString);
    Print(L"Do something... MySample_DoSth.\n");
    return EFI_SUCCESS;
}


EFI_STATUS MySampleProtocolInit(VOID)
{
    MY_PRIVATE_DATA *mydata =&gMyData;
    mydata->Signature = MY_PRIVATE_DATA_SIGNATURE;
    mydata->myProtocol.Revision=0x101;
    mydata->myProtocol.MySample_In=MySample_In;
    mydata->myProtocol.MySample_Out=MySample_Out;
    mydata->myProtocol.MySample_DoSth=MySample_DoSth;

    return EFI_SUCCESS;
}
