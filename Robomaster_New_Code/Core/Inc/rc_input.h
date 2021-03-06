/*
 * rc_input.h
 *
 *  Created on: Dec 16, 2021
 *      Author: wx
 */

#ifndef TASKS_INC_RC_INPUT_H_
#define TASKS_INC_RC_INPUT_H_



void control_task(void *pvParameters);
void dbus_reset();
HAL_StatusTypeDef rc_start(UART_HandleTypeDef *huart);
void dbus_remote_ISR(DMA_HandleTypeDef *hdma);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

enum left_switch
{
	aimbot_enable = 1,
	launcher_off = 2,
	launcher_on = 3
};

enum right_switch
{
	all_off = 1,
	all_on = 2,
	gimbal_on = 3
};

typedef struct {
	/* Joysticks - Values range from -660 to 660 */
	int16_t right_x;
	int16_t right_y;
	int16_t left_x;
	int16_t left_y;
	/* Switches - Values range from 1 - 3 */
	int8_t left_switch;
	int8_t right_switch;
	/* Mouse movement - Values range from -32768 to 32767 */
	int16_t mouse_x;
	int16_t mouse_y;
	int16_t mouse_z;
	int32_t mouse_hori;
	int32_t mouse_vert;
	/* Mouse clicks - Values range from 0 to 1 */
	int8_t mouse_left;
	int8_t mouse_right;

	/* Keyboard keys mapping
	 * Bit0 -- W 键
	 * Bit1 -- S 键
	 *	Bit2 -- A 键
	 *	Bit3 -- D 键
	 *	Bit4 -- Q 键
	 *	Bit5 -- E 键
	 *	Bit6 -- Shift 键
	 *	Bit7 -- Ctrl 键
	 *
	 */
	int16_t keyboard_keys;
	int16_t side_dial;
	uint32_t last_time;
} remote_cmd_t;

#endif /* TASKS_INC_RC_INPUT_H_ */
