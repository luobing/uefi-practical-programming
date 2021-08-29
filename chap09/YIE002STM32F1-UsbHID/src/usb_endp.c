/**
 * 《UEFI编程实践》随书代码
 * 更多的开发探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//uint8_t Receive_Buffer[2];
uint8_t Receive_Buffer[0xff];
extern __IO uint8_t PrevXferComplete;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
//  BitAction Led_State;
	uint32_t DataLength = 0;
  /* Read received data (2 bytes) */  
  DataLength=USB_SIL_Read(EP1_OUT, Receive_Buffer); //读取端点得到的数据
  SetEPRxStatus(ENDP1, EP_RX_VALID);
	
	if (Receive_Buffer[0] == 0xA0)   //将第二个字节改为1返回，表示是采用端点发送的方式
	{
		Receive_Buffer[1]=0x1;
	}
	USB_SIL_Write(EP1_IN,Receive_Buffer,DataLength);
	SetEPTxStatus(ENDP1,EP_TX_VALID);
//  if (Receive_Buffer[1] == 0)
//  {
//    Led_State = Bit_RESET;
//  }
//  else 
//  {
//    Led_State = Bit_SET;
//  }
// 
// 
//  switch (Receive_Buffer[0])
//  {
//    case 1: /* Led 1 */
//     if (Led_State != Bit_RESET)
//     {
//       STM_EVAL_LEDOn(LED1);
//     }
//     else
//     {
//       STM_EVAL_LEDOff(LED1);
//     }
//     break;
//    case 2: /* Led 2 */
//     if (Led_State != Bit_RESET)
//     {
//       STM_EVAL_LEDOn(LED2);
//     }
//     else
//     {
//       STM_EVAL_LEDOff(LED2);
//     }
//      break;
//    case 3: /* Led 3 */
//     if (Led_State != Bit_RESET)
//     {
//       STM_EVAL_LEDOn(LED3);
//     }
//     else
//     {
//       STM_EVAL_LEDOff(LED3);
//     }
//      break;
//    case 4: /* Led 4 */
//     if (Led_State != Bit_RESET)
//     {
//       STM_EVAL_LEDOn(LED4);
//     }
//     else
//     {
//       STM_EVAL_LEDOff(LED4);
//     }
//      break;
//  default:
//    STM_EVAL_LEDOff(LED1);
//    STM_EVAL_LEDOff(LED2);
//    STM_EVAL_LEDOff(LED3);
//    STM_EVAL_LEDOff(LED4); 
//    break;
//  }
 
  
 
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  PrevXferComplete = 1;
//	if (Receive_Buffer[0] == 0x31)
//	{
//		Receive_Buffer[1]='R';
//	}
////	USB_SIL_Read(EP1_OUT, Receive_Buffer); //读取端点得到的数据
//	USB_SIL_Write(EP1_IN,Receive_Buffer,16);
//	SetEPTxStatus(ENDP1,EP_TX_VALID);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

