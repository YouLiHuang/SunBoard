#include "motor.h"

static void set_current_angle(void *pointer, uint16_t angle)
{

    Motor_Controller *p = pointer;
    if (angle <= 360)
    {
        p->Current_Angle = (uint32_t)angle * 4096 / 360;
    }
}

static void set_target_angle(void *pointer, uint16_t angle)
{

    Motor_Controller *p = pointer;
    if (angle <= 360)
    {
        p->Target_Angle = (uint32_t)angle * 4096 / 360;
        p->motor_status = MOTOR_BUSY;
        HAL_TIM_Base_Start_IT(p->timer); // 开启电机定时器
    }
}

static void set_total_angle(void *pointer, uint16_t angle)
{

    Motor_Controller *p = pointer;
    if (angle <= 360)
    {
        p->total_angle = angle;
    }
}

static uint16_t get_current_angle(void *pointer)
{
    Motor_Controller *p = pointer;
    uint16_t angle = (uint32_t)p->Current_Angle * 360 / 4096;
    return angle;
}

/* static uint16_t get_target_angle(void *pointer)
{
    Motor_Controller *p = pointer;
    p = p;
    uint16_t angle = (uint32_t)p->Target_Angle * 360 / 4096;
    return angle;
} */

static void motor_left_correct(void *pointer, uint8_t angle_step)
{
    Motor_Controller *p = pointer;
    p->Dir = LEFT_DIR;

    p->set_current_angle(p, 0);
    p->set_target_angle(p, angle_step);
}

static void motor_right_correct(void *pointer, uint8_t angle_step)
{
    Motor_Controller *p = pointer;
    p->Dir = RIGHT_DIR;

    p->set_current_angle(p, 0);
    p->set_target_angle(p, angle_step);
}

static void motor_stop(void *pointer)
{
    Motor_Controller *p = pointer;
    HAL_TIM_Base_Stop_IT(p->timer);
    p->set_target_angle(p, 0);
    p->set_current_angle(p, 0);
}

static void motor_reset(void *pointer)
{
    Motor_Controller *p = pointer;
    p->Dir = LEFT_DIR;
    p->set_current_angle(p, 0);
    p->set_target_angle(p, 90);
    HAL_TIM_Base_Start_IT(p->timer);
    p->set_total_angle(p, 0);
}

static void motor_status_led_ctrl(void *pointer, GPIO_PinState state)
{
    Motor_Controller *p = pointer;
    p = p;
    HAL_GPIO_WritePin(MotorLed_GPIO_Port, MotorLed_Pin, state);
}

