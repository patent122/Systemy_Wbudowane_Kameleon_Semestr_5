#include "main.h"
#include "stm32l4xx_it.h"
#include <stdbool.h>
#include "kamami_l496_mems.h"
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
#define DIG_ALL DIG_1 | DIG_2 | DIG_3 | DIG_4
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

int limiter = 0;
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

enum MODE {
	X, Y, Z
};
int MODE = 0;
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
	HAL_GPIO_WritePin(DIGITS, DIG_ALL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SEGMENTS, SEG_ALL, GPIO_PIN_RESET);

//	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);
}
void setSegment(uint16_t digit, uint16_t segment) {
	resetDigit(digit);
	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SEGMENTS, segment, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);

}
void setDigit(uint16_t digit, int value) {
	resetDigit(digit);
	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SEGMENTS, segments[value], GPIO_PIN_SET);
	//HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);

}

void action_change_mode() {
	if (!HAL_GPIO_ReadPin(GPIOE, PUSH) && block.push == 0) {
		block.push = 1;
		if (MODE == X)
			MODE = Y;
		else if (MODE == Y)
			MODE = Z;
		else if (MODE == Z)
			MODE = X;
		else
			MODE = X;
	} else if (HAL_GPIO_ReadPin(GPIOE, PUSH) && block.push == 1) {
		block.push = 0;
	}
}

void actions() {
	action_change_mode();
}

int safety(int val) {
	if (val <= 10 && val >= 0)
		return val;
	return 1;
}
void SysTick_Handler(void) {
	HAL_IncTick();
	actions();
	if (limiter >= 4)
		limiter = 0;

	float value = 0x7fff;
	float f_x = (float) mems_acc_read_x() * 2.0 / value;
	float f_y = (float) mems_acc_read_y() * 2.0 / value;
	float f_z = (float) mems_acc_read_z() * 2.0 / value;

	int x = 100 * f_x;
	int y = 100 * f_y;
	int z = 100 * f_z;
//65000 - 2G
//*2/memsaccmaxval = 0x7ffe
	if (limiter == 0) {
		if (MODE == X)
			setSegment(DIG_1, SEG_A);
		else if (MODE == Y)
			setSegment(DIG_1, SEG_G);
		else if (MODE == Z)
			setSegment(DIG_1, SEG_D);
	}
	if (limiter == 1) {
		if (MODE == X)
			setDigit(DIG_2, safety(x / 100));
		else if (MODE == Y)
			setDigit(DIG_2, safety(y / 100));
		else if (MODE == Z)
			setDigit(DIG_2, safety(z / 100));
	}
	if (limiter == 2) {
		if (MODE == X)
			setDigit(DIG_3, safety((x % 100) / 10));
		else if (MODE == Y)
			setDigit(DIG_3, safety((y % 100) / 10));
		else if (MODE == Z)
			setDigit(DIG_3, safety((z % 100) / 10));
	}
	if (limiter == 3) {
		if (MODE == X)
			setDigit(DIG_4, safety((x % 100 % 10) / 1));
		else if (MODE == Y)
			setDigit(DIG_4, safety((y % 100 % 10) / 1));
		else if (MODE == Z)
			setDigit(DIG_4, safety((z % 100 % 10) / 1));
	}
//	if (limiter == 1)
//		setDigit(DIG_2, (int) (y % 1000) / 100);
//	if (limiter == 2)
//		setDigit(DIG_3, (int) (z % 1000 % 100) / 10);
//	if (limiter == 3)
//		setDigit(DIG_4, (z % 1000 % 100) / 10);

	limiter++;
}
