/*
 * timmer_model.c
 *
 *  Created on: 2020
 *      Author: Administrator
 */

#include "includes.h"


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  //1ms IRQ
{
	static uint16_t sec_cnt = 0;
	static uint16_t get_adc_flag=0;
	if (htim == &htim2)
	{
		//uartReceiveTimmerRun(&hrs485);
		//ADS_getAllChannel(30);
		if (++sec_cnt > 1000)
		{
			sec_cnt=0;
		    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}
		if(++get_adc_flag > 100)
		{

		//	ADS_getAllChannel(50);

		}
	}
}

