//code by luobing 2013-3-19 11:11:43

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
  
EFI_STATUS ByteReadPCI(IN UINT64 baseaddress,IN UINT16 index,OUT UINT8 *value);
EFI_STATUS ByteWritePCI(IN UINT64 baseaddress,IN UINT16 index,IN UINT8 value);


#endif