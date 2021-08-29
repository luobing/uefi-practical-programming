//code by luobing 2013-3-19 10:54:39

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "Common.h"


EFI_STATUS GetKeyEx(UINT16 *ScanCode, UINT16 *UniChar, UINT32 *ShiftState, EFI_KEY_TOGGLE_STATE * ToggleState);
EFI_STATUS GetKey(EFI_INPUT_KEY *key);
VOID FlushKeyBuffer(VOID);
VOID WaitKey(VOID);

#endif