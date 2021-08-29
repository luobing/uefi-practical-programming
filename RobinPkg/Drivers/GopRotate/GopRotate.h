/* GopRotate.h
Copyright (c) 2015, Aaron Pop
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/ComponentName.h>
#include <Protocol/BlockIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DriverBinding.h>
#include <Library/DebugLib.h>

#include <Protocol/GraphicsOutput.h>
#include <Library/PcdLib.h>
//
// Libraries
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
// LAB FAUX
#include <Library/UefiRuntimeServicesTableLib.h>
// LAB FAUX
//
// UEFI Driver Model Protocols
//
#include <Protocol/DriverBinding.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>
#include <Protocol/HiiConfigAccess.h>

#define GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL_GUID \
    { 0xda62085c, 0x96e4, 0x4345, { 0xa3, 0xb0, 0x98, 0x89, 0xda, 0xf3, 0x16, 0xb7 } }

typedef enum 
{
    Rotate0 = 0,
    Rotate90 = 1,
    Rotate180 = 2,
    Rotate270 = 3,
    RotateMax = 4
} ROTATE_SCREEN;

typedef struct _GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *GRAPHICS_OUTPUT_PROTOCOL_ROTATE_GET_ROTATION)(
  IN  GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL  *This,
  IN  ROTATE_SCREEN                             *Rotation
  );

typedef
EFI_STATUS
(EFIAPI *GRAPHICS_OUTPUT_PROTOCOL_ROTATE_SET_ROTATION)(
  IN  GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL  *This,
  IN  ROTATE_SCREEN                             Rotation
  );

struct _GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL
{
    GRAPHICS_OUTPUT_PROTOCOL_ROTATE_GET_ROTATION GetRotation;
    GRAPHICS_OUTPUT_PROTOCOL_ROTATE_SET_ROTATION SetRotation;
};


extern EFI_GUID gGraphicsOutputProtocolRotateProtocolGuid;

#define GRAPHICS_OUTPUT_ROTATE_DEV_SIGNATURE  SIGNATURE_32('g', 'o', 'r', 'p')  
typedef struct 
{
    UINTN                                       Signature;  ///< Signature of the Private structure
    EFI_HANDLE                                  Handle;     ///< Handle of the device being managed
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT            Blt;        ///< Pointer to the original Blt function
    EFI_GRAPHICS_OUTPUT_PROTOCOL                *Gop;       ///< Pointer to the Graphics Output Protocol
    GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL    GopRotate;  ///< Pointer to the Gop Rotate Protocol
    ROTATE_SCREEN                               Rotation;   ///< Internal tracking of the current screen rotation
} GRAPHICS_OUTPUT_ROTATE_PRIVATE;

#define GRAPHICS_OUTPUT_ROTATE_PRIVATE_FROM_GOP_ROTATE(a)\
  CR(a, GRAPHICS_OUTPUT_ROTATE_PRIVATE, GopRotate, GRAPHICS_OUTPUT_ROTATE_DEV_SIGNATURE)


EFI_STATUS EFIAPI GopRotateGetRotation(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *This, ROTATE_SCREEN *Rotation);
EFI_STATUS EFIAPI GopRotateSetRotation(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *This, ROTATE_SCREEN Rotation);


EFI_STATUS EFIAPI BltRotate 
(
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *BltBuffer,     OPTIONAL
    IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION   BltOperation,
    IN  UINTN                               SourceX,
    IN  UINTN                               SourceY,
    IN  UINTN                               DestinationX,
    IN  UINTN                               DestinationY,
    IN  UINTN                               Width,
    IN  UINTN                               Height,
    IN  UINTN                               Delta           OPTIONAL
);

typedef struct 
{
    LIST_ENTRY Link;  
    GRAPHICS_OUTPUT_ROTATE_PRIVATE *Private;
} GOP_DEVICE_LIST;


GRAPHICS_OUTPUT_ROTATE_PRIVATE *GetPrivateFromGopRotate(GRAPHICS_OUTPUT_PROTOCOL_ROTATE_PROTOCOL *GopRotate);

