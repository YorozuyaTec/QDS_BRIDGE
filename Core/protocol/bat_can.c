/*
 * bat_can.c
 *
 *  Created on: 2021年11月23日
 *      Author: Administrator
 */



#include "bat_can.h"


BatInfoTypeDef can_battery;
ChargeFlagsTypeDef chg_flags;
uint8_t chg_fsm = CHG_INIT;
int16_t temp16=0;


void bat_can_data_handler(uint32_t id,uint8_t *data)
{
	TIM2->CNT=0;
	switch(id)
	{
	case PACK:
		can_battery.ntc_num = data[0];
		can_battery.series_num = data[2];
		break;
	case CODE1:
		for(uint8_t i=0;i<8;i++)
		{
			if(data[i]>127) //not ascii
			{
				data[i]=0;
			}
		}
		memcpy(&can_battery.battery_code[0],data,8);
		break;
	case CODE2:
		for(uint8_t i=0;i<8;i++)
		{
			if(data[i]>127) //not ascii
			{
				data[i]=0;
			}
		}
		memcpy(&can_battery.battery_code[8],data,8);
		break;
	case CODE3:
		for(uint8_t i=0;i<8;i++)
		{
			if(data[i]>127) //not ascii
			{
				data[i]=0;
			}
		}
		memcpy(&can_battery.battery_code[16],data,8);
		break;
	case BSP:
		can_battery.max_value.max_charge_current = (data[1]*256+data[0])*10;
		//can_battery.max_value.max_discharge_current = -25000;
		break;
	case BUI:
		can_battery.battery_soc = data[0];
		can_battery.battery_soh = data[1];
		can_battery.pack_voltage = ((data[3]*256)+data[2])*10;
		memcpy(&can_battery.cycle_counter,&data[4],2);
		can_battery.full_capacity = (data[7]*256+data[6])*10;
		can_battery.design_capacity = can_battery.full_capacity;
		can_battery.current_capacity = can_battery.full_capacity*can_battery.battery_soc/100;
		break;
	case BCS:
		//can_battery.pack_voltage = ((data[3]*256)+data[2])*10;
		chg_flags.chg_status_flag = true;
		memcpy(&temp16,&data[2],2);
		can_battery.real_time_current = temp16*10;
		can_battery.battery_soc = data[4];
		//can_battery.max_value.max_pack_temperature = data[5];

		break;
	case BTP:
		can_battery.mosfet_tmp = data[7]-40;
		can_battery.highest_cell_tmp = data[0];
		can_battery.lowest_cell_tmp = data[0];
		for(uint8_t i=0;i<can_battery.ntc_num;i++)
		{
			if(data[i]>can_battery.highest_cell_tmp)
			{
				can_battery.highest_cell_tmp = data[i];
			}
			if(data[i]<can_battery.lowest_cell_tmp)
			{
				can_battery.lowest_cell_tmp = data[i];
			}
		}
		can_battery.highest_cell_tmp -= 40;
		can_battery.lowest_cell_tmp -= 40;

		break;
	case BCD1:
		memcpy(&can_battery.cell_voltage[0], data,8);
		break;
	case BCD2:
		memcpy(&can_battery.cell_voltage[4], data,8);
		break;
	case BCD3:
		memcpy(&can_battery.cell_voltage[8], data,8);
		break;
	case BCD4:
		memcpy(&can_battery.cell_voltage[12], data,8);
		can_battery.max_value.max_cell_voltage = can_battery.cell_voltage[0];
		can_battery.max_value.min_cell_voltage = can_battery.cell_voltage[0];
		for(uint8_t i=0;i<can_battery.series_num;i++)
		{
			if(can_battery.cell_voltage[i]>can_battery.max_value.max_cell_voltage)
			{
				can_battery.max_value.max_cell_voltage = can_battery.cell_voltage[i];
			}
			if(can_battery.cell_voltage[i]<can_battery.max_value.min_cell_voltage)
			{
				can_battery.max_value.min_cell_voltage = can_battery.cell_voltage[i];
			}

		}
		break;
	case BTD:
		can_battery.max_value.max_pack_temperature = data[0]-40;
		can_battery.max_value.min_pack_temperature = data[1]-40;
		can_battery.mosfet_tmp = data[4]-40;
		break;
	case BDD:
		memcpy(&can_battery.version_info.hw_version,&data[0],2);
		memcpy(&can_battery.version_info.fw_version,&data[2],2);
		memcpy(can_battery.version_info.fw_index,&data[4],4);
		break;
	case BFD:
		memcpy(can_battery.error_info.error,&data[0],6);
		can_battery.status_info.pre_charge_circuit = (data[6]&0x02);
		can_battery.status_info.charge_mosfet = (data[6]&0x02);
		break;

	case BHM:
		chg_flags.handshake_flag = true;
		break;
	case BRM:
		chg_flags.identification_flag = true;
		break;
	case BCP:
		chg_flags.bat_chg_parameter = true;
		break;
	case BRO:
		if(data[0] == 0xAA)
		{
			chg_flags.bat_ready_flag = true;
		}
		break;
	case BCL:
		chg_flags.chg_demand_flag = true;
		break;

	case BST:
		chg_flags.chg_bms_stop_flag = true;
		break;
	case BSD:
		chg_flags.chg_bms_cnt_flag = true;
		break;
	}

}

