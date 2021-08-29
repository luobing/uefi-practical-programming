/**
 * 使用F4探索者制作的 USB HID设备，供学习使用,可作为《UEFI编程实践》第9章的实验设备。
 * 更多的开发探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "sys.h"
#include "delay.h"  
#include "usart.h"    
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 
 
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况
u8 Divece_STA = 0;
int main(void)
{        
	
	uint8_t  buf[20] = {0,1,2,3,4,5,6,7,8};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200

	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_HID_cb,&USR_cb);
	delay_ms(1800);	
	printf("USBD_Init Finish\r\n");
  while(1)
  {
//		if(Divece_STA!=bDeviceState) 
//		{
//			if(bDeviceState==1){
//				printf("USB Connected\r\n");//USB已连接
//			}
//			else 
//				printf("USB DisConnected\r\n");//USB已断开
//			Divece_STA=bDeviceState;
//		}
//		delay_ms(10);
//		if(Divece_STA==1)
//		{
//			//buf[0]++;
//			printf("USB HID Send %d\r\n",buf[0]);
//			USBD_HID_SendReport(&USB_OTG_dev, buf, 16);
//		}
//		delay_ms(1000);
  }
}




