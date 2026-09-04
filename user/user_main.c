#include "user_main.h"
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
float t1=0;
float t2=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    get_time();
    if (htim->Instance == htim1.Instance)
    {
        uint8_t data[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
        t1=get_time();
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
        FDCAN_SendData(data, 0x11, 8, CAN_ID_STD);
        MT6816_Get_Angle(&encoder);
        t2=get_time();
    }
}