void bat_charge_management()
{
	uint8_t tx_data[8];
	memset(tx_data,0,8);
	uint32_t errors;
	static uint16_t retry_cnt=0;

	if(TIM2->CNT > 2000)  //2S未接收到超时
	{
		TIM2->CNT=0;
		chg_fsm = CHG_INIT;  //error
		memset(&can_battery,0,sizeof(BatInfoTypeDef));
		memset(&chg_flags,0,sizeof(ChargeFlagsTypeDef));
		can_battery.connected_flag = false;
	}

	switch(chg_fsm)
	{
	case CHG_INIT:

		memset(tx_data,0xFF,8);
		user_can_send_data(CBV, tx_data);
		chg_fsm = CHG_CHECK_PAR;
		break;
	case CHG_CHECK_PAR:
		if(can_battery.battery_code[0]>0 && can_battery.battery_soc<100)  //读到编码且未满电  can_battery.battery_soc>=0
		{
			can_battery.connected_flag = true;
			memcpy(&errors,can_battery.error_info.error,4);
			if(errors>0)
			{
				chg_fsm = CHG_INIT;  //error
			}
			else
			{

				chg_fsm = CHG_HANDSHAKE;
			}
			retry_cnt=0;
		}
		else if(can_battery.battery_soc == 100)
		{
			can_battery.connected_flag = true;
			chg_fsm = CHG_FINISH;
		}
		else
		{
			chg_fsm = CHG_INIT;
			retry_cnt++;
			if(retry_cnt>10)  //电池未连接
					{
				retry_cnt = 10;
				can_battery.connected_flag = false;
				chg_fsm = CHG_INIT;
					}
		}
		//memset(tx_data,)

		break;
	case CHG_HANDSHAKE:
		memset(tx_data,0,8);
		user_can_send_data(CHM, tx_data);
		chg_fsm = CHG_WAIT_HANDSHAKE;
		break;
	case CHG_WAIT_HANDSHAKE:
		if(chg_flags.handshake_flag)
		{
			chg_fsm = CHG_IDENTIFICATION;
		}
		else
		{
			chg_fsm  = CHG_HANDSHAKE;
			retry_cnt++;
			if(retry_cnt>50)
			{
				retry_cnt = 0;
				chg_fsm = CHG_INIT;
			}
		}

		break;
	case CHG_IDENTIFICATION:
		memset(tx_data,0xFF,8);
		//tx_data[0] =0;
		//user_can_send_data(CRM, tx_data);
		//HAL_Delay(250);
		tx_data[0] =0xAA;
		user_can_send_data(CRM, tx_data);
		user_can_send_data(CRM, tx_data);

		chg_fsm = CHG_WAIT_IDENTIFICATION;
		break;
	case CHG_WAIT_IDENTIFICATION:
		if(chg_flags.identification_flag && chg_flags.bat_chg_parameter)
		{
			chg_fsm = CHG_MAX_OUTPUT;

		}
		else
		{
			chg_fsm = CHG_IDENTIFICATION;
			retry_cnt++;
			if(retry_cnt>10)
			{
				retry_cnt = 0;
				chg_fsm = CHG_INIT;
			}
		}
		break;
	case CHG_MAX_OUTPUT:
		tx_data[0] = 0x70;tx_data[1] = 0x17;
		tx_data[2] = 0xC0;tx_data[3] = 0x12;
		tx_data[4] = 0x08;tx_data[5] = 0x3E;
		tx_data[6] = 0x0;tx_data[7] = 0x0;
		//memset(tx_data,0x00,8);
		user_can_send_data(CML, tx_data);
		chg_fsm = CHG_WAIT_MAX_OUTPUT;
		break;
	case CHG_WAIT_MAX_OUTPUT:
		if(chg_flags.bat_ready_flag)
			//if(1)
		{
			chg_fsm = CHG_READY;
		}
		else
		{
			chg_fsm = CHG_MAX_OUTPUT;
			retry_cnt++;
			if(retry_cnt>10)
			{
				retry_cnt = 0;
				chg_fsm = CHG_INIT;
			}
		}
		break;
	case CHG_READY:
		//if(chg_flags.cabibet_ready_flag)
		if(1)
		{
			memset(tx_data,0xFF,8);
			tx_data[0] = 0xAA;
			user_can_send_data(CRO, tx_data);
			chg_fsm = CHG_WAIT_READY;
		}
		else
		{
			//chg_fsm = CHG_WAIT_MAX_OUTPUT;
			retry_cnt++;
			if(retry_cnt>10)
			{
				retry_cnt = 0;
				chg_fsm = CHG_INIT;
			}
		}
		break;
	case CHG_WAIT_READY:
		if(chg_flags.chg_demand_flag && chg_flags.chg_status_flag)
		{
			chg_fsm = CHG_ING;
		}
		if(chg_flags.chg_bms_stop_flag)
		{
			chg_fsm = CHG_INIT;
		}
		break;
	case CHG_ING:
		//    	tx_data[0] = 0x38;tx_data[1] = 0x15;
		//    	tx_data[2] = 0xE8;tx_data[3] = 0x03;
		memset(tx_data,0,8);
		user_can_send_data(CCS, tx_data);
		user_can_send_data(CCS, tx_data);
		user_can_send_data(CBV, tx_data);
		user_can_send_data(CCS, tx_data);
		//user_can_send_data(CCS, tx_data);
		//user_can_send_data(CCS, tx_data);
	//	user_can_send_data(CBV, tx_data);
		user_can_send_data(CBV, tx_data);
		chg_fsm = CHG_WAIT_READY;
		if(chg_flags.chg_bms_stop_flag)
		{
			chg_fsm = CHG_STOP;
		}
		break;
	case CHG_STOP:
		chg_fsm = CHG_FINISH;

		break;
	case CHG_FINISH:
		memset(tx_data,0xFF,8);
		user_can_send_data(CBV, tx_data);
		chg_fsm = CHG_CHECK_PAR;
		break;

	}





	//	if(can_battery.battery_soh > 0)  //
	//	{
	//
	//
	//	}
	//	else
	//	{
	//
	//	}


}




