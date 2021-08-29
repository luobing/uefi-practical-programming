//Smbus.c: functions of acces smbus device
//Code by luobing 
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/

#include "mySmbus.h"

//*************************** Extern Variable *******************************************
extern EFI_SMBUS_HC_PROTOCOL *gSmbusHcProtocol;

//========================= Smbus function =========================================
//Function Name: ReadSmbusByte
//Input: SlaveAddress && RegisterIndex
//Output: Value
//Description: Read byte from specify smbus device
UINT8 ReadSmbusByte(IN UINT8 SlaveAddress,IN UINT8 RegisterIndex)
{
  EFI_STATUS    STATUS = EFI_SUCCESS;
  EFI_SMBUS_DEVICE_ADDRESS device;
  UINTN Length = 0x01;
  UINT8 buffer8;
  EFI_SMBUS_DEVICE_COMMAND index;
  device.SmbusDeviceAddress = (SlaveAddress >> 1);
  index=RegisterIndex;
  STATUS = gSmbusHcProtocol->Execute( gSmbusHcProtocol,
      device,
      index,
      EfiSmbusReadByte,
      FALSE,
      &Length,
      &buffer8
      );
  return buffer8;
}
//Function Name:  WriteSmbusByte
//Input: SlaveAddress && RegisterIndex && Value
//Output: Void
//Description: Write byte to specify smbus device
VOID WriteSmbusByte(IN UINT8 SlaveAddress,IN UINT8 RegisterIndex,IN UINT8 Value)
{
  EFI_STATUS    STATUS = EFI_SUCCESS;
  EFI_SMBUS_DEVICE_ADDRESS device;
  UINTN Length = 0x01;
  UINT8 buffer8;
  EFI_SMBUS_DEVICE_COMMAND index;
  device.SmbusDeviceAddress = (SlaveAddress >> 1);
  index=RegisterIndex;
  buffer8=Value;
  STATUS = gSmbusHcProtocol->Execute( gSmbusHcProtocol,
      device,
      index,
      EfiSmbusWriteByte,
      FALSE,
      &Length,
      &buffer8
      );
  return;
}