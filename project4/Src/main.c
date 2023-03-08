/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

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
int limiter = 0;
struct block {
	bool up;
	bool down;
	bool left;
	bool right;
	bool push;

} block = { 0, 0, 0, 0 };
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void resetDigit(uint16_t digit) {
	HAL_GPIO_WritePin(DIGITS, DIG_ALL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SEGMENTS, SEG_ALL, GPIO_PIN_RESET);

//	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);
}
void setDigit(uint16_t digit, int value) {
	resetDigit(digit);
	HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SEGMENTS, segments[value], GPIO_PIN_SET);
	//HAL_GPIO_WritePin(DIGITS, digit, GPIO_PIN_RESET);

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
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim2);

	__HAL_RCC_GPIOE_CLK_ENABLE(); //joystick
	__HAL_RCC_GPIOG_CLK_ENABLE(); //segments
	__HAL_RCC_GPIOB_CLK_ENABLE(); //digits
	HAL_PWREx_EnableVddIO2();

	GPIO_InitTypeDef G = { .Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2
			| GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_9,
			.Mode = GPIO_MODE_OUTPUT_PP, .Speed = SPEED, .Pull = GPIO_NOPULL };
	HAL_GPIO_Init(SEGMENTS, &G);

	GPIO_InitTypeDef B = { .Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
			| GPIO_PIN_5, .Mode =
	GPIO_MODE_OUTPUT_PP, .Speed = SPEED, .Pull = GPIO_NOPULL, };
	HAL_GPIO_Init(DIGITS, &B);

	GPIO_InitTypeDef E = { .Pin = RIGHT | LEFT | FRONT | BACK | PUSH, .Mode =
	GPIO_MODE_INPUT, .Speed = SPEED, .Pull = GPIO_NOPULL, };
	HAL_GPIO_Init(JOYSTICK, &E);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 3999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_OC_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	__HAL_TIM_ENABLE_OCxPRELOAD(&htim2, TIM_CHANNEL_1);
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_IncTick();
	countTime();
	actions();
	if (limiter >= 5)
		limiter = 0;

	if (mode == MMSS) {
		if (limiter == 0)
			setDigit(DIG_1, minutes / 10);
		if (limiter == 1)
			setDigit(DIG_2, minutes % 10);
		if (limiter == 2)
			setDigit(DIG_2, DOT);
		if (limiter == 3)
			setDigit(DIG_3, seconds / 10);
		if (limiter == 4)
			setDigit(DIG_4, seconds % 10);
	} else {
		if (limiter == 0)
			setDigit(DIG_1, hours / 10);
		if (limiter == 1)
			setDigit(DIG_2, hours % 10);
		if (limiter == 2)
			setDigit(DIG_2, DOT);
		if (limiter == 3)
			setDigit(DIG_3, minutes / 10);
		if (limiter == 4)
			setDigit(DIG_4, minutes % 10);
	}
	counter++;
	limiter++;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
