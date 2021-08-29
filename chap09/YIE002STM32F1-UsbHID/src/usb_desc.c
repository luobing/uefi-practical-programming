/**
 * 《UEFI编程实践》随书代码
 * 更多的开发探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Custom HID Demo
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
#include "usb_lib.h"
#include "usb_desc.h"

#define LOBYTE(x)  ((u8)(x & 0x00FF))
#define HIBYTE(x)  ((u8)((x & 0xFF00) >>8))

#define USBD_VID                     0x8765
#define USBD_PID                     0x4321
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    LOBYTE(USBD_VID),           /*idVendor (0x1234)*/
    HIBYTE(USBD_VID),
    LOBYTE(USBD_PID),           /*idProduct = 0x4321*/
    HIBYTE(USBD_PID),
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    1,                          /*Index of string descriptor describing
                                              manufacturer */
    2,                          /*Index of string descriptor describing
                                             product*/
    3,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* CustomHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    CUSTOMHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /* bNumInterfaces: 1 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xE0,         /* bmAttributes: Self powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    CUSTOMHID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x40,          /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x20,          /* bInterval: Polling Interval (32 ms) */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x01,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
    0x20,	/* bInterval: Polling Interval (20 ms) */
    /* 41 */
  }
  ; /* CustomHID_ConfigDescriptor */
const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] =
  {                    
//    0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */                       
//    0x09, 0x01,            /* USAGE (Demo Kit)               */    
//    0xa1, 0x01,            /* COLLECTION (Application)       */            
//    /* 6 */
//    
//    /* Led 1 */        
//    0x85, 0x01,            /*     REPORT_ID (1)		     */
//    0x09, 0x01,            /*     USAGE (LED 1)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x01,            /*     REPORT_ID (1)              */
//    0x09, 0x01,            /*     USAGE (LED 1)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 26 */
//    
//    /* Led 2 */
//    0x85, 0x02,            /*     REPORT_ID 2		     */
//    0x09, 0x02,            /*     USAGE (LED 2)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x02,            /*     REPORT_ID (2)              */
//    0x09, 0x02,            /*     USAGE (LED 2)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 46 */
//    
//    /* Led 3 */        
//    0x85, 0x03,            /*     REPORT_ID (3)		     */
//    0x09, 0x03,            /*     USAGE (LED 3)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x03,            /*     REPORT_ID (3)              */
//    0x09, 0x03,            /*     USAGE (LED 3)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 66 */
//    
//    /* Led 4 */
//    0x85, 0x04,            /*     REPORT_ID 4)		     */
//    0x09, 0x04,            /*     USAGE (LED 4)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x04,            /*     REPORT_ID (4)              */
//    0x09, 0x04,            /*     USAGE (LED 4)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 86 */
//    
//    /* key Push Button */  
//    0x85, 0x05,            /*     REPORT_ID (5)              */
//    0x09, 0x05,            /*     USAGE (Push Button)        */      
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */  
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
//    
//    0x09, 0x05,            /*     USAGE (Push Button)        */               
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */           
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
//         
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
//    0x85, 0x05,            /*     REPORT_ID (2)              */         
//                    
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */                      
//    /* 114 */

//    /* Tamper Push Button */  
//    0x85, 0x06,            /*     REPORT_ID (6)              */
//    0x09, 0x06,            /*     USAGE (Tamper Push Button) */      
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */  
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
//    
//    0x09, 0x06,            /*     USAGE (Tamper Push Button) */               
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */           
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
//         
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
//    0x85, 0x06,            /*     REPORT_ID (6)              */         
//                    
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */  
//    /* 142 */
//    
//    /* ADC IN */
//    0x85, 0x07,            /*     REPORT_ID (7)              */         
//    0x09, 0x07,            /*     USAGE (ADC IN)             */          
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */               
//    0x26, 0xff, 0x00,      /*     LOGICAL_MAXIMUM (255)      */                 
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */           
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */                    
//    0x85, 0x07,            /*     REPORT_ID (7)              */                 
//    0x09, 0x07,            /*     USAGE (ADC in)             */                     
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */                                 
//    /* 161 */

//    0xc0 	          /*     END_COLLECTION	             */
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)

 0x09, 0x00, // USAGE (0)
 
 //这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
 //该集合是一个应用集合。它的性质在前面由用途页和用途定义为
 //用户自定义。
 0xa1, 0x01, // COLLECTION (Application)

 //这是一个全局条目，说明逻辑值最小值为0。
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 
 //这是一个全局条目，说明逻辑值最大为255。
 0x25, 0xff, //     LOGICAL_MAXIMUM (255)
 
 //这是一个局部条目，说明用途的最小值为1。
 0x19, 0x01, //     USAGE_MINIMUM (1)
 
 //这是一个局部条目，说明用途的最大值16。
 0x29, 0x10, //     USAGE_MAXIMUM (16) 
 
 //这是一个全局条目，说明数据域的数量为16个。
 0x95, 0x10, //     REPORT_COUNT (16)
 
 //这是一个全局条目，说明每个数据域的长度为8bit，即1字节。
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //这是一个主条目，定义输入。
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //这是一个局部条目，说明用途的最小值为1。
 0x19, 0x01, //     USAGE_MINIMUM (1)
 
 //这是一个局部条目，说明用途的最大值16。
 0x29, 0x10, //     USAGE_MAXIMUM (16) 
 
 //这是一个主条目。定义输出数据（16字节，注意前面的全局条目）。
  0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0x19, 0x01, //     USAGE_MINIMUM (1)
	0x29, 0x10, //     USAGE_MAXIMUM (16)
	0xB1, 0x02, // Feature(Data, Variable, Absolute)
 
 //下面这个主条目用来关闭前面的集合。bSize为0，所以后面没数据。
 0xc0        // END_COLLECTION
  }; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
  {
    CUSTOMHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] =
  {
    CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "Robin"*/
		'R', 0, 'o', 0, 'b', 0, 'i', 0, 'n', 0
  };

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] =
  {
    CUSTOMHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'R', 0, 'o', 0, 'b', 0, 'i', 0, 'n', 0, '\'', 0, 's', 0,
    ' ', 0, 'U', 0, 'E', 0, 'F', 0, 'I', 0, ' ', 0, 'E', 0,
    'x', 0, 'p', 0, 'l', 0, 'o', 0, 'r', 0, 'e', 0, 'r', 0
  };
uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] =
  {
    CUSTOMHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'M', 0, 'y', 0, '1', 0,'2', 0, '3', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

