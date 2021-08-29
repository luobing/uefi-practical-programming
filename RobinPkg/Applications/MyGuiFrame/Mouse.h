#ifndef _MOUSE_H
#define _MOUSE_H	
#include "Common.h"


EFI_STATUS DisplayMouseMode(void);
VOID putMouseArrow(UINTN x,UINTN y);
VOID initMouseArrow(VOID);
EFI_STATUS GetMouseState(EFI_SIMPLE_POINTER_STATE *State);			//luobing 2013-5-21 7:58:27
EFI_STATUS CheckMouseEvent(VOID);		//luobing 2013-5-21 7:58:35
#endif