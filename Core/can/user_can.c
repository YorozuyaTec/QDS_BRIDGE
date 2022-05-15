/*
 * user_can.c
 *
 *  Created on: 2021年11月8日
 *      Author: Administrator
 */



#include "user_can.h"

CAN_TxHeaderTypeDef TXHeader;
CAN_RxHeaderTypeDef RXHeader;

FireControllerTypeDef fire_controller =
{
	.head = {0x47,0x16,0x1,0xA6,0x05},
};

uint8_t irq_flag=0;
float adc1,adc2,adc3;

uint8_t TXmessage[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t RXmessage[8];
uint32_t pTxMailbox = 0;
uint8_t buffer[1024];

/* USER CODE BEGIN 1 */
  void CAN1_Config(void)
{
	CAN_FilterTypeDef CAN_FilterType;
	CAN_FilterType.FilterBank=0;
	CAN_FilterType.FilterIdHigh=0x0000;
	CAN_FilterType.FilterIdLow=0x0000;
	CAN_FilterType.FilterMaskIdHigh=0x0000;
	CAN_FilterType.FilterMaskIdLow=0x0000;
	CAN_FilterType.FilterFIFOAssignment=CAN_RX_FIFO0;
	CAN_FilterType.FilterMode=CAN_FILTERMODE_IDMASK;
	CAN_FilterType.FilterScale=CAN_FILTERSCALE_32BIT;
	CAN_FilterType.FilterActivation=ENABLE;
	CAN_FilterType.SlaveStartFilterBank=14;
	if(HAL_CAN_ConfigFilter(&hcan,&CAN_FilterType)!=HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_Start(&hcan)!=HAL_OK)
	{
		Error_Handler();
	}
}


  /***********************************************
  函数功能：can发送数据
  入口参数：
  			ide：	0：标准帧
  					1：扩展帧
  			id：	帧ID
  			len：	数据长度
  			data：	数据
  返回值：0：成功。1：失败
  ************************************************/
  uint8_t Can_TxMessage(uint8_t ide,uint32_t id,uint8_t len,uint8_t *data)
  {
  	uint32_t   TxMailbox;
  	CAN_TxHeaderTypeDef CAN_TxHeader;
  	HAL_StatusTypeDef   HAL_RetVal;
  	uint16_t i=0;
  	if(ide == 0)
  	{
  		CAN_TxHeader.IDE = CAN_ID_STD;	//标准帧
  		CAN_TxHeader.StdId = id;
  	}
  	else
  	{
  		CAN_TxHeader.IDE = CAN_ID_EXT;			//扩展帧
  		CAN_TxHeader.ExtId = id;
  	}
  	CAN_TxHeader.DLC = len;
  	CAN_TxHeader.RTR = CAN_RTR_DATA;//数据帧,CAN_RTR_REMOTE遥控帧
  	CAN_TxHeader.TransmitGlobalTime = DISABLE;
  	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
  	{
  		i++;
  		if(i>0xfffe)
  			return 1;
  	}
  	HAL_Delay(500);
  	HAL_RetVal = HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,data,&TxMailbox);
  	if(HAL_RetVal != HAL_OK)
  		return 1;
  	return 0;
  }



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)//接受邮箱0挂起中断回调函数
{
	//static uint16_t cnt=0;
	if(hcan->Instance==CAN1)
	{
		HAL_CAN_GetRxMessage(hcan,CAN_FILTER_FIFO0,&RXHeader,RXmessage);//获取数据

		bat_can_data_handler(RXHeader.ExtId,RXmessage);
		//uart_module_send_hex(RXmessage, 8);
//		memcpy(buffer+cnt,RXmessage,8);
//		cnt+=8;
//		if(cnt>128)
//		{
//			cnt = 0;
//			//uart_module_send_hex(buffer, 128);
//
//		}
		//HAL_UART_Transmit(&huart2, RXmessage, 8, 1000);
//		if(RXHeader.ExtId == 0x14007FFF && RXHeader.RTR == 2)  //远程帧
//		{
			//CAN_senddata(hcan);
			//CAN_Send_Pack(test, sizeof(test));
//			irq_flag =1;
//			 HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
//
//		}


    }

}

void CAN_senddata(CAN_HandleTypeDef *hcan)
{
     TXHeader.StdId=0x00000012;
	 TXHeader.ExtId=0x18FC0908;
	 TXHeader.DLC=8;
	 TXHeader.IDE=CAN_ID_EXT;
	 TXHeader.RTR=CAN_RTR_DATA;
	 TXHeader.TransmitGlobalTime = DISABLE;
	 HAL_CAN_AddTxMessage(hcan,&TXHeader,TXmessage,&pTxMailbox);
}

uint8_t user_can_send_data(uint32_t id , uint8_t *data)
{
	HAL_StatusTypeDef status;
     TXHeader.StdId=0x00000000;
	 TXHeader.ExtId=id;
	 TXHeader.DLC=8;
	 TXHeader.IDE=CAN_ID_EXT;
	 TXHeader.RTR=CAN_RTR_DATA;
	 TXHeader.TransmitGlobalTime = DISABLE;
	 status =  HAL_CAN_AddTxMessage(&hcan,&TXHeader,data,&pTxMailbox);
	 return status;
}

void CAN_Send_Pack(uint8_t *data,uint16_t lenth)
{
	uint8_t pack_id=0;
	//uint8_t data_buffer[8];
	uint32_t id_index[7]={0x140B7FFF,0x142B7FFF,0x144B7FFF,0x146B7FFF,0x148B7FFF,0x14CB7FFF,0x14EB7FFF};
	uint8_t *P = data;
    uint8_t check_sum;
	uint16_t surplus = lenth+1;

	TXHeader.StdId=0x00000000;
	TXHeader.ExtId=0x140B7FFF;
	TXHeader.DLC=8;
	TXHeader.IDE=CAN_ID_EXT;
	TXHeader.RTR=CAN_RTR_DATA;
	TXHeader.TransmitGlobalTime = DISABLE;
	//HAL_CAN_AddTxMessage(hcan,&TXHeader,TXmessage,&pTxMailbox);

	 while(surplus > 8)
	    {
		    TXHeader.ExtId=id_index[pack_id];
			memcpy(TXmessage,P,8);
			while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan ) == 0 );
			HAL_CAN_AddTxMessage(&hcan,&TXHeader,TXmessage,&pTxMailbox);
//			CanSetDataPackId(&can_frame_id, pack_id);
//			CanSendFrame(CanSetDataPackId(&can_frame_id, pack_id),data_buffer);
			pack_id++;
			P+=8;
			surplus-=8;
//			DelayMs(100);
	    }
	    check_sum = getCheckSum(data,lenth);
	    memset(TXmessage,0,8);
	    memcpy(TXmessage,P,surplus-1);
	    TXmessage[surplus-1] = check_sum;
	    TXHeader.ExtId=id_index[pack_id];
	    while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan ) == 0 );
	    HAL_CAN_AddTxMessage(&hcan,&TXHeader,TXmessage,&pTxMailbox);

//	    CanSendFrame(CanSetDataPackId(&can_frame_id, pack_id),data_buffer);


}


uint8_t  getCheckSum(uint8_t *data,uint16_t lenth)
{
	uint32_t sum=0;
	for(uint16_t i=0;i<lenth;i++)
	{
		sum+=data[i];

	}
	return (uint8_t )sum;

}
