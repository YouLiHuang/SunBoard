#include "PCF8563.h"



static uint8_t BCD_To_Dec(uint8_t bcd)
{
    return ((bcd & 0x70) >> 4) * 10 + (bcd & 0x0f);
}

static uint8_t Dec_To_BCD(uint8_t dec)
{
    return ((dec / 10) << 4) + (dec % 10);
}

static HAL_StatusTypeDef PCF8563_Write(uint8_t regAddr, uint8_t data)
{
    HAL_StatusTypeDef status;
    uint8_t buf[2];
    buf[0] = regAddr;
    buf[1] = data;
    status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_WRITE_ADRESS, buf, 2, 100);
    if (status != HAL_OK)
    {
        return status;
    }
    return HAL_OK;
}

static HAL_StatusTypeDef PCF8563_Read(uint8_t regAddr, uint8_t *data)
{
    HAL_StatusTypeDef status;
    // 发送寄存器地址
    status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_WRITE_ADRESS, regAddr, 1, 100);
    if (status != HAL_OK)
    {
        return status;
    }
    // 读取寄存器数据
    status = HAL_I2C_Master_Receive(&hi2c1, SLAVE_READ_ADRESS, data, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        return status;
    }
    return HAL_OK;
}

HAL_StatusTypeDef PCF8563_set_time(uint8_t Years,
                                   uint8_t Months,
                                   uint8_t WeekDays,
                                   uint8_t Days,
                                   uint8_t hours,
                                   uint8_t mins,
                                   uint8_t seconds)
{

    HAL_StatusTypeDef status;
    uint8_t timeBuffer[7];
    timeBuffer[0] = Dec_To_BCD(seconds); // 秒
    timeBuffer[1] = Dec_To_BCD(mins);    // 分钟
    timeBuffer[2] = Dec_To_BCD(hours);   // 小时
    timeBuffer[3] = Dec_To_BCD(Days);    // 日
    timeBuffer[4] = Dec_To_BCD(WeekDays);
    timeBuffer[5] = Dec_To_BCD(Months);      // 月
    timeBuffer[6] = Dec_To_BCD(Years % 100); // 年（只取后两位）

    status = HAL_I2C_Mem_Write(&hi2c1,
                               SLAVE_WRITE_ADRESS,
                               SECONDS_ADRESS,
                               I2C_MEMADD_SIZE_8BIT,
                               timeBuffer,
                               sizeof(timeBuffer),
                               HAL_MAX_DELAY);

    return status;
}

static HAL_StatusTypeDef PCF8563_read_time(uint8_t *data)
{

    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read(&hi2c1,
                              SLAVE_READ_ADRESS,
                              SECONDS_ADRESS,
                              I2C_MEMADD_SIZE_8BIT,
                              data,
                              7,
                              HAL_MAX_DELAY);
    return status;
}

static HAL_StatusTypeDef time_updata(void *pointer)
{
    HAL_StatusTypeDef status;
    PCF8563_Controller *p = pointer;

    // 存储上次时间
    static uint8_t last_hour, current_hour;
    if (BCD_To_Dec(p->read_buffer[2]) <= 23 && BCD_To_Dec(p->read_buffer[2]) >= 0)
        last_hour = BCD_To_Dec(p->read_buffer[2]);
    // 更新当前时间
    status = PCF8563_read_time(p->read_buffer);
    if (status != HAL_OK)
        return status;
    if (BCD_To_Dec(p->read_buffer[2]) <= 23 && BCD_To_Dec(p->read_buffer[2]) >= 0)
        current_hour = BCD_To_Dec(p->read_buffer[2]);

    if (current_hour == 0) // 正常溢出
    {
        p->data_time->Hour = 0;
    }

    // 修正
    if (current_hour - last_hour == 1) // 正常增长
    {
        p->data_time->Hour = current_hour;
        p->timehourFlash = HOUR_UPDATA;
    }
    else if (current_hour == last_hour) // 维持原时间
    {
        p->data_time->Hour = current_hour;
    }

    /*更新时间和日期*/
    p->data_time->Second = BCD_To_Dec(p->read_buffer[0]);
    p->data_time->Min = BCD_To_Dec(p->read_buffer[1]);
    p->data_time->Day = BCD_To_Dec(p->read_buffer[3]);
    p->data_time->WeekDays = BCD_To_Dec(p->read_buffer[4]);
    p->data_time->Month = BCD_To_Dec(p->read_buffer[5]);
    p->data_time->Year = BCD_To_Dec(p->read_buffer[6]);

    return status;
}

static HAL_StatusTypeDef PCF8563_init(PCF8563_Controller *pcf_ctrl)
{
    // uint64_t readbuffer[7];
    //  Data_Read_From_Flash(FLASH_USER_START_ADDR, readbuffer, 7);

    pcf_ctrl->data_time = (data_time *)malloc(sizeof(data_time));
    pcf_ctrl->data_time->Year = 24;
    pcf_ctrl->data_time->Month = 9;
    pcf_ctrl->data_time->Day = 1;
    pcf_ctrl->data_time->Hour = 0;
    pcf_ctrl->data_time->Second = 0;
    pcf_ctrl->data_time->WeekDays = 1;
    pcf_ctrl->read_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 16);
    pcf_ctrl->timer = &htim1;
    pcf_ctrl->timeFlash = TIME_WAIT;
    pcf_ctrl->timehourFlash = HOUR_IDEAL;
    pcf_ctrl->time_updata = time_updata;

    HAL_StatusTypeDef status;
    /*写模式：发送要读的寄存器地址*/
    status = PCF8563_Write(CONTROL_STATUS_1, 0x00);
    if (status != HAL_OK)
    {
        return status;
    }
    // 年、月、工作日、日、時、分、秒
    /*
    status = PCF8563_set_time(user_time.Year,
                              user_time.Month,
                              user_time.WeekDays,
                              user_time.Day,
                              user_time.Hour,
                              user_time.Min,
                              user_time.Second);
    if (status != HAL_OK)
    {
        return status;
    }
    */

    // 完成初次时间更新
    status = PCF8563_read_time(pcf_ctrl->read_buffer);
    if (status != HAL_OK)
    {
        return status;
    }

    // 开启时间获取定时器
    status = HAL_TIM_Base_Start_IT(pcf_ctrl->timer);
    if (status != HAL_OK)
    {
        return status;
    }

    return status;
}

PCF8563_Controller *newPCF8563()
{
    PCF8563_Controller *PCF8563 = (PCF8563_Controller *)malloc(sizeof(PCF8563_Controller));

    if (PCF8563 != NULL)
    {
        PCF8563_init(PCF8563);
        return PCF8563;
    }
}

bool deletePCF8563(PCF8563_Controller *pcf)
{
    if (pcf != NULL)
    {
        free(pcf->read_buffer);
        free(pcf->data_time);
        free(pcf);
        return true;
    }
    return false;
}