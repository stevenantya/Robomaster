/*
 * rc_input.c
 *
 *  Created on: Dec 16, 2021
 *      Author: wx
 */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_dma.h"
#include "main.h"
#include "rc_input.h"

#define JOYSTICK_OFFSET 1024;
remote_cmd_t remote_cmd;

uint8_t remote_raw_data[18];


void dbus_rx_cplt(DMA_HandleTypeDef *hdma)
{
		remote_cmd.right_x = (remote_raw_data[0] | remote_raw_data[1] << 8) & 0x07FF;
		remote_cmd.right_x -= JOYSTICK_OFFSET;
		remote_cmd.right_y = (remote_raw_data[1] >> 3 | remote_raw_data[2] << 5) & 0x07FF;
		remote_cmd.right_y -= JOYSTICK_OFFSET;
		remote_cmd.left_x = (remote_raw_data[2] >> 6 | remote_raw_data[3] << 2 | remote_raw_data[4] << 10) & 0x07FF;
		remote_cmd.left_x -= JOYSTICK_OFFSET;
		remote_cmd.left_y = (remote_raw_data[4] >> 1 | remote_raw_data[5] << 7) & 0x07FF;
		remote_cmd.left_y -= JOYSTICK_OFFSET;
		//Left switch position
		remote_cmd.left_switch = ((remote_raw_data[5] >> 4) & 0x000C) >> 2;
		remote_cmd.right_switch = (remote_raw_data[5] >> 4) & 0x0003;
		remote_cmd.mouse_x = ((int16_t)remote_raw_data[6] | ((int16_t)remote_raw_data[7] << 8));
		remote_cmd.mouse_y = ((int16_t)remote_raw_data[8] | ((int16_t)remote_raw_data[9] << 8));
		remote_cmd.mouse_z = ((int16_t)remote_raw_data[10] | ((int16_t)remote_raw_data[11] << 8));
		remote_cmd.mouse_left = (remote_raw_data[12]);
		remote_cmd.mouse_right = (remote_raw_data[13]);
		remote_cmd.keyboard_keys = (remote_raw_data[14]);
		remote_cmd.side_dial = ((int16_t)remote_raw_data[16]) | ((int16_t)remote_raw_data[17] << 8);
		remote_cmd.side_dial -= JOYSTICK_OFFSET;
		remote_cmd.last_time = HAL_GetTick();
}



HAL_StatusTypeDef rc_start(UART_HandleTypeDef *huart)
{
	uint8_t *pData = remote_raw_data;
	uint16_t Size = 18;

	  huart->pRxBuffPtr = pData;
	  huart->RxXferSize = Size;

	  huart->ErrorCode = HAL_UART_ERROR_NONE;
	  huart->RxState = HAL_UART_STATE_BUSY_RX;

	  if (huart->hdmarx != NULL)
	  {
	    /* Set the UART DMA transfer complete callback */
	    huart->hdmarx->XferCpltCallback = dbus_rx_cplt;


	    /* Enable the DMA channel */
	    if (HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->RDR, (uint32_t)huart->pRxBuffPtr, Size) != HAL_OK)
	    {
	      /* Set error code to DMA */
	      huart->ErrorCode = HAL_UART_ERROR_DMA;

	      __HAL_UNLOCK(huart);

	      /* Restore huart->RxState to ready */
	      huart->RxState = HAL_UART_STATE_READY;

	      return HAL_ERROR;
	    }
	  }
	  __HAL_UNLOCK(huart);

	  /* Enable the UART Parity Error Interrupt */
	  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);

	  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

	  /* Enable the DMA transfer for the receiver request by setting the DMAR bit
	  in the UART CR3 register */
	  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

	  return HAL_OK;
}

void dbus_reset()
{
	remote_cmd.right_switch = 3;
	remote_cmd.right_x = 0;
	remote_cmd.right_y = 0;
	remote_cmd.left_x = 0;
	remote_cmd.left_y = 0;
	remote_cmd.left_switch = 3;
	remote_cmd.mouse_x = 0;
	remote_cmd.mouse_y = 0;
	remote_cmd.mouse_z = 0;
	remote_cmd.mouse_left = 0;
	remote_cmd.mouse_right = 0;
}

