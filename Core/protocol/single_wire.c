/*
 * single_wire.c
 *
 *  Created on: 2021年11月23日
 *      Author: Administrator
 */

#include "single_wire.h"

//10Byte
uint8_t cmd_00[] = { 0x47, 0x16, 0x01, 0x00 ,0x04, 0x46, 0x00, 0x00, 0x00, 0xA8};
//14Byte
uint8_t cmd_0F[] = { 0x47,0x16,0x01,0x0F,0x08,0x06,0x1B,0x00,0x00,0x01,0x45,0x00,0x00,0xDC};
//22Byte
uint8_t cmd_18[] = { 0x47,0x16,0x01,0x18,0x10,0xBA,0x45,0x00,0x00,0xE0,0xC4,0x00,0x00,0x03,
		0x02,0x01,0x00,0x55,0x31,0x33,0x31,0x19};
//38Byte
uint8_t cmd_23[] = { 0x47,0x16,0x01,0x23,0x20,0x42,0x4E,0x39,0x37,0x50,0x30,0x30,0x31,0x39,0x38,0x31,0x39,
		0x30,0x30,0x32,0x33,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x58};
//46Byte
uint8_t cmd_24[] = { 0x47,0x16,0x01,0x24,0x28,0x1A,0x0E,0x1A,0x0E,0x1B,0x0E,0x1B,0x0E,0x1B,0x0E,0x1B,0x0E,
		0x1D,0x0E,0x1B,0x0E,0x1A,0x0E,0x18,0x0E,0x1A,0x0E,0x17,0x0E,0x1A,0x0E,0x1C,0x0E,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDF};
//20Byte
uint8_t cmd_26[] = { 0x47,0x16,0x01,0x26,0x0E,0x1E,0x6E,0xFF,0xFF,0x8E,0x27,0x00,0x00,0x8F,0x10,0x45,0x0A,0x3B,0x0A,0x04};
//32Byte
uint8_t cmd_A0[] = { 0x47,0x16,0x01,0xA0,0x1A,0xAC,0x3D,0x00,0x00,0x00,0x00,0x00,0x00,0x27,0x63,0x71,0xC5,
		0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x19,0x1A,0x1A,0x8C,0x00,0x65,0x00,0x1A};

uint8_t cmd_00_w[] = {0x47, 0x16, 0x00, 0x00, 0x00, 0x5D};

uint8_t cmd_A0_1[] = {0x47, 0x16, 0x01, 0xA0, 0x01, 0x28,0x27};


int8_t check_sum(uint8_t  *data , uint8_t lenth)
{
	uint16_t sum=0;
	for(uint8_t  i=0;i<lenth-1;i++)
	{
		sum+=data[i];

	}
	if((uint8_t)sum == data[lenth-1])
	{
		return 1;
	}

	return 0;
}

int8_t check_sum_head(uint8_t  *data , uint8_t lenth)
{
	uint16_t sum=0;
	if(data[0] != 0x46 || data[1] != 0x16 )
	{
		return 0;
	}
	for(uint8_t  i=0;i<lenth-1;i++)
	{
		sum+=data[i];

	}
	if((uint8_t)sum == data[lenth-1])
	{
		return 1;
	}

	return 0;
}

uint8_t get_check_sum(uint8_t  *data , uint8_t lenth)
{
	uint16_t sum=0;
	for(uint8_t  i=0;i<lenth;i++)
	{
		sum+=data[i];

	}
	return (uint8_t)sum;
}


