/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32l4xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32l4xx_it.h"

#define SPEED GPIO_SPEED_FREQ_HIGH

#define D_BLUE GPIO_PIN_12
#define D_RED GPIO_PIN_13
#define B_GREEN GPIO_PIN_8

#define RIGHT GPIO_PIN_0
#define LEFT GPIO_PIN_1
#define UP GPIO_PIN_2
#define DOWN GPIO_PIN_3
#define PUSH GPIO_PIN_15
#define ALL RIGHT || LEFT || FRONT || BACK || PUSH

enum color_switch
{
    red,
    green,
    blue,
    all
};
color_switch = 0;

int counter = 0;
int block_right = 0;
int block_left = 0;
int block_up = 0;
int block_down = 0;
int reset = 0;

int initial_initial = 1000;
int initial = 1000;

#define MIN_VALUE 80
#define MAX_VALUE 0
#define STEP 25

volatile uint32_t msTicks;
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1)
    {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
        /* USER CODE END W1_MemoryManagement_IRQn 0 */
    }
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
        /* USER CODE END W1_UsageFault_IRQn 0 */
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
    /* USER CODE BEGIN SVCall_IRQn 0 */

    /* USER CODE END SVCall_IRQn 0 */
    /* USER CODE BEGIN SVCall_IRQn 1 */

    /* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
    /* USER CODE BEGIN PendSV_IRQn 0 */

    /* USER CODE END PendSV_IRQn 0 */
    /* USER CODE BEGIN PendSV_IRQn 1 */

    /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	label:
    HAL_IncTick();
    msTicks++;
    if(counter < initial){
    	 HAL_GPIO_WritePin(GPIOD, D_RED, GPIO_PIN_RESET);
    	 HAL_GPIO_WritePin(GPIOB, B_GREEN, GPIO_PIN_RESET);
    	 HAL_GPIO_WritePin(GPIOD, D_BLUE, GPIO_PIN_RESET);
    	 counter++;
    	 goto label;
    }else counter = 0;
    if (!HAL_GPIO_ReadPin(GPIOE, RIGHT) && block_right == 0)
    {
        block_right = 1;
        initial += STEP;
        if(initial > initial_initial){
        	initial = 0;
        }
    }
    else if (HAL_GPIO_ReadPin(GPIOE, RIGHT) && block_right == 1)
    {
        block_right = 0;
    }

    if (!HAL_GPIO_ReadPin(GPIOE, LEFT) && block_left == 0)
    {
        block_left = 1;
        initial -= STEP;
        if(initial < 0){
        	initial = initial_initial;
        }
    }
    else if (HAL_GPIO_ReadPin(GPIOE, LEFT) && block_left == 1)
    {
        block_left = 0;
    }

    if (!HAL_GPIO_ReadPin(GPIOE, UP) && block_up == 0)
    {
        block_up = 1;

        if (color_switch >= all)
        {
            color_switch = 0;
        }
        else color_switch++;
    }
    else if (HAL_GPIO_ReadPin(GPIOE, UP) && block_up == 1)
    {
        block_up = 0;
    }

    if (!HAL_GPIO_ReadPin(GPIOE, DOWN) && block_down == 0)
    {
        block_down = 1;

        if (color_switch >= all)
        {
            color_switch = 0;
        }
        else color_switch++;
    }
    else if (HAL_GPIO_ReadPin(GPIOE, DOWN) && block_down == 1)
    {
        block_down = 0;
    }
    if (color_switch == red || color_switch == all)
        HAL_GPIO_WritePin(GPIOD, D_RED, GPIO_PIN_SET);
    if (color_switch == green || color_switch == all)
        HAL_GPIO_WritePin(GPIOB, B_GREEN, GPIO_PIN_SET);
    if (color_switch == blue || color_switch == all)
        HAL_GPIO_WritePin(GPIOD, D_BLUE, GPIO_PIN_SET);
    if(color_switch != all){
    	 if(color_switch != red) HAL_GPIO_WritePin(GPIOD, D_RED, GPIO_PIN_RESET);
    	 if(color_switch != green) HAL_GPIO_WritePin(GPIOB, B_GREEN, GPIO_PIN_RESET);
    	 if(color_switch != blue) HAL_GPIO_WritePin(GPIOD, D_BLUE, GPIO_PIN_RESET);
    }


}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
