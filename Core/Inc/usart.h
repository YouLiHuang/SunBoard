/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "string.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define MAX_DATA_LENGTH 50

  typedef enum
  {
    DMA_ON = 0x00,
    DMA_OFF

  } Send_Mode;

  typedef struct
  {
    UART_HandleTypeDef *uart_handle;
    Send_Mode mode;
    char *send_buffer;
    uint8_t *receive_buffer;

    HAL_StatusTypeDef (*uart_send)(void *pointer, char data[]);
    HAL_StatusTypeDef (*uart_receive)(void *pointer, char data[]);

  } MyUart;

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
  void MyUart_init(MyUart *myUart);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