void signal_wire_read_handler()
{
	uint8_t *P;
	uint16_t lenth;
	int32_t temp_value;
	if(signal_wire_huart_module.rec_flag)
	{
		uart_module_stop_receive(&signal_wire_huart_module);
		signal_wire_huart_module.rec_flag = 0;
		//process
		P = signal_wire_huart_module.uart_module_receive_buffer;
		lenth = signal_wire_huart_module.rec_lenth;

		if(can_battery.connected_flag)
		{
			if(check_sum_head(P, lenth))  //
			{
				//uart_module_send_hex(P, lenth);
				if(P[2] == 0x01)  //读取指令
				{
					switch(P[3])
					{
					case 0x00:  //综合信息
						uart_module_send_hex(cmd_00, sizeof(cmd_00));
						break;
					case 0x0F:
						memcpy(&cmd_0F[5],&can_battery.current_capacity,4);
						memcpy(&cmd_0F[9],&can_battery.full_capacity,4);
						cmd_0F[13] = getCheckSum(cmd_0F, 13);
						uart_module_send_hex(cmd_0F, sizeof(cmd_0F));
						break;
					case 0x18:  //设计容量 设计电压 版本
						memcpy(&cmd_18[5],&can_battery.design_capacity,4);
						memcpy(&cmd_18[9],&can_battery.design_voltage,4);
						memcpy(&cmd_18[13],&can_battery.version_info,8);
						cmd_18[21] = getCheckSum(cmd_18, 21);
						uart_module_send_hex(cmd_18, sizeof(cmd_18));
						break;
					case 0x23:  //电池编码
						memcpy(&cmd_23[5],can_battery.battery_code,32);
						cmd_23[37] = getCheckSum(cmd_23, 37);
						uart_module_send_hex(cmd_23, sizeof(cmd_23));
						break;
					case 0x24:  //单体电芯电压
						memcpy(&cmd_24[5],&can_battery.cell_voltage[0],32);
						cmd_24[45] = getCheckSum(cmd_24, 45);
						uart_module_send_hex(cmd_24, sizeof(cmd_24));
						break;
					case 0x26:  //电池最值记录
						memcpy(&cmd_26[5],&can_battery.max_value.max_discharge_current,4);
						memcpy(&cmd_26[9],&can_battery.max_value.max_charge_current,4);
						cmd_26[19] = getCheckSum(cmd_26, 19);
						uart_module_send_hex(cmd_26, sizeof(cmd_26));
						break;
					case 0xA0:  //电池内部状态
						if(P[4] == 0x01)
						{
							uart_module_send_hex(cmd_A0_1, sizeof(cmd_A0_1));
						}
						else
						{
							memcpy(&cmd_A0[7],can_battery.error_info.error,6);
							cmd_A0[13] = can_battery.battery_soc;
							cmd_A0[14] = can_battery.battery_soh;
							memcpy(&cmd_A0[15],&can_battery.pack_voltage,4);
							temp_value = abs(can_battery.real_time_current);
							memcpy(&cmd_A0[19],&temp_value,4);
							//cmd_A0[23] = can_battery.highest_cell_tmp;
							cmd_A0[23] = can_battery.max_value.max_pack_temperature;  //
							//cmd_A0[24] = can_battery.lowest_cell_tmp;
							cmd_A0[24] = can_battery.max_value.min_pack_temperature;
							cmd_A0[25] = can_battery.mosfet_tmp;
							cmd_A0[26] = can_battery.mosfet_tmp;
							memcpy(&cmd_A0[29],&can_battery.cycle_counter,2);
							cmd_A0[31] = getCheckSum(cmd_A0, 31);
							uart_module_send_hex(cmd_A0, sizeof(cmd_A0));
						}
						break;
					}
				}
				else if(P[2] == 0x00)  //写指令
				{
					if(P[3] == 0x00)   //写00 换电柜充电准备就绪
					{
						chg_flags.cabibet_ready_flag = true;
						uart_module_send_hex(cmd_00_w, sizeof(cmd_00_w));
					}

				}
			}
		}

		//	uart_module_send_hex(signal_wire_huart_module.uart_module_receive_buffer,signal_wire_huart_module.rec_lenth);
		uart_module_start_receive(&signal_wire_huart_module);
		memset(signal_wire_huart_module.uart_module_receive_buffer,0,sizeof(signal_wire_huart_module.uart_module_receive_buffer));
		//clear flags
	}



}

void signle_test()
{
	uart_module_send_hex(cmd_00, sizeof(cmd_00));
}
