/*
 * bat_can.h
 *
 *  Created on: 2021年11月23日
 *      Author: Administrator
 */

#ifndef PROTOCOL_BAT_CAN_H_
#define PROTOCOL_BAT_CAN_H_

#include "includes.h"


enum
{
	CHG_INIT,                  //初始化       ->CBV
	CHG_CHECK_PAR,             //检查参数
	CHG_HANDSHAKE,         //充电握手        ->CHM   BHM->
	CHG_WAIT_HANDSHAKE,
	CHG_IDENTIFICATION,    //充电柜识别      ->CRM   BRM->,BCP->
	CHG_WAIT_IDENTIFICATION,
	CHG_MAX_OUTPUT,        //最大充电参数     ->CML   BRO->
	CHG_WAIT_MAX_OUTPUT,
	CHG_READY,             //充电柜准备就绪   ->CRO   BCL->,BCS->
	CHG_WAIT_READY,
	CHG_ING,               //充电柜状态交换   ->CCS
	CHG_STOP,              //充电结束         BSD->  ->CSD
	CHG_WAIT_STOP,
	CHG_FINISH

}CHARGE_FSM;

typedef struct
{
	bool handshake_flag;
	bool identification_flag;
	bool bat_chg_parameter;
	bool bat_ready_flag;
	bool cabibet_ready_flag;
	bool chg_demand_flag;
	bool chg_status_flag;
	bool chg_bms_stop_flag;
	bool chg_bms_cnt_flag;

}ChargeFlagsTypeDef;

enum
{
	CBV   =  0x18FC0908,
	PACK  =  0x18F10809,
	CODE1 =  0x18630809,
	CODE2 =  0x18640809,
	CODE3 =  0x18650809,
	BED   =  0x18F30809,
	BSP   =  0x18E00809,
	BUI   =  0x18E10809,
	//BCS   =  0x1C3A0809,
	BTP   =  0x18F50809,
	BCD1  =  0x18F60809,
	BCD2  =  0x18F70809,
	BCD3  =  0x18F80809,
	BCD4  =  0x18F90809,
	BTD   =  0X18FD0809,
	BDD   =  0x18610809,
	BFD   =  0x18620809,

	  //握手阶段
	CHM   =  0X18310908,   //换电柜->BMS  充电握手
	BHM   =  0x18320809,   //BMS->换电柜
     //充电参数配置阶段
	CRM   =  0x18330908,   //换电柜->BMS  换电柜识别
	BRM   =  0x18340809,   //BMS->换电柜
	BCP   =  0x1C350809,   //BMS->换电柜  电池充电参数
     //充电阶段
	CML   =  0x18360908,   //换电柜->BMS  充电器最大输出能力
	BRO   =  0x10370809,   //BMS->换电柜  电池充电准备就绪状态
	CRO   =  0x10380908,   //换电柜->BMS  换电柜输出准备就绪

	 //充电结束阶段
	BCL   =  0x18390809,   //BMS->换电柜  电池充电需求
	BCS   =  0x1C3A0809,   //BMS->换电柜  电池充电总状态
	CCS   =  0x183B0908,   //换电柜->BMS  充电柜充电状态
	//CEM\BEM
	BST   =  0x103C0908,   //BMS->换电柜  BMS中止充电
	CST   =  0x103D0809,   //换电柜->BMS  充电柜中止充电


	BSD   =  0x183E0908,   //BMS->换电柜  BMS统计数据
	CSD   =  0x183F0809    //换电柜->BMS  充电柜统计数据



}BAT_CAN_PGN;


typedef struct
{
	uint16_t fw_version;
	uint16_t hw_version;
	char    fw_index[4];

}BatVersionTypeDef;

typedef struct
{
	int32_t max_discharge_current;
	int32_t max_charge_current;
    uint16_t max_cell_voltage;
    uint16_t min_cell_voltage;
    int8_t max_pack_temperature;
    int8_t min_pack_temperature;

}MaxVlaueTypeDef;

typedef struct
{
	uint8_t charger_connect;
	uint8_t pre_charge_circuit;
	uint8_t charge_mosfet;
	uint8_t bat_chg_stop_flag;

}BatInnerStatusTypeDef;

typedef struct
{
	uint8_t error[6];  //6Byte

}BatErrorInfoTypeDef;


typedef struct
{
	uint8_t ntc_num;
	uint8_t series_num;
	uint32_t current_capacity;
	uint32_t full_capacity;
	uint32_t design_capacity;
	uint32_t design_voltage;
	BatVersionTypeDef version_info;
	char battery_code[32];
	uint16_t cell_voltage[16];
	MaxVlaueTypeDef max_value;
	BatErrorInfoTypeDef error_info;
	BatInnerStatusTypeDef status_info;
	uint8_t battery_soc;
	uint8_t battery_soh;
	uint32_t pack_voltage;
	int32_t real_time_current;
	int8_t highest_cell_tmp;
	int8_t lowest_cell_tmp;
	int8_t mosfet_tmp;
	uint16_t cycle_counter;
    bool connected_flag;

}BatInfoTypeDef;



extern BatInfoTypeDef can_battery;
extern ChargeFlagsTypeDef chg_flags;


void bat_can_data_handler(uint32_t id,uint8_t *data);
void bat_charge_management(void);


#endif /* PROTOCOL_BAT_CAN_H_ */
