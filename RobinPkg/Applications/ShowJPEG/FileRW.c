/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/

#include "FileRW.h"

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem;  //操作FAT文件系统的句柄
extern EFI_FILE_PROTOCOL *gFileRoot;		//操作文件的句柄

//Name: OpenFile
//Input: fileHandle,fileName,OpenMode
//Output: if success,file's handle is *fileHandle 
//Descriptor: OpenMode:EFI_FILE_MODE_READ,EFI_FILE_MODE_WRITE,EFI_FILE_MODE_CREATE
//生成各种属性的文件，本函数不提供，需要的话可以修改
EFI_STATUS OpenFile(EFI_FILE_PROTOCOL **fileHandle,CHAR16 * fileName,UINT64 OpenMode)
{
  EFI_STATUS  Status = 0;
  Status = gFileRoot ->Open(
            gFileRoot,     
            fileHandle,
            (CHAR16*)fileName, 
            OpenMode,
            0   //如果OpenMode为Create属性，则此参数有效，参照spec
            );
  
  return Status;
}
//Name: CloseFile
//Input: fileHandle 
//Output:  
//Descriptor:  
EFI_STATUS CloseFile(EFI_FILE_PROTOCOL *fileHandle)
{
  EFI_STATUS  Status = 0;
  Status = fileHandle->Close(fileHandle);
   
  return Status;
}
//Name: ReadFile
//Input: fileHandle,bufSize,buffer
//Output:read data to the buffer and the length of data is bufSize
//Descriptor:
EFI_STATUS ReadFile(EFI_FILE_PROTOCOL *fileHandle,UINTN *bufSize,VOID *buffer)
{
  EFI_STATUS Status = 0;
 
  Status = fileHandle->Read(fileHandle, bufSize, buffer);

  return Status;
}

//Name: WriteFile
//Input: fileHandle,bufSize,buffer
//Output:write data to the file,data in buffer and the length of data is bufSize
//Descriptor:
EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *fileHandle,UINTN *bufSize,VOID *buffer)
{
  EFI_STATUS Status = 0;

  Status = fileHandle->Write(fileHandle, bufSize, buffer);

  return Status;
}
//Name: SetFilePosition
//Input: fileHandle,position
//Output: 
//Descriptor: if position is 0xffffffffffffffff,the current position will be set to the end of file.
EFI_STATUS SetFilePosition(EFI_FILE_PROTOCOL *fileHandle,UINT64 position)
{
  EFI_STATUS Status = 0;
  
  Status = fileHandle->SetPosition(fileHandle, position);

  return Status;
}
//Name: GetFilePosition
//Input: fileHandle,position
//Output: store in the var position
//Descriptor: if position is 0xffffffffffffffff,the current position will be set to the end of file.
EFI_STATUS GetFilePosition(EFI_FILE_PROTOCOL *fileHandle,UINT64 *position)
{
  EFI_STATUS Status = 0;
  
  Status = fileHandle->GetPosition(fileHandle, position);

  return Status;
}
