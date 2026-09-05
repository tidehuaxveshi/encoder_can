#include "user_main.h"
#include "string.h"
uint8_t encoder_id = 1;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data);
}

void user_init(void)
{
    time_init();
    can_init();
    encoder_init();
}
float t1 = 0;
float t2 = 0;
uint16_t led_cnt_sub = 0;
uint8_t led_cnt = 0;
uint8_t bright_period = 200;
uint8_t wait_period = 2;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    t1 = get_time();
    if (htim->Instance == htim1.Instance)
    {
        uint8_t data[4];
        MT6816_Get_Angle(&encoder);
        float angle = encoder.angle;
        memcpy(data, &angle, 4);
        FDCAN_SendData(data, encoder_id, 4, CAN_ID_STD);

        // led thread
        led_cnt_sub++;
        if (led_cnt_sub >= 2 * bright_period)
        {
            led_cnt_sub = 0;
            led_cnt++;
        }
        else if (led_cnt_sub >= bright_period)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
        }
        else if (led_cnt < encoder_id)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
        }

        if (led_cnt >= encoder_id + wait_period + 1)
        {
            led_cnt = 0;
        }
    }
    t2 = get_time();
}