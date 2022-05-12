/*
 * uart_module.h
 *
 *  Created on: 2021年7月15日
 *      Author: Zou
 */

#ifndef MODULE_UART_MODULE_H_
#define MODULE_UART_MODULE_H_

#include "includes.h"

#define RECEIVE_BUFFER_SIZE  256


typedef struct
{
	UART_HandleTypeDef *UART;
	DMA_HandleTypeDef *hdma_usart_rx;
	uint8_t rec_flag;
	uint16_t rec_lenth;
	uint8_t uart_module_receive_buffer[RECEIVE_BUFFER_SIZE];
	void ( *Handler) (void);


}uart_module_TypeDef;


extern uart_module_TypeDef signal_wire_huart_module;
extern uart_module_TypeDef modbus_huart_module;


void uart_module_init(uart_module_TypeDef *UART_MODULE,UART_HandleTypeDef *UART,DMA_HandleTypeDef *hdma_usart_rx);
void usb_uart_printLog(void);
void modbus_printLog(void);
void uart_module_IRQHandler(uart_module_TypeDef *UART_MODULE);
void uart_module_send_string(char *str);
void usb_uart_send_string(char *str);
void modbus_uart_send_string(char *str);
void uart_module_send_hex(uint8_t *data,uint8_t lenth);
void usb_uart_module_send_hex(uint8_t *data,uint8_t lenth);
void uart_module_stop_receive(uart_module_TypeDef *UART_MODULE);
void uart_module_start_receive(uart_module_TypeDef *UART_MODULE);
void rs485_handler(void);
void signal_wire_uart_handler(void);
void Logout (char *fmt, ...);
void uart_module_send_hex(uint8_t *data,uint8_t lenth);

#endif /* MODULE_UART_MODULE_H_ */
