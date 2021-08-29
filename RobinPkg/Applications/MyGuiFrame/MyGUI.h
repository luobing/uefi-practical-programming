#ifndef _MY_GUI_H
#define _MY_GUI_H	
#include "Common.h"

#define MOUSE_SPEED 3   //指针移动速度

enum EVENT_ID {
	EVENT_TIMER = 0,
	EVENT_KEY,
	EVENT_MOUSE
	
};
VOID InitGUI(VOID);
VOID TimerNotifyFunc(IN EFI_EVENT Event, IN VOID *Context);
VOID HanlderGUI(VOID);
// EFI_STATUS HandlerKeyboard(EFI_KEY_DATA *key);
EFI_STATUS HandlerKeyboard(EFI_INPUT_KEY *key);
EFI_STATUS HandlerMouse(EFI_SIMPLE_POINTER_STATE *State);
EFI_STATUS HandlerTimer(VOID);







#endif



