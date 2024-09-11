/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TM1650.h"
#include "PCF8563.h"
#include "key.h"
#include "myFlash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ANGLE_PER_HOUR 10
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
userkey *key0 = NULL;
userkey *key1 = NULL;
userkey *key2 = NULL;
tm1650 *TM1650 = NULL;
PCF8563_Controller *Pcf8563_ctrl = NULL;
Motor_Controller *Motor_Ctrl = NULL;

extern MyUart myuart;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */
  key0 = newKey(0);
  key1 = newKey(1);
  key2 = newKey(2);

  TM1650 = newTM1650(TM1650_BRIGHT2);
  Pcf8563_ctrl = newPCF8563();
  Motor_Ctrl = newMotorCtrl();

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  MyUart_init(&myuart);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 主线程，刷新数码显示时间（20s）
    if (Pcf8563_ctrl->timeFlash == TIME_OK)
    {
      Pcf8563_ctrl->timeFlash = TIME_WAIT;
      HAL_StatusTypeDef status = Pcf8563_ctrl->time_updata(&Pcf8563_ctrl);
      if (HAL_OK == status)
      {
        TM1650->TM1650_show_time(TM1650, Pcf8563_ctrl->data_time->Hour, Pcf8563_ctrl->data_time->Min);
      }

      /*输出一些状态信息*/
      /*       char buffer[MAX_DATA_LENGTH] = {0};
            sprintf(buffer,
                    "time is : %d : %d \r\n",
                    Pcf8563_ctrl.data_time->Hour,
                    Pcf8563_ctrl.data_time->Min);
            myuart.uart_send(&myuart, buffer); */
    }

    // 主线程，随时间旋转电机
    if (Pcf8563_ctrl->timehourFlash == HOUR_UPDATA)
    {
      Pcf8563_ctrl->timehourFlash = HOUR_IDEAL;
      if (Pcf8563_ctrl->data_time->Hour < 18 && Pcf8563_ctrl->data_time->Hour > 9) // 白天转动
      {
        Motor_Ctrl->motor_right_correct(&Motor_Ctrl, ANGLE_PER_HOUR);
      }
      else if (Pcf8563_ctrl->data_time->Hour == 18) // 18：00复位
      {
        Motor_Ctrl->motor_reset(&Motor_Ctrl);
      }
    }

    // 主线程，响应按键动作
    key0->response(key0);
    key1->response(key1);
    key2->response(key2);
  }

  deletePCF8563(Pcf8563_ctrl);
  delete_MotorCtrl(Motor_Ctrl);
  deleteTM1650(TM1650);
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
