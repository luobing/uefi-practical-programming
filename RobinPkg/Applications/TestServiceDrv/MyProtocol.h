#ifndef MYPROTOCOL_H
#define MYPROTOCOL_H

#include <Uefi.h>

extern EFI_GUID gEfiMYSampleProtocolGUID;


//------------------------------------------- Declaration of protocols  ------------------------
typedef struct _EFI_MYSAMPLE_PROTOCOL EFI_MYSAMPLE_PROTOCOL;
/**   Sample of protocol, print the UserString

  @param  This       Indicates a pointer to the calling context.
  @param  UserString User's String

  @retval EFI_SUCCESS           The video is opened successfully. 
  @retval EFI_NOT_FOUND         There is no such file.
**/
typedef 
EFI_STATUS
(EFIAPI* EFI_MYSAMPLE_IN)(
        IN EFI_MYSAMPLE_PROTOCOL* This,
        IN CHAR16* UserString
        );

/**   Sample of protocol, print the UserString and do something

  @param  This       Indicates a pointer to the calling context.
  @param  UserString User's String

  @retval EFI_SUCCESS           The video is opened successfully. 
  @retval EFI_NOT_FOUND         There is no such file.
**/
typedef 
EFI_STATUS
(EFIAPI* EFI_MYSAMPLE_DOSTH)(
        IN EFI_MYSAMPLE_PROTOCOL* This,
        IN CHAR16* UserString
        );



/**   Sample of protocol,

  @param  This       Indicates a pointer to the calling context.
  @retval EFI_SUCCESS           The video is opened successfully. 
  @retval EFI_NOT_FOUND         There is no such file.
**/
typedef 
EFI_STATUS
(EFIAPI* EFI_MYSAMPLE_OUT)(
        IN EFI_MYSAMPLE_PROTOCOL* This
        );
/**  
 * **/
struct _EFI_MYSAMPLE_PROTOCOL{
 UINT64          Revision;
 EFI_MYSAMPLE_IN  MySample_In;
 EFI_MYSAMPLE_OUT MySample_Out;
 EFI_MYSAMPLE_DOSTH MySample_DoSth;
};

typedef struct {
    UINTN         Signature;
    EFI_MYSAMPLE_PROTOCOL  myProtocol; 
    UINT8         *myBuffer;
    UINT16        myWord;
    UINT32        myDword;
} MY_PRIVATE_DATA;

EFI_STATUS MySampleProtocolInit(VOID);
#endif