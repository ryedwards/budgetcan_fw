/*

The MIT License (MIT)

Copyright (c) 2022 Ryan Edwards

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "usbd_gs_can.h"
#include "can.h"
#include "led.h"

LED_HandleTypeDef hledrx;
LED_HandleTypeDef hledtx;

CAN_HandleTypeDef hcan;

extern USBD_GS_CAN_HandleTypeDef hGS_CAN;

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								  |RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/** Configure pins
*/
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, nSI86_EN_Pin|CAN_nSTANDBY_Pin|DCDC_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LED_TX_Pin|LED_RX_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : nSI86_EN_Pin CAN_nSTANDBY_Pin DCDC_EN_Pin */
	GPIO_InitStruct.Pin = nSI86_EN_Pin|CAN_nSTANDBY_Pin|DCDC_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : SENSE_5V_Pin */
	GPIO_InitStruct.Pin = SENSE_5V_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SENSE_5V_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_TX_Pin LED_RX_Pin */
	GPIO_InitStruct.Pin = LED_TX_Pin|LED_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void main_init_cb(void)
{
	/* carry over the LED blink from original firmware */
	for (uint8_t i=0; i<10; i++)
	{
		HAL_GPIO_TogglePin(LED_RX_GPIO_Port, LED_RX_Pin);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);
	}

	HAL_GPIO_WritePin(CAN_nSTANDBY_GPIO_Port, CAN_nSTANDBY_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(nSI86_EN_GPIO_Port,	  nSI86_EN_Pin,		GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DCDC_EN_GPIO_Port,	  DCDC_EN_Pin,		GPIO_PIN_SET);

	hGS_CAN.channels[0] = &hcan;
	can_init(hGS_CAN.channels[0], CAN);

	led_init(&hledrx, LED_RX_GPIO_Port, LED_RX_Pin, LED_MODE_INACTIVE, LED_ACTIVE_LOW);
	led_init(&hledtx, LED_TX_GPIO_Port, LED_TX_Pin, LED_MODE_ACTIVE,   LED_ACTIVE_LOW);
}

void main_task_cb(void)
{
	/* update all the LEDs */
	led_update(&hledrx);
	led_update(&hledtx);

	if (HAL_GPIO_ReadPin(SENSE_5V_GPIO_Port, SENSE_5V_Pin) == 0) {
		/* Do something */
	}
}

void can_on_enable_cb(uint8_t channel)
{
	UNUSED(channel);
	led_set_inactive(&hledtx);
	led_set_active(&hledrx);
}

void can_on_disable_cb(uint8_t channel)
{
	UNUSED(channel);
	led_set_inactive(&hledrx);
	led_set_active(&hledtx);
}

void can_on_tx_cb(uint8_t channel, struct gs_host_frame *frame)
{
	UNUSED(channel);
	UNUSED(frame);
	led_indicate_rxtx(&hledtx);
}

void can_on_rx_cb(uint8_t channel, struct gs_host_frame *frame)
{
	UNUSED(channel);
	UNUSED(frame);
	led_indicate_rxtx(&hledrx);
}

void can_identify_cb(uint32_t do_identify)
{
	if (do_identify) {
		led_blink_start(&hledrx, 250);
	}
	else {
		led_blink_stop(&hledrx);
	}
}