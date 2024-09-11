#include "key.h"

extern tm1650 *TM1650;
extern PCF8563_Controller *Pcf8563_ctrl;
extern Motor_Controller *Motor_Ctrl;
extern MyUart myuart;

static HAL_StatusTypeDef keyRespnse(void *poniter)
{
    userkey *key = (userkey *)poniter;
    if (key->flag == SINGLE_CLICKED)
    {
        key->singleClickedCallback(poniter);
    }
    else if (key->flag == DOUBLE_CLICKED)
    {
        key->doubleClickCallback(poniter);
    }
    else if (key->flag == LONG_PRESS)
    {
        key->longPressCallback(poniter);
    }
    else if (key->flag == LONG_LONG_PRESS)
    {
        key->longlongPressCallback(poniter);
    }

    return HAL_OK;
}

static HAL_StatusTypeDef singleClickedCallback(void *pointer)
{

    userkey *key = pointer;
    key->flag = DEFAULT_STATUS;
    key->TaskStatus = KEY_BUSY;
    /*实现动作*/
    /*...*/
    switch (key->keynum)
    {
    case 0: /*时间设定*/
    {
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_stop(&Motor_Ctrl);
        }
        TM1650->TM1650_cursor_value_set(&TM1650);
        break;
    }

    case 1: /*时间设定：左移 电机设定：左转*/
    {
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_left_correct(&Motor_Ctrl, SLOW_STEP);
        }
        if (TM1650->cursor != CURSOROFF) // 光标开启状态
        {
            TM1650->TM1650_cursorMove(&TM1650, LEFT);
        }
        break;
    }

    case 2: /*电机设定：右转 时间设定：右移*/
    {
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_right_correct(&Motor_Ctrl, SLOW_STEP);
        }
        if (TM1650->cursor != CURSOROFF) // 光标开启状态
        {
            TM1650->TM1650_cursorMove(&TM1650, RIGHT);
        }
        break;
    }

    default:
        break;
    }
    /*完成动作*/
    key->TaskStatus = KEY_IDEAL;
    return HAL_OK;
}

static HAL_StatusTypeDef longPressCallback(void *poniter)
{

    /*实现长按动作*/
    userkey *key = poniter;
    key->flag = DEFAULT_STATUS;
    key->TaskStatus = KEY_BUSY;
    /*实现动作*/
    /*...*/
    switch (key->keynum)
    {
    case 0: /*时间设定*/
    {
        /*退出电机设定*/
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->set_current_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->set_target_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->set_total_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->status_led_ctrl(&Motor_Ctrl, GPIO_PIN_RESET);
            Motor_Ctrl->mode = MOTOR_IDEAL_MODE;
            break;
        }
        /*时间设定模式入口*/
        if (TM1650->cursor == CURSOROFF) // 光标off状态则开启光标，进入时间设定模式
        {
            HAL_TIM_Base_Stop_IT(Pcf8563_ctrl->timer); // 关闭时间获取定时器
            /*获取当前时间*/
            Pcf8563_ctrl->time_updata(&Pcf8563_ctrl);
            TM1650->TM1650_cursorOnOff(&TM1650, DSPON);
            break;
        }
        else
        {
            HAL_TIM_Base_Stop_IT(Pcf8563_ctrl->timer); // 停止时间获取定时器
            TM1650->TM1650_cursorOnOff(&TM1650, DSPOFF);
            /*获取当前日期*/
            Pcf8563_ctrl->time_updata(&Pcf8563_ctrl);
            uint8_t Year = Pcf8563_ctrl->data_time->Year;
            uint8_t Month = Pcf8563_ctrl->data_time->Month;
            uint8_t Day = Pcf8563_ctrl->data_time->Day;
            uint8_t WeekDay = Pcf8563_ctrl->data_time->WeekDays;
            /*获取按键设定的时间*/
            uint8_t hour = TM1650->numList[0] * 10 + TM1650->numList[1];
            uint8_t min = TM1650->numList[2] * 10 + TM1650->numList[3];
            /*设定新的时间*/
            PCF8563_set_time(Year,
                             Month,
                             WeekDay,
                             Day,
                             hour,
                             min,
                             0);
            /*刷新时间*/
            Pcf8563_ctrl->time_updata(&Pcf8563_ctrl);
            TM1650->TM1650_show_time(TM1650,
                                     Pcf8563_ctrl->data_time->Hour,
                                     Pcf8563_ctrl->data_time->Min);

            // 存到flash：年、月、日、时、分、秒、星期
            /*
            if (Data_Save_To_Flash(FLASH_USER_START_ADDR, Pcf8563_ctrl->read_buffer, 7) != HAL_OK)
            {
                return HAL_ERROR;
            }
            */

            HAL_TIM_Base_Start_IT(Pcf8563_ctrl->timer); // 重新开启时间获取定时器
            break;
        }
    }

    case 1:
    {

        /*电机快速修正模式（左转）*/
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_left_correct(&Motor_Ctrl, FAST_STEP);
            break;
        }
        // 系统复位
        else
        {
            TM1650_clear(0, 4);
            HAL_Delay(500);
            HAL_NVIC_SystemReset();
            break;
        }
    }

    case 2:
    {
        /*电机快速修正模式（右转）*/
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_right_correct(&Motor_Ctrl, FAST_STEP);
            break;
        }
    }

    default:
        break;
    }
    /*完成动作*/
    key->TaskStatus = KEY_IDEAL;
    return HAL_OK;
}