static void motor_wave_control(void *pointer)
{
    Motor_Controller *p = pointer;

#if FOURTH_STEP == 1
    if (p->Dir == LEFT_DIR)
    {
        if (p->Current_Angle < p->Target_Angle)
        {
            p->Current_Angle++;
            p->Current_Step++;
            p->Current_Step %= 4;

            switch (p->Current_Step)
            {
            case 1:
                MOTORA_H;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 2:
                MOTORA_L;
                MOTORB_H;
                MOTORC_H;
                MOTORD_L;
                break;
            case 3:
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_H;
                break;
            case 0:
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;

            default:
                break;
            }
        }
        else
        {
            p->motor_status = MOTOR_READY;
            MOTORA_L;
            MOTORB_L;
            MOTORC_L;
            MOTORD_L;
            HAL_TIM_Base_Stop_IT(p->timer);
        }
    }
    else if (p->Dir == RIGHT_DIR)
    {
        if (p->Current_Angle < p->Target_Angle)
        {
            p->Current_Angle++;
            p->Current_Step++;
            p->Current_Step %= 4;

            switch (p->Current_Step)
            {
            case 0:
                MOTORA_H;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 3:
                MOTORA_L;
                MOTORB_H;
                MOTORC_H;
                MOTORD_L;
                break;
            case 2:
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_H;
                break;
            case 1:
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;

            default:
                break;
            }
        }
        else
        {
            p->motor_status = MOTOR_READY;
            MOTORA_L;
            MOTORB_L;
            MOTORC_L;
            MOTORD_L;
            HAL_TIM_Base_Stop_IT(p->timer);
        }
    }

#endif

#if EIGHT_STEP == 1
    if (p->Dir == LEFT_DIR)
    {
        if (p->Current_Angle < p->Target_Angle)
        {
            p->Current_Angle++;
            p->Current_Step++;
            p->Current_Step %= 8;

            switch (p->Current_Step)
            {
            case 1: /*a*/
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_L;
                break;
            case 2: /*ab*/
                MOTORA_H;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 3: /*b*/
                MOTORA_L;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 4: /*bc*/
                MOTORA_L;
                MOTORB_H;
                MOTORC_H;
                MOTORD_L;
                break;
            case 5: /*c*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_L;
                break;
            case 6: /*cd*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_H;
                break;
            case 7: /*d*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;
            case 0: /*da*/
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;

            default:
                break;
            }
        }
        else
        {
            p->motor_status = MOTOR_READY;
            MOTORA_L;
            MOTORB_L;
            MOTORC_L;
            MOTORD_L;
            HAL_TIM_Base_Stop_IT(p->timer);
        }
    }
    else if (p->Dir == RIGHT_DIR)
    {
        if (p->Current_Angle < p->Target_Angle)
        {
            p->Current_Angle++;
            p->Current_Step++;
            p->Current_Step %= 8;

            switch (p->Current_Step)
            {
            case 0: /*a*/
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_L;
                break;
            case 7: /*ab*/
                MOTORA_H;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 6: /*b*/
                MOTORA_L;
                MOTORB_H;
                MOTORC_L;
                MOTORD_L;
                break;
            case 5: /*bc*/
                MOTORA_L;
                MOTORB_H;
                MOTORC_H;
                MOTORD_L;
                break;
            case 4: /*c*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_L;
                break;
            case 3: /*cd*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_H;
                MOTORD_H;
                break;
            case 2: /*d*/
                MOTORA_L;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;
            case 1: /*da*/
                MOTORA_H;
                MOTORB_L;
                MOTORC_L;
                MOTORD_H;
                break;

            default:
                break;
            }
        }
        else
        {
            p->motor_status = MOTOR_READY;
            MOTORA_L;
            MOTORB_L;
            MOTORC_L;
            MOTORD_L;
            HAL_TIM_Base_Stop_IT(p->timer);
        }
    }

#endif
}

static void Motor_Ctrl_init(Motor_Controller *motor_ctrl)
{

    motor_ctrl->timer = &htim17;
    motor_ctrl->Current_Step = 0;
    motor_ctrl->mode = MOTOR_IDEAL_MODE;
    motor_ctrl->motor_status = MOTOR_READY;
    motor_ctrl->Current_Angle = 0;
    motor_ctrl->Target_Angle = motor_ctrl->Current_Angle;
    motor_ctrl->total_angle = 0;
    motor_ctrl->Dir = RIGHT_DIR;

    motor_ctrl->set_current_angle = set_current_angle;
    motor_ctrl->set_target_angle = set_target_angle;
    motor_ctrl->set_total_angle = set_total_angle;
    motor_ctrl->get_current_angle = get_current_angle;
    motor_ctrl->motor_left_correct = motor_left_correct;
    motor_ctrl->motor_right_correct = motor_right_correct;
    motor_ctrl->motor_stop = motor_stop;
    motor_ctrl->motor_reset = motor_reset;
    motor_ctrl->status_led_ctrl = motor_status_led_ctrl;
    motor_ctrl->motor_wave_ctrl = motor_wave_control;

    MOTORA_L;
    MOTORB_L;
    MOTORC_L;
    MOTORD_L;
    HAL_TIM_Base_Stop_IT(motor_ctrl->timer);
}

Motor_Controller *newMotorCtrl()
{
    Motor_Controller *motor_crtl = (Motor_Controller *)malloc(sizeof(Motor_Controller));
    if (motor_crtl != NULL)
    {
        Motor_Ctrl_init(motor_crtl);
        return motor_crtl;
    }
}

bool delete_MotorCtrl(Motor_Controller *ctrl)
{
    if (ctrl != NULL)
    {
        free(ctrl);
        return true;
    }

    return false;
}