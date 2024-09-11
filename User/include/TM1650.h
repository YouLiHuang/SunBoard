#ifndef TM1650_H_
#define TM1650_H_

#include "i2c.h"
#include "tim.h"

// 显示参数
#define TM1650_BRIGHT1 0x11 /*一级亮度，打开LED显示*/
#define TM1650_BRIGHT2 0x21 /*二级亮度，打开LED显示*/
#define TM1650_BRIGHT3 0x31 /*三级亮度，打开LED显示*/
#define TM1650_BRIGHT4 0x41 /*四级亮度，打开LED显示*/
#define TM1650_BRIGHT5 0x51 /*五级亮度，打开LED显示*/
#define TM1650_BRIGHT6 0x61 /*六级亮度，打开LED显示*/
#define TM1650_BRIGHT7 0x71 /*七级亮度，打开LED显示*/
#define TM1650_BRIGHT8 0x01 /*八级亮度，打开LED显示*/
#define TM1650_DSP_OFF 0x00 /*关闭LED显示*/

// 数码管位选
#define TM1650_DIG1 0
#define TM1650_DIG2 1
#define TM1650_DIG3 2
#define TM1650_DIG4 3

// 模拟从机地址
#define SLAVE_ADRESS 0x48
#define DIG1_ADRESS 0x68
#define DIG2_ADRESS 0x6A
#define DIG3_ADRESS 0x6C
#define DIG4_ADRESS 0x6E

typedef enum
{
    CURSOROFF = 0xff,
    FIRST = 0x00,
    SECOND = 0x01,
    THIRD = 0x02,
    FOURTH = 0x03
} cursorPos;

typedef enum
{
    DSPON = 0x00,
    DSPOFF = 0x01
} displayStatus;

typedef enum
{
    LEFT = 0x00,
    RIGHT = 0x01
} CurDir;

typedef enum
{
    CURSOR_FALSH_ON = 0x00,
    CURSOR_FALSH_OFF = 0x01

} CursorFlashFlag;

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint8_t *numList;
    cursorPos cursor;
    displayStatus dspStatus;
    CursorFlashFlag cursorFlashFlag;

    void (*TM1650_displayOnOff)(displayStatus status);
    void (*TM1650_cursorOnOff)(void *TM1650, displayStatus OnOff);
    void (*TM1650_writeCursor)(cursorPos pos, uint8_t num);
    void (*TM1650_cursorMove)(void *TM1650, CurDir dir);
    void (*TM1650_cursor_value_set)(void *TM1650);
    void (*TM1650_cursorFlash_ctrl)(void *TM1650);
    void (*TM1650_show_time)(uint8_t hour, uint8_t min);

} tm1650;

void TM1650_init(tm1650 *tm1650, uint8_t param);
void TM1650_display_pos(uint8_t pos, uint8_t num);
void TM1650_clear(cursorPos pos, uint8_t all);

#endif // TM1650_H_