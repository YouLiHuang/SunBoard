#include "TM1650.h"

static uint8_t LedCode[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; // 0~9
static uint8_t write_buffer[4];

static void TM1650_write_param(uint8_t param, uint16_t adress)
{
    write_buffer[0] = param;
    HAL_I2C_Master_Transmit(&hi2c2, adress, write_buffer, 1, 100);
}

void TM1650_display_pos(uint8_t pos, uint8_t num)
{
    if (pos <= 3)
    {
        if (num <= 9)
            TM1650_write_param(LedCode[num], pos * 2 + DIG1_ADRESS);
    }
}

void TM1650_clear(cursorPos pos, uint8_t all)
{
    if (all != 0)
    {
        TM1650_write_param(0x00, DIG1_ADRESS);
        TM1650_write_param(0x00, DIG2_ADRESS);
        TM1650_write_param(0x00, DIG3_ADRESS);
        TM1650_write_param(0x00, DIG4_ADRESS);
    }
    else
    {
        switch (pos)
        {
        case FIRST:
            TM1650_write_param(0x00, DIG1_ADRESS);
            break;
        case SECOND:
            TM1650_write_param(0x00, DIG2_ADRESS);
            break;
        case THIRD:
            TM1650_write_param(0x00, DIG3_ADRESS);
            break;
        case FOURTH:
            TM1650_write_param(0x00, DIG4_ADRESS);
            break;
        default:
            break;
        }
    }
}

static void displayOnOff(displayStatus status)
{
    if (status == DSPOFF)
    {
        TM1650_write_param(0x00, DIG1_ADRESS);
        TM1650_write_param(0x00, DIG2_ADRESS);
        TM1650_write_param(0x00, DIG3_ADRESS);
        TM1650_write_param(0x00, DIG4_ADRESS);
    }
    else
    {
        TM1650_write_param(TM1650_BRIGHT3, SLAVE_ADRESS); // 设置初始亮度
        TM1650_write_param(LedCode[0], DIG1_ADRESS);
        TM1650_write_param(LedCode[0], DIG2_ADRESS);
        TM1650_write_param(LedCode[0], DIG3_ADRESS);
        TM1650_write_param(LedCode[0], DIG4_ADRESS);
    }
}

static void cursorOnOff(void *TM1650, displayStatus OnOff)
{
    tm1650 *p = TM1650;
    if (OnOff == DSPON && p->dspStatus == DSPON)
    {
        HAL_TIM_Base_Start_IT(p->timer);
        p->cursor = FIRST; // 光标复位
    }
    else
    {
        HAL_TIM_Base_Stop_IT(p->timer);
        p->TM1650_writeCursor(p->cursor, p->numList[p->cursor]); // 还原当前位置
        p->cursor = CURSOROFF;
    }
}

static void cursorMove(void *TM1650, CurDir dir)
{
    tm1650 *p = TM1650;
    if (p->dspStatus == DSPON && p->cursor != CURSOROFF)
    {
        p->TM1650_writeCursor(p->cursor, p->numList[p->cursor]); // 还原当前位置
        if (dir == LEFT)
        {

            if (p->cursor == FIRST)
            {
                p->cursor = FOURTH;
            }
            else
            {
                p->cursor--;
            }
        }
        else if (dir == RIGHT)
        {
            if (p->cursor == FOURTH)
            {
                p->cursor = FIRST;
            }
            else
            {
                p->cursor++;
            }
        }
    }
}

static void writeCursor(cursorPos pos, uint8_t num)
{
    switch (pos)
    {
    case FIRST:
        TM1650_write_param(LedCode[num], DIG1_ADRESS);
        break;
    case SECOND:
        TM1650_write_param(LedCode[num], DIG2_ADRESS);
        break;
    case THIRD:
        TM1650_write_param(LedCode[num], DIG3_ADRESS);
        break;
    case FOURTH:
        TM1650_write_param(LedCode[num], DIG4_ADRESS);
        break;
    case CURSOROFF:
        break;

    default:
        break;
    }
}

static void cursor_value_set(void *TM1650, CurDir dir)
{

    tm1650 *p = TM1650;

    if (p->cursor != CURSOROFF) // 光标开启状态
    {
        switch (p->cursor)
        {
        case FIRST:
        {
            if (p->numList[p->cursor] >= 2)
                p->numList[p->cursor] = 0;
            else
            {
                p->numList[p->cursor]++;
            }
            break;
        }
        case SECOND:
        {
            if (p->numList[FIRST] == 2)
            {
                if (p->numList[p->cursor] >= 3)
                {
                    p->numList[p->cursor] = 0;
                }
                else
                {
                    p->numList[p->cursor]++;
                }
            }
            else
            {
                if (p->numList[p->cursor] < 9)
                    p->numList[p->cursor]++;
                else
                {
                    p->numList[p->cursor] = 0;
                }
            }
            break;
        }

        case THIRD:
        {
            if (p->numList[p->cursor] >= 5)
            {
                p->numList[p->cursor] = 0;
            }
            else
            {
                p->numList[p->cursor]++;
            }
            break;
        }

        case FOURTH:
        {
            if (p->numList[p->cursor] >= 9)
            {
                p->numList[p->cursor] = 0;
            }
            else
            {
                p->numList[p->cursor]++;
            }
            break;
        }

        default:
            break;
        }
    }
}

static void TM1650_show_time(void *tm, uint8_t hour, uint8_t min)
{
    tm1650 *p = (tm1650 *)tm;
    TM1650_display_pos(0, hour / 10);
    p->numList[0] = hour / 10;
    TM1650_display_pos(1, hour % 10);
    p->numList[1] = hour % 10;
    TM1650_display_pos(2, min / 10);
    p->numList[2] = min / 10;
    TM1650_display_pos(3, min % 10);
    p->numList[3] = min % 10;
}

void cursor_flash_ctrl(void *TM1650)
{
    tm1650 *p = TM1650;
    if (p->cursor != CURSOROFF)
    {
        if (p->cursorFlashFlag == CURSOR_FALSH_ON)
        {
            p->cursorFlashFlag = CURSOR_FALSH_OFF;
            TM1650_clear(p->cursor, 0);
        }
        else
        {
            p->cursorFlashFlag = CURSOR_FALSH_ON;
            p->TM1650_writeCursor(p->cursor, p->numList[p->cursor]);
        }
    }
}

static void TM1650_init(tm1650 *tm1650, uint8_t init_Light)
{

    tm1650->numList = (uint8_t *)malloc(sizeof(uint8_t) * DATA_LEN);
    tm1650->numList[0] = 1;
    tm1650->numList[1] = 2;
    tm1650->numList[2] = 0;
    tm1650->numList[3] = 0;

    tm1650->timer = &htim16;
    tm1650->cursor = CURSOROFF;
    tm1650->dspStatus = DSPON;
    tm1650->TM1650_displayOnOff = displayOnOff;
    tm1650->TM1650_cursorOnOff = cursorOnOff;
    tm1650->TM1650_writeCursor = writeCursor;
    tm1650->TM1650_cursorMove = cursorMove;
    tm1650->TM1650_show_time = TM1650_show_time;
    tm1650->TM1650_cursorFlash_ctrl = cursor_flash_ctrl;
    tm1650->TM1650_cursor_value_set = cursor_value_set;
    tm1650->cursorFlashFlag = 0;

    TM1650_write_param(init_Light, SLAVE_ADRESS); // 设置初始亮度
    TM1650_write_param(LedCode[tm1650->numList[0]], DIG1_ADRESS);
    TM1650_write_param(LedCode[tm1650->numList[1]], DIG2_ADRESS);
    TM1650_write_param(LedCode[tm1650->numList[2]], DIG3_ADRESS);
    TM1650_write_param(LedCode[tm1650->numList[3]], DIG4_ADRESS);
}

tm1650 *newTM1650(uint8_t init_Light)
{
    tm1650 *tm = (tm1650 *)malloc(sizeof(tm1650));
    if (tm != NULL)
    {
        TM1650_init(tm, init_Light);
        return tm;
    }
}

bool deleteTM1650(tm1650 *tm)
{
    if (tm != NULL)
    {
        free(tm->numList);
        free(tm);
    }

    return false;
}