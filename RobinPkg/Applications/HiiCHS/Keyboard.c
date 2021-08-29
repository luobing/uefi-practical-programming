//Member functions of keyboard
//code by luobing 2019-3-19 10:52:38
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Keyboard.h"

extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;

//===========================================Keyboard function begin====================================
//Name:  GetKeyEx
//Input:
//Output:
//Descriptor: 
EFI_STATUS GetKeyEx(UINT16 *ScanCode, UINT16 *UniChar, UINT32 *ShiftState, EFI_KEY_TOGGLE_STATE * ToggleState)
{
	EFI_STATUS                        Status;
  EFI_KEY_DATA                      KeyData;
 	UINTN															Index;
	  
	gBS->WaitForEvent(1,&(gSimpleTextInputEx->WaitForKeyEx),&Index);
	Status = gSimpleTextInputEx->ReadKeyStrokeEx(gSimpleTextInputEx,&KeyData);
	if(!EFI_ERROR(Status))
	{
 		*ScanCode=KeyData.Key.ScanCode;
		*UniChar=KeyData.Key.UnicodeChar;
		*ShiftState=KeyData.KeyState.KeyShiftState;
		*ToggleState=KeyData.KeyState.KeyToggleState;
		return EFI_SUCCESS;
	} 
	return Status;
}
//Function Name: FlushKeyBuffer
//Input: none
//Output: none
//Description: Flush all keyboard buffer
VOID FlushKeyBuffer(VOID)
{
  EFI_INPUT_KEY     Key;
 
  // flush keyboard buffer
  while ( gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS );
}
//Function Name: WaitKey
//Input: none
//Output: none
//Description: Wait any key press
VOID WaitKey(VOID)
{
  EFI_INPUT_KEY     Key;
  UINTN Index;

  // flush keyboard buffer
  while ( gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS );
  gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
  return;
}
//Function Name: GetKey
//Input: none
//Output: none
//Description: Get key code,return unicode code and scan code(such as F1...)
EFI_STATUS GetKey(EFI_INPUT_KEY *key)
{
	UINTN Index;
	gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
  return gST->ConIn->ReadKeyStroke(gST->ConIn,key);  
}