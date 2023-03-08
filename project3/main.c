#include "main.h"
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
#define ALL RIGHT || LEFT || FRONT || BACK || PUSH

#define SPEED GPIO_SPEED_FREQ_LOW

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

int main(void) {
	HAL_Init();
	SystemClock_Config();
	/* USER CODE BEGIN 1 */
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

//	MX_GPIO_Init();
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_CRSInitTypeDef RCC_CRSInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}

	__HAL_RCC_CRS_CLK_ENABLE();

	RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
	RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_GPIO;
	RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
	RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(
			48000000, 1);
	RCC_CRSInitStruct.ErrorLimitValue = 34;
	RCC_CRSInitStruct.HSI48CalibrationValue = 32;

	HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
