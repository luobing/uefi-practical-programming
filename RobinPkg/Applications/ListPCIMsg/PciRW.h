//code by luobing  

#ifndef _PCIRW_H
#define _PCIRW_H

#include "Common.h"
EFI_STATUS PciDevicePresent (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  OUT PCI_TYPE00                          *Pci,
  IN  UINT8                               Bus,
  IN  UINT8                               Device,
  IN  UINT8                               Func
  );
EFI_STATUS GetSpecifyPciDevice(
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  OUT PCI_TYPE00                          *Pci,
  IN	UINT16 VendorID,
  IN	UINT16 DeviceID
  );
  



#endif