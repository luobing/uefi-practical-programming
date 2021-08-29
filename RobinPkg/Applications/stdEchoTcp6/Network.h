/** @file
  function definitions for internal to Network functions.
	code by luobing 2020-03-29 20:37:57
**/

#ifndef _NETWORK_H
#define _NETWORK_H
//所有包含的目录到mdepkg下去找
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#include <Protocol/Tcp4.h>
#include <Protocol/ServiceBinding.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>



#define MYIPV4(a,b,c,d) (a | b<<8 | c << 16 | d <<24)


VOID NopNoify(IN EFI_EVENT Event, IN VOID *Context);
EFI_STATUS InitTcp4SocketFd(INTN index);
UINTN CreateTCP4Socket(VOID);
EFI_STATUS ConfigTCP4Socket(UINTN index, UINT32 Ip32, UINT16 Port);
EFI_STATUS SendTCP4Socket(UINTN index, CHAR8 *Data, UINTN Lenth);
EFI_STATUS RecvTCP4Socket(IN UINTN index, IN CHAR8 *Buffer, IN UINTN Length, OUT UINTN *recvLength);
EFI_STATUS ConnectTCP4Socket(UINTN index, UINT32 Ip32, UINT16 Port);
INTN DestroyTCP4Socket(UINTN index);
EFI_STATUS CloseTCP4Socket(UINTN index);

#endif