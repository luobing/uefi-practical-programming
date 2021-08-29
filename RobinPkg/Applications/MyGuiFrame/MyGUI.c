/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "MyGUI.h"
#include "Pictures.h"
#include "Graphic.h"
#include "Window.h"
#include "Font.h"
#include "Mouse.h"

extern UINT16 mouse_xres;		//鼠标全局位置，只有鼠标显示函数可以使用
extern UINT16 mouse_yres;
extern UINT16 mouse_xScale,mouse_yScale;
extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gConInEx;
extern EFI_SIMPLE_POINTER_PROTOCOL *gMouse;   //Mouse

extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
//Global variable
EFI_EVENT gTimerEvent;
EFI_EVENT gWaitArray[3];

//Name: InitGUI
//Input: None
//Output: None
//Description: Init all event
VOID InitGUI(VOID)
{
    EFI_STATUS Status;

    //1 Create Timer and TimerNotifyFunc
    // Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, 
    //                             TPL_CALLBACK,
    //                             (EFI_EVENT_NOTIFY)TimerNotifyFunc,
    //                             (VOID*)NULL,
    //                             &gTimerEvent
    //                             );
    //1 Create Timer
    Status = gBS->CreateEvent(EVT_TIMER,TPL_APPLICATION,
                                (EFI_EVENT_NOTIFY)NULL,
                                (VOID*)NULL,
                                &gTimerEvent
                                );
    Status = gBS->SetTimer(gTimerEvent,TimerPeriodic,10*1000*1000);  //1s 

    //2 init event group
    gWaitArray[EVENT_TIMER]=gTimerEvent;
    // gWaitArray[EVENT_KEY]=gConInEx->WaitForKeyEx;
    gWaitArray[EVENT_KEY]=gST->ConIn->WaitForKey;
    gWaitArray[EVENT_MOUSE]=gMouse->WaitForInput;
    
    //3 init mouse...
    initMouseArrow();

}
//Name: HanlderGUI
//Input: None
//Output: None
//Description: main loop
VOID HanlderGUI(VOID)
{
    EFI_STATUS Status;
    UINTN Index;
    // EFI_KEY_DATA keyEx={0,0};
    EFI_INPUT_KEY key={0,0};
    EFI_SIMPLE_POINTER_STATE mouseState;

    while(1)
    {
        Status = gBS->WaitForEvent(3, gWaitArray, &Index);
        // Print(L"Status =%r\n",Status);
        if(Index == EVENT_KEY)  //处理键盘事件
        {
            
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn,&key);
            // Status = gConInEx->ReadKeyStrokeEx(gConInEx,&keyEx);
            // key.ScanCode=keyEx.Key.ScanCode;
            // key.UnicodeChar=keyEx.Key.UnicodeChar;
            HandlerKeyboard(&key);
        }
        else if(Index == EVENT_MOUSE) //处理鼠标事件
        {
            GetMouseState(&mouseState);
            HandlerMouse(&mouseState);
        }
        else if(Index == EVENT_TIMER) //处理定时器事件
        {
            HandlerTimer();
        }
        else  //意外错误
        {
            //do nothing
        }
    }
    
}
//Name: HandlerKeyboard
//Input: key
//Output: EFI_SUCCESS
// EFI_STATUS HandlerKeyboard(EFI_KEY_DATA *key)
EFI_STATUS HandlerKeyboard(EFI_INPUT_KEY *key)
{
  // if((key->KeyState.KeyToggleState&EFI_CAPS_LOCK_ACTIVE) == EFI_CAPS_LOCK_ACTIVE)
  UINT8 *s_text = "Please Input:";  
  //1 (100,100)处显示按键信息
  draw_string(s_text, 100, 100, &MyFontArray, &(gColorTable[WHITE]));
  rectblock(240,100,270,130,&(gColorTable[DEEPBLUE]));
  draw_single_char((UINT32)key->UnicodeChar,240,100,&MyFontArray, &(gColorTable[RED]));
  
  return EFI_SUCCESS;
        
}
//Name: HandlerTimer
//Input:  
//Output: EFI_SUCCESS
EFI_STATUS HandlerTimer(VOID)
{
  static UINT8 flag=0;
  UINT8 *s_text = "Timer Event has triggered.";  
  if(flag==1)
  {
    flag=0;
    draw_string(s_text, 100, 150, &MyFontArray, &(gColorTable[WHITE]));
  }
  else
  {
    flag=1;
    rectblock(100,150,400,180,&(gColorTable[DEEPBLUE]));
  }
  
  return EFI_SUCCESS;
}
//Name: HandlerMouse
//Input:  
//Output: EFI_SUCCESS
EFI_STATUS HandlerMouse(EFI_SIMPLE_POINTER_STATE *State)
{
    INT32 i,j;
    // Print(L"lbdebug: mouse handler\n");
    i=(INT32)mouse_xres;
    j=(INT32)mouse_yres;
    i += ((State->RelativeMovementX<<MOUSE_SPEED) >> mouse_xScale);
    if (i < 0)
      i = 0;
    if (i > SY_SCREEN_WIDTH - 1)
      i = SY_SCREEN_WIDTH - 1;
    j += ((State->RelativeMovementY<<MOUSE_SPEED) >> mouse_yScale);
    if (j < 0)
      j = 0;
    if (j > SY_SCREEN_HEIGHT - 1)
      j = SY_SCREEN_HEIGHT - 1;
        
    putMouseArrow(i, j);
}
//Name: TimerNotifyFunc
//Input: Event, *Context
//Output: None
VOID TimerNotifyFunc(IN EFI_EVENT Event, IN VOID *Context)
{
    //do something
}