static HAL_StatusTypeDef doubleClickCallback(void *poniter)
{

    /*实现双击动作*/
    userkey *key = poniter;
    key->flag = DEFAULT_STATUS;
    key->TaskStatus = KEY_BUSY;
    /*实现动作*/
    /*...*/
    switch (key->keynum)
    {
    case 0:
    {
        /*退出电机设定*/
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->set_current_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->set_target_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->set_total_angle(&Motor_Ctrl, 0);
            Motor_Ctrl->status_led_ctrl(&Motor_Ctrl, GPIO_PIN_RESET);
            Motor_Ctrl->mode = MOTOR_IDEAL_MODE;
            break;
        }
    }

    case 1:
    {
        break;
    }

    case 2:
    {
        break;
    }

    default:
        break;
    }
    /*完成动作*/
    key->TaskStatus = KEY_IDEAL;
    return HAL_OK;
}

static HAL_StatusTypeDef longlongPressCallback(void *pointer)
{

    userkey *key = pointer;
    key->TaskStatus = KEY_BUSY;
    /*实现动作*/
    /*...*/
    switch (key->keynum)
    {
    case 0:
    {
        /*电机设定模式进入/退出*/
        if (Motor_Ctrl->mode == MOTOR_IDEAL_MODE)
        {
            Motor_Ctrl->mode = MOTOR_CORRECT_MODR;
            Motor_Ctrl->status_led_ctrl(&Motor_Ctrl, GPIO_PIN_SET);
        }
        break;
    }
    case 1:
    {
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_left_correct(&Motor_Ctrl, SLOW_STEP);
        }
        break;
    }
    case 2:
    {
        if (Motor_Ctrl->mode == MOTOR_CORRECT_MODR)
        {
            Motor_Ctrl->motor_right_correct(&Motor_Ctrl, SLOW_STEP);
        }
        break;
    }
    }

    /*完成动作*/
    key->TaskStatus = KEY_IDEAL;
    return HAL_OK;
}
GPIO_PinState keystatus(uint8_t keynum)
{
    switch (keynum)
    {
    case 0:
    {
        return HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
        break;
    }
    case 1:
    {
        return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        break;
    }
    case 2:
    {
        return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
        break;
    }

    default:
        return GPIO_PIN_RESET;
        break;
    }
}

void keyActionScan(userkey *key)
{
    if (keystatus(key->keynum) == 0) // 电平监测
    {
        key->score++;
        if (key->score >= 5 && key->score <= 50)
            key->action = KEY_PRESS; // 短按
        if (key->score >= 100 && key->score <= 200)
            key->action = KEY_LONG_PRESS; // 长按
        if (key->score >= 300)
        {
            key->action = KEY_LONG_LONG_PRESS;
            key->flag = LONG_LONG_PRESS;
        }
    }
    else
    {
        switch (key->action)
        {
        case KEY_PRESS:
        {
            if (key->up_score++ >= 10)
            {
                key->action = KEY_RELEASE; // 一次有效的短按
                key->press_count++;
                key->score = 0;
                key->up_score = 0;
                key->flag = SINGLE_CLICKED;
            }
            break;
        }
        case KEY_LONG_PRESS:
        {
            if (key->up_score++ >= 10)
            {
                key->action = KEY_RELEASE; // 一次有效的长按
                key->score = 0;
                key->up_score = 0;
                key->flag = LONG_PRESS;
            }
            break;
        }
        case KEY_LONG_LONG_PRESS:
        {
            key->action = KEY_RELEASE; // 一次有效的超长按
            key->score = 0;
            key->up_score = 0;
            key->flag = DEFAULT_STATUS;
            break;
        }
        default:
            break;
        }
    }
}

HAL_StatusTypeDef keyResponse(const userkey *key)
{
    switch (key->flag)
    {
    case SINGLE_CLICKED:
        return key->singleClickedCallback((void *)key);
        break;
    case DOUBLE_CLICKED:
        return key->doubleClickCallback((void *)key);
        break;
    case LONG_PRESS:
        return key->longPressCallback((void *)key);
        break;
    case LONG_LONG_PRESS:
        return key->longlongPressCallback((void *)key);
        break;

    default:
        break;
    }

    return HAL_OK;
}

void keyDoubleClickCheck(userkey *key)
{
    if (key->press_count >= 2)
    {
        key->flag = DOUBLE_CLICKED;
        key->press_count = 0;
    }
    key->press_count = 0;
}

static void keyInit(userkey *key, uint8_t keynum)
{
    key->keynum = keynum;
    key->score = 0;
    key->up_score = 0;
    key->press_count = 0;
    key->action = KEY_RELEASE;
    key->flag = DEFAULT_STATUS;
    key->TaskStatus = KEY_IDEAL;
    key->singleClickedCallback = singleClickedCallback;
    key->longPressCallback = longPressCallback;
    key->doubleClickCallback = doubleClickCallback;
    key->longlongPressCallback = longlongPressCallback;
    key->response = keyRespnse;
}

userkey *newKey(uint8_t keynum)
{
    userkey *key = (userkey *)malloc(sizeof(userkey));
    if (key != NULL)
    {
        keyInit(key, keynum);
        return key;
    }

    return NULL;
}

bool delete_key(userkey *key)
{
    if (key != NULL)
    {
        free(key);
        return true;
    }
    return false;
}
