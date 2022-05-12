/* * uart_module.c * *  Created on: 2021年7月15日 *      Author: Zou */#include "uart_module.h"uart_module_TypeDef usb_huart_module ={	//.Handler = &modbus_handler};uart_module_TypeDef signal_wire_huart_module ={	.Handler = &signal_wire_read_handler};//uart_module_TypeDef modbus_huart_module =//{//	.Handler = &rs485_handler//};uint8_t uart_cnt=0;/*        Enable printf               */#ifdef __GNUC__#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)#else#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)#endifPUTCHAR_PROTOTYPE{    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);    return ch;}void uart_module_init(uart_module_TypeDef *UART_MODULE,UART_HandleTypeDef *UART,DMA_HandleTypeDef *hdma_usart_rx){    UART_MODULE->UART = UART;    UART_MODULE->hdma_usart_rx = hdma_usart_rx;	__HAL_UART_ENABLE_IT(UART_MODULE->UART, UART_IT_IDLE);	HAL_UART_Receive_DMA(UART_MODULE->UART, UART_MODULE->uart_module_receive_buffer, RECEIVE_BUFFER_SIZE);}void usb_uart_printLog(){	//HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, SET);	usb_uart_send_string(" (C) YorozuyaTec. \r\n");	usb_uart_send_string(" LED Matrix displayer Version 1.0\r\n");	usb_uart_send_string(" 2021.07.18\r\n");    //HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, RESET);}//void modbus_printLog()//{//	HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, SET);//	modbus_uart_send_string(" (C) YorozuyaTec. \r\n");//	modbus_uart_send_string(" LED Matrix displayer Version 1.0\r\n");//	modbus_uart_send_string(" 2021.07.18\r\n");////    HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, RESET);//}void uart_module_IRQHandler(uart_module_TypeDef *UART_MODULE){	uint16_t data_length;   if (RESET != __HAL_UART_GET_FLAG(UART_MODULE->UART, UART_FLAG_IDLE) )		{			__HAL_UART_CLEAR_IDLEFLAG(UART_MODULE->UART);			//printf("\r\nUART1 Idle IQR Detected\r\n");			uart_cnt++;			//HAL_UART_DMAStop(huart);			data_length = RECEIVE_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(UART_MODULE->hdma_usart_rx);			//printf("Receive Data_length = %d : ", data_length);			//HAL_UART_Transmit(huart, UART_MODULE->uart_module_receive_buffer,data_length, 0x200);			//printf("\r\n");			//memset(UART_MODULE->uart_module_receive_buffer, 0, data_length);			UART_MODULE->rec_lenth = data_length;			data_length = 0;			//HAL_UART_Receive_DMA(&huart1,UART_MODULE->uart_module_receive_buffer,RECEIVE_BUFFER_SIZE);		    UART_MODULE->rec_flag = 1;		    UART_MODULE->Handler();		}}//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  //定时器中断回调//{//	static uint16_t sec_cnt = 0;//	//static uint16_t iwg_cnt = 0;//	exled.exled_ms_cnt++;//	if (htim == &htim3)//	{//		uartReceiveTimmerRun(&husb_uart);//		//uartReceiveTimmerRun(&hrs485_uart);//		hsmsTimerRun();//		keyTimerRun();////		if (++sec_cnt > 500)//		{//			sec_cnt = 0;//			sec_flag = true;//			HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);//			HAL_IWDG_Refresh(&hiwdg);//		}////	}//}//void usb_uart_send_string(char *str)//{//	uint8_t lenth;//	lenth = strlen(str);//	//HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, SET);//    HAL_UART_Transmit(&huart1, (uint8_t *)str, lenth, 1000);//   // HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, RESET);//}void usb_uart_module_send_hex(uint8_t *data,uint8_t lenth){	HAL_UART_Transmit(usb_huart_module.UART, data, lenth, 1000);}void uart_module_send_hex(uint8_t *data,uint8_t lenth){	//HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, SET);	uart_module_stop_receive(&signal_wire_huart_module);	HAL_UART_Transmit(signal_wire_huart_module.UART, data, lenth, 1000);	uart_module_start_receive(&signal_wire_huart_module);	//HAL_GPIO_WritePin(RS485_CTRL_GPIO_Port, RS485_CTRL_Pin, RESET);}void uart_module_stop_receive(uart_module_TypeDef *UART_MODULE){	HAL_UART_DMAStop(UART_MODULE->UART);}void uart_module_start_receive(uart_module_TypeDef *UART_MODULE){	UART_MODULE->rec_lenth=0;	HAL_UART_Receive_DMA(UART_MODULE->UART,UART_MODULE->uart_module_receive_buffer,RECEIVE_BUFFER_SIZE);}void rs485_handler(){	if(modbus_huart_module.rec_flag)	{		uart_module_stop_receive(&modbus_huart_module);		modbus_huart_module.rec_flag = 0;		//uart_module_send_hex(modbus_huart_module.uart_module_receive_buffer,modbus_huart_module.rec_lenth);		//uart_module_clean(&huart_module);		uart_module_start_receive(&modbus_huart_module);		//clear flags	}}void signal_wire_uart_handler(){	if(signal_wire_huart_module.rec_flag)	{		uart_module_stop_receive(&signal_wire_huart_module);		signal_wire_huart_module.rec_flag = 0;		uart_module_send_hex(signal_wire_huart_module.uart_module_receive_buffer,signal_wire_huart_module.rec_lenth);		uart_module_start_receive(&signal_wire_huart_module);		//clear flags	}}void Logout (char *fmt, ...){  printf(fmt);}