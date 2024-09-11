#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"
#include "TM1650.h"
#include "PCF8563.h"
#include "tim.h"
#include "usart.h"

typedef enum
{
    DEFAULT_STATUS = 0x01,
    SINGLE_CLICKED,
    DOUBLE_CLICKED,
    LONG_PRESS,
    LONG_LONG_PRESS

} ClickFlag;

typedef enum
{
    KEY_PRESS = 0x00,
    KEY_LONG_PRESS,
    KEY_LONG_LONG_PRESS,
    KEY_RELEASE
} KeyAction;

typedef enum
{
    KEY_IDEAL = 0x00,
    KEY_BUSY = 0x01
} KeyTask;

typedef struct
{
    uint8_t keynum;      // 按键序号
    uint16_t score;      // 表征累计按下的时长（每五个点代表一次有效的按下动作）
    uint16_t up_score;   // 表征松开时长
    uint8_t press_count; // 统计按下次数

    KeyAction action;
    ClickFlag flag;
    KeyTask TaskStatus;

    HAL_StatusTypeDef (*singleClickedCallback)(void *key);
    HAL_StatusTypeDef (*longPressCallback)(void *key);
    HAL_StatusTypeDef (*longlongPressCallback)(void *key);
    HAL_StatusTypeDef (*doubleClickCallback)(void *key);

    HAL_StatusTypeDef (*response)(void *key);

} userkey;

GPIO_PinState keystatus(uint8_t keynum);
void keyInit(userkey *key, uint8_t keynum);
void keyActionScan(userkey *key);
void keyDoubleClickCheck(userkey *key);
HAL_StatusTypeDef keyResponse(const userkey *key);

#endif