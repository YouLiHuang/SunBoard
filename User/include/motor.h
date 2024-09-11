#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"
#include "tim.h"

/*默认八拍形式 1：八拍 0：4：拍*/
#define EIGHT_STEP 1

#if EIGHT_STEP == 1
#define FOURTH_STEP 0
#else
#define FOURTH_STEP 1
#endif

#define FAST_STEP 30
#define SLOW_STEP 3

#define MOTORA_H HAL_GPIO_WritePin(MOTORA_GPIO_Port, MOTORA_Pin, GPIO_PIN_SET)
#define MOTORA_L HAL_GPIO_WritePin(MOTORA_GPIO_Port, MOTORA_Pin, GPIO_PIN_RESET)

#define MOTORB_H HAL_GPIO_WritePin(MOTORB_GPIO_Port, MOTORB_Pin, GPIO_PIN_SET)
#define MOTORB_L HAL_GPIO_WritePin(MOTORB_GPIO_Port, MOTORB_Pin, GPIO_PIN_RESET)

#define MOTORC_H HAL_GPIO_WritePin(MOTORC_GPIO_Port, MOTORC_Pin, GPIO_PIN_SET)
#define MOTORC_L HAL_GPIO_WritePin(MOTORC_GPIO_Port, MOTORC_Pin, GPIO_PIN_RESET)

#define MOTORD_H HAL_GPIO_WritePin(MOTORD_GPIO_Port, MOTORD_Pin, GPIO_PIN_SET)
#define MOTORD_L HAL_GPIO_WritePin(MOTORD_GPIO_Port, MOTORD_Pin, GPIO_PIN_RESET)

typedef enum
{
    MOTOR_CORRECT_MODR = 0x00,
    MOTOR_IDEAL_MODE

} Ctrl_Mode;

typedef enum
{
    MOTOR_BUSY = 0x00,
    MOTOR_READY = 0x01
} Motor_Status;

typedef enum
{
    LEFT_DIR = 0x00,
    RIGHT_DIR = 0x01
} Motor_Dir;

typedef struct
{
    TIM_HandleTypeDef *timer;
    // 调节模式标志
    Ctrl_Mode mode;
    Motor_Status motor_status;
    // 方向
    Motor_Dir Dir;
    // 波形控制，步进
    uint8_t Current_Step;
    // 映射 0-4095 ==> 0-360°
    uint32_t Current_Angle;
    uint32_t Target_Angle;
    // 累计转角
    uint16_t total_angle;

    void (*set_current_angle)(void *Motor_Controller, uint16_t angle);
    void (*set_target_angle)(void *Motor_Controller, uint16_t angle);
    void (*set_total_angle)(void *Motor_Controller, uint16_t angle);

    uint16_t (*get_current_angle)(void *Motor_Controller);
    uint16_t (*get_target_angle)(void *Motor_Controller);

    void (*motor_left_correct)(void *Motor_Controller, uint8_t angle_step);
    void (*motor_right_correct)(void *Motor_Controller, uint8_t angle_step);
    void (*motor_stop)(void *Motor_Controller);
    void (*motor_reset)(void *Motor_Controller);

    void (*status_led_ctrl)(void *Motor_Controller, GPIO_PinState state);
    void (*motor_wave_ctrl)(void *Motor_Controller);

} Motor_Controller;

void Motor_Ctrl_init(Motor_Controller *);

#endif