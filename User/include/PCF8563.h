#ifndef __PCF8563_H__
#define __PCF8563_H__

#include "i2c.h"
#include "tim.h"
#include "myFlash.h"

#define SLAVE_WRITE_ADRESS 0xA2
#define SLAVE_READ_ADRESS 0xA3

#define CONTROL_STATUS_1 0x00
#define CONTROL_STATUS_2 0x01

#define SECONDS_ADRESS 0x02
#define MINUTES_ADRESS 0x03
#define HOURS_ADRESS 0x04
#define DAYS_ADRESS 0x05
#define WEEKDAYS_ADRESS 0x06
#define MONTHS_ADRESS 0x07
#define YEARS_ADRESS 0x08

typedef struct
{
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t WeekDays;
    uint8_t Hour;
    uint8_t Min;
    uint8_t Second;
} data_time;

typedef enum{
    TIME_OK=0x00,
    TIME_WAIT
}TimeFlushFlag;

typedef enum{
    HOUR_UPDATA=0x00,
    HOUR_IDEAL
}TimeHourFlash;

typedef enum{
    RESET_MODE=0x00,
    FURST_MODE
}Init_Mode;

typedef struct
{
    TIM_HandleTypeDef *timer;
    TimeFlushFlag timeFlash;
    TimeHourFlash timehourFlash;
    data_time *data_time;
    uint8_t *read_buffer;

    HAL_StatusTypeDef (*time_updata)(void *pointer);

} PCF8563_Controller;

HAL_StatusTypeDef PCF8563_init(PCF8563_Controller *pcf_ctrl);
HAL_StatusTypeDef PCF8563_set_time(uint8_t Years,
                                   uint8_t Months,
                                   uint8_t WeekDays,
                                   uint8_t Days,
                                   uint8_t hours,
                                   uint8_t mins,
                                   uint8_t seconds);

#endif