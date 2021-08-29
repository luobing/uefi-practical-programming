//code by luobing 
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/

#include "PciRW.h"

extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;


//===========================================PCI function begin======================================
/**
  This routine is used to check whether the pci device is present.

  @param PciRootBridgeIo   Pointer to instance of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param Pci               Output buffer for PCI device configuration space.
  @param Bus               PCI bus NO.
  @param Device            PCI device NO.
  @param Func              PCI Func NO.

  @retval EFI_NOT_FOUND    PCI device not present.
  @retval EFI_SUCCESS      PCI device is found.

**/
EFI_STATUS PciDevicePresent (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  OUT PCI_TYPE00                          *Pci,
  IN  UINT8                               Bus,
  IN  UINT8                               Device,
  IN  UINT8                               Func
  )
{
  UINT64      Address;
  EFI_STATUS  Status;
  
  // Create PCI address map in terms of Bus, Device and Func
  Address = EFI_PCI_ADDRESS (Bus, Device, Func, 0);
  // Read the Vendor ID register
  Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint32,
                                  Address,
                                  1,
                                  Pci
                                  );

  if (!EFI_ERROR (Status) && (Pci->Hdr).VendorId != 0xffff) {
    // Read the entire config header for the device  
    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint32,
                                    Address,
                                    sizeof (PCI_TYPE00) / sizeof (UINT32),
                                    Pci
                                    );

    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}
//Name GetSpecifyPciDevice
//Input:
//Output:
EFI_STATUS GetSpecifyPciDevice(
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  OUT PCI_TYPE00                          *Pci,
  IN	UINT16 VendorID,
  IN	UINT16 DeviceID
  )
{
	UINT8 Bus,Dev,Func;
	EFI_STATUS                         Status=EFI_NOT_FOUND;
	BOOLEAN myFlag=TRUE;
	//Find specify device
	for(Bus=0;Bus<=10;Bus++)		//应该没有这么多总线吧
		for(Dev=0;Dev<=PCI_MAX_DEVICE;Dev++)
			for(Func=0;Func<=PCI_MAX_FUNC;Func++)
			{
				if(myFlag)
				{
					Status = PciDevicePresent(gPCIRootBridgeIO,Pci,Bus,Dev,Func);
					if (Status == EFI_SUCCESS)		//Find device
					{
//						Print(L"mes:   %x    %x    %x",Bus,Dev,Func);
//						Print(L"       %x       %x        %x\n",(Pci->Hdr).VendorId,(Pci->Hdr).DeviceId,(Pci->Hdr).ClassCode[0]);
						if(((Pci->Hdr).VendorId == VendorID) && ((Pci->Hdr).DeviceId == DeviceID))
						{
							Status=EFI_SUCCESS;
							myFlag=FALSE;
//							Print(L"GetSpecifyPciDevice OK,bus=%x,dev=%x,func=%d\n",Bus,Dev,Func);
							break;
						}
					}
				}
			}
	
	return Status;
}
//===========================================PCI function end======================================
