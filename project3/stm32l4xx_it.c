#include "main.h"
#include "stm32l4xx_it.h"
#include <stdbool.h>

//PUSH -> reset to 00:00
//LEFT, RIGHT -> change mode
//FRONT -> set time to 23:59:50
//BACK -> add 1 hour or 5 minutes
#define TIME_SPEED 1 //speed 1-1000
#define SEGMENTS GPIOG
#define DIGITS GPIOB
#define JOYSTICK GPIOE
//segments
#define SEG_A GPIO_PIN_0
#define SEG_B GPIO_PIN_1
#define SEG_C GPIO_PIN_2
#define SEG_D GPIO_PIN_3
#define SEG_E GPIO_PIN_4
#define SEG_F GPIO_PIN_5
#define SEG_G GPIO_PIN_6
#define SEG_DP GPIO_PIN_9
#define SEG_ALL SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_E | SEG_F | SEG_G | SEG_DP
//digits
#define DIG_1 GPIO_PIN_2
#define DIG_2 GPIO_PIN_3
#define DIG_3 GPIO_PIN_4
#define DIG_4 GPIO_PIN_5
//joystick
#define RIGHT GPIO_PIN_0
#define LEFT GPIO_PIN_1
#define FRONT GPIO_PIN_2
#define BACK GPIO_PIN_3
#define PUSH GPIO_PIN_15
#define ALL RIGHT | LEFT | FRONT | BACK | PUSH

#define SPEED GPIO_SPEED_FREQ_HIGH
#define DOT 10
#define FIRST_HALF 0
#define SECOND_HALF 1
#define HHMM 0
#define MMSS 1

volatile uint8_t hours = 0;
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;
bool mode = MMSS;
int counter = 0;
const uint16_t segments[] = {
SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // 0
		SEG_B | SEG_C, // 1
		SEG_A | SEG_E | SEG_B | SEG_D | SEG_G, // 2
		SEG_A | SEG_B | SEG_G | SEG_C | SEG_D, // 3
		SEG_F | SEG_G | SEG_B | SEG_C, // 4
		SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // 5
		SEG_A | SEG_F | SEG_G | SEG_E | SEG_C | SEG_D, // 6
		SEG_A | SEG_B | SEG_C, // 7
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G, // 9
		GPIO_PIN_9 };

volatile uint32_t msTicks;

struct block {
	bool up;
	bool down;
	bool left;
	bool right;
	bool push;

} block = { 0, 0, 0, 0 };

void NMI_Handler(void) {
	while (1) {
	}
}

void HardFault_Handler(void) {
	while (1) {
	}
}

void MemManage_Handler(void) {
	while (1) {
	}
}

void BusFault_Handler(void) {
	while (1) {
	}
}

void UsageFault_Handler(void) {
	while (1) {
	}
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}
void resetDigit(uint16_t digit) {
	HAL_GPIO_WritePin(SEGMENTS, SEG_ALL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);
}
void setDigit(uint16_t digit, int value) {
	HAL_GPIO_WritePin(SEGMENTS, segments[value], GPIO_PIN_SET);
	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_SET);
	resetDigit(digit);
}
void setDoubleDigit(bool half, int value) {
	if (half == FIRST_HALF) {
		setDigit(DIG_1, value / 10);
		setDigit(DIG_2, value % 10);
		return;
	}
	setDigit(DIG_3, value / 10);
	setDigit(DIG_4, value % 10);
}
void setFullDigit(int value) {
	setDigit(DIG_1, value / 1000);
	value = value % 1000;
	setDigit(DIG_2, value / 100);
	value = value % 100;
	setDigit(DIG_3, value / 10);
	value = value % 10;
	setDigit(DIG_4, value);
}
void countDays() {
	if (hours >= 24) {
		hours = 0;
		minutes = 0;
		seconds = 0;
	}
}
void countHours() {
	if (minutes >= 60) {
		hours++;
		minutes = 0;
		seconds = 0;
	}
}
void countMinutes() {
	if (seconds >= 60) {
		minutes++;
		seconds = 0;
	}
}
void countSeconds() {
	if (counter >= (1000 / TIME_SPEED)) {
		seconds++;
		counter = 0;
	}

}
void countTime() {
	countSeconds();
	countMinutes();
	countHours();
	countDays();
}
void action_reset_time() {
	if (!HAL_GPIO_ReadPin(GPIOE, PUSH) && block.push == 0) {
		block.push = 1;
		seconds = minutes = hours = 0;
	} else if (HAL_GPIO_ReadPin(GPIOE, PUSH) && block.push == 1) {
		block.push = 0;
	}
}
void action_change_mode() {
	if (!HAL_GPIO_ReadPin(GPIOE, LEFT) && block.left == 0) {
		block.left = 1;
		if (mode == 0)
			mode = 1;
		else
			mode = 0;
	} else if (HAL_GPIO_ReadPin(GPIOE, LEFT) && block.left == 1) {
		block.left = 0;
	}

	if (!HAL_GPIO_ReadPin(GPIOE, RIGHT) && block.right == 0) {
		block.right = 1;
		if (mode == 0)
			mode = 1;
		else
			mode = 0;
	} else if (HAL_GPIO_ReadPin(GPIOE, RIGHT) && block.right == 1) {
		block.right = 0;
	}
}

void action_setTime() {
	if (!HAL_GPIO_ReadPin(GPIOE, BACK) && block.down == 0) {
		block.down = 1;
		hours = 23;
		minutes = 59;
		seconds = 45;
	} else if (HAL_GPIO_ReadPin(GPIOE, BACK) && block.down == 1) {
		block.down = 0;
	}
}
void action_add() {
	if (!HAL_GPIO_ReadPin(GPIOE, FRONT) && block.up == 0) {
		block.up = 1;
		if (mode == MMSS)
			minutes += 5;
		if (mode == HHMM)
			hours++;
	} else if (HAL_GPIO_ReadPin(GPIOE, FRONT) && block.up == 1) {
		block.up = 0;
	}
}
void actions() {
	action_reset_time();
	action_change_mode();
	action_setTime();
	action_add();
}
void SysTick_Handler(void) {
	HAL_IncTick();
	countTime();
	actions();
	if (mode == MMSS) {
		setDoubleDigit(FIRST_HALF, minutes);
		setDigit(DIG_2, DOT);
		setDoubleDigit(SECOND_HALF, seconds);
	} else {
		setDoubleDigit(FIRST_HALF, hours);
		setDigit(DIG_2, DOT);
		setDoubleDigit(SECOND_HALF, minutes);
	}
	counter++;
}
