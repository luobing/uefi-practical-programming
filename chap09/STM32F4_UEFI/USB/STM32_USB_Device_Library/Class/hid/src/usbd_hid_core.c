/**
  ******************************************************************************
  * @file    usbd_hid_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                HID Class  Description
  *          =================================================================== 
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick)
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usart.h" 

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */
uint8_t USB_Rx_Buffer[0xFF];

static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req);

static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length);

static uint8_t  USBD_HID_DataIn (void  *pdev, uint8_t epnum);

static uint8_t  USBD_HID_DataOut (void  *pdev, uint8_t epnum);
/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Variables
  * @{
  */ 

USBD_Class_cb_TypeDef  USBD_HID_cb = 
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  USBD_HID_DataOut, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_HID_GetCfgDesc,
#ifdef USB_OTG_HS_CORE  
  USBD_HID_GetCfgDesc, /* use same config as per FS */
#endif  
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */        
__ALIGN_BEGIN static uint32_t  USBD_HID_AltSet  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */      
__ALIGN_BEGIN static uint32_t  USBD_HID_Protocol  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
__ALIGN_BEGIN static uint32_t  USBD_HID_IdleState __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */ 
//USB报告描述符
__ALIGN_BEGIN static uint8_t HID_ReportDesc[] __ALIGN_END =
{

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
 
 //这是一个局部条目，说明用途的最大值64。
 0x29, 0x10, //     USAGE_MAXIMUM (16) 
 
 //这是一个全局条目，说明数据域的数量为16个。
 0x95, 0x10, //     REPORT_COUNT (16)
 
 //这是一个全局条目，说明每个数据域的长度为8bit，即1字节。
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //这是一个主条目，说明有8个长度为8bit的数据域做为输入。
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //这是一个局部条目，说明用途的最小值为1。
 0x19, 0x01, //     USAGE_MINIMUM (1)
 
 //这是一个局部条目，说明用途的最大值64。
 0x29, 0x10, //     USAGE_MAXIMUM (16) 
 
 //这是一个主条目。定义输出数据（16字节，注意前面的全局条目）。
 0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0x19, 0x01, //     USAGE_MINIMUM (1)
	0x29, 0x10, //     USAGE_MAXIMUM (16)
	0xB1, 0x02, // Feature(Data, Variable, Absolute)
 
 //下面这个主条目用来关闭前面的集合。bSize为0，所以后面没数据。
 0xc0        // END_COLLECTION
//	0x05, 0x01,                              // Usage Page
//	0x09, 0x06,                              // Usage
//	0xA1, 0x01,                              // Collection
//	0x05, 0x07,                              // Usage Page
//	0x19, 0xE0,                              // Usage Minimum
//	0x29, 0xE7,                              // Usage Maximum
//	0x15, 0x00,                              // Logical Minimum
//	0x25, 0x01,                              // Logical Maximum
//	0x75, 0x01,                              // Report Size
//	0x95, 0x08,                              // Report Count
//	0x81, 0x02,                              // Input
//	0x95, 0x01,                              // Report Count
//	0x75, 0x08,                              // Report Size
//	0x81, 0x01,                              // Input
//	0x95, 0x05,                              // Report Count
//	0x75, 0x01,                              // Report Size
//	0x05, 0x08,                              // Usage Page
//	0x19, 0x01,                              // Usage Minimum
//	0x29, 0x05,                              // Usage Maximum
//	0x91, 0x02,                              // Output
//	0x95, 0x01,                              // Report Count
//	0x75, 0x03,                              // Report Size
//	0x91, 0x01,                              // Output
//	0x95, 0x06,                              // Report Count
//	0x75, 0x08,                              // Report Size
//	0x15, 0x00,                              // Logical Minimum
//	0x25, 0x65,                              // Logical Maximum
//	0x05, 0x07,                              // Usage Page
//	0x19, 0x00,                              // Usage Minimum
//	0x29, 0x65,                              // Usage Maximum
//	0x81, 0x00,                              // Input
//  //lbdebug begin   
//	0x06, 0xA1, 0xFF,                       // Usage Page (Vendor defined)
//	0x09, 0xA2,                             // Usage (Vendor defined)
//	0x15, 0x81,                             // Logical Minimum (-128)
//	0x25, 0x7F,                             // Logical Maximum (127)
//	0x75, 0x08,                             // Report Size (8) (bits)
//	0x95, 0x04,                             // Report Count(4) (fields)
//	0xB1, 0x02,                             // Feature(Data, Variable, Absolute)
//	//lbdebug end
//	0xC0                                    // End Collection
};
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  sizeof(USBD_HID_CfgDesc),
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0x00,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x10,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,//0x21,  @mfm       /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  sizeof(HID_ReportDesc),//HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of  endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 255 Byte max */
  0x00,
  0x0A,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_OUT_EP,     /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET, /*wMaxPacketSize: 255 Byte max */
  0x00,
  0x0A,          /*bInterval: Polling Interval (10 ms)*/
  /* 41 */
} ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
};
#endif 


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
//#if 0
//__ALIGN_BEGIN static uint8_t HID_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
//{
//	  
//    0x05, 0x8c,            /* USAGE_PAGE (ST Page)           */                   
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
//		
//		
//}; 
//#endif
/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx)
{
  
  /* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP,
              HID_IN_PACKET,
              USB_OTG_EP_INT);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP,
              HID_OUT_PACKET,
              USB_OTG_EP_INT);
  
  /* Prepare Out endpoint to receive next packet */
  DCD_EP_PrepareRx(pdev,
                   HID_OUT_EP,
                   (uint8_t*)(USB_Rx_Buffer),
                   HID_OUT_PACKET);	
	
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  DCD_EP_Close (pdev , HID_IN_EP);
  DCD_EP_Close (pdev , HID_OUT_EP);
  
  
  return USBD_OK;
}

