/*
 * user_can.h
 *
 *  Created on: 2021年11月8日
 *      Author: Administrator
 */

#ifndef CAN_USER_CAN_H_
#define CAN_USER_CAN_H_


#include "includes.h"


#define HW_VERSION 10
#define SW_VERSION 10


void CAN_senddata(CAN_HandleTypeDef *hcan);
void getAdc(void);
uint8_t  getCheckSum(uint8_t *data,uint16_t lenth);
void CAN_Send_Pack(uint8_t *data,uint16_t lenth);
uint8_t Can_TxMessage(uint8_t ide,uint32_t id,uint8_t len,uint8_t *data);
void CAN1_Config(void);
//void user_can_send_data(uint32_t id , uint8_t *data);
uint8_t user_can_send_data(uint32_t id , uint8_t *data);

typedef  struct
{
	uint8_t head[5];
	uint16_t hw_version;;
	uint16_t sw_version;

    uint16_t battery_voltage;  //mV
    uint16_t input_voltage;
    uint16_t battery_current;  //mA

    uint8_t status;
    uint8_t water_temp;

}FireControllerTypeDef;

extern FireControllerTypeDef fire_controller;

#endif /* CAN_USER_CAN_H_ */