//luobing 2020/12/08 
uint8_t myInOutReport[16];
/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req)
{
  uint8_t USBD_HID_Report_LENGTH = 0; //luobing add
	uint16_t USBD_HID_Report_ID; //luobing add
	uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  uint16_t i;
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
      
    case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;
      
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_Protocol,
                        1);    
      break;
      
    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_IdleState,
                        1);        
      break;      
		//luobing: add for host fucntion: HidD_GetInputReport and HidD_SetOutputReport
    case HID_REQ_SET_REPORT:
       /* HidD_SetOutputReport 1-byte = Report ID (IN) */
//       flag = 1;
       USBD_HID_Report_ID = (uint8_t)(req->wValue);
       USBD_HID_Report_LENGTH = (uint8_t)(req->wLength);
       USBD_CtlPrepareRx (pdev, myInOutReport, USBD_HID_Report_LENGTH); 
       break;  
   
     case HID_REQ_GET_REPORT:
       /* HidD_GetInputReport 1-byte = Report ID (OUT) */
//       flag = 1;
			for(i=0;i<16;i++)
				myInOutReport[i]=(0x10+i);
			
       USBD_CtlSendData (pdev, (uint8_t *)&myInOutReport, 16); // to pc
       break; 
		 
		 
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)  //robin: return report desc
      {
        len = MIN(HID_REPORT_DESC_SIZE , req->wLength);
        pbuf = HID_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = USBD_HID_CfgDesc + 0x12;
#endif 
        len = MIN(USB_HID_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport 
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport     (USB_OTG_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  if (pdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    DCD_EP_Tx (pdev, HID_IN_EP, report, len);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgDesc);
  return USBD_HID_CfgDesc;
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_DataIn (void  *pdev, 
                              uint8_t epnum)
{
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  DCD_EP_Flush(pdev, HID_IN_EP);
  return USBD_OK;
}


static uint8_t  USBD_HID_DataOut (void  *pdev, uint8_t epnum)
{
  uint16_t USB_Rx_Cnt;
//	uint8_t i;
  
	if ( epnum != (HID_OUT_EP & 0x0F) )
    return USBD_FAIL;

  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;
	
  printf("USB_Rx_Cnt = %d\r\n",USB_Rx_Cnt);
		//printf("USB_Rx_Buffer[%d] = %d\r\n",i,USB_Rx_Buffer[i]);
	USBD_HID_SendReport((USB_OTG_CORE_HANDLE*)pdev, USB_Rx_Buffer, USB_Rx_Cnt);
  
  /* Prepare Out endpoint to receive next packet */
  DCD_EP_PrepareRx(pdev,
                   HID_OUT_EP,
                   (uint8_t*)(USB_Rx_Buffer),
                   HID_OUT_PACKET);

  return USBD_OK;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
