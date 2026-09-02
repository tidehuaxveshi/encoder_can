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
}
uint8_t data[32] = {0};
uint8_t device_state = 0;
uint32_t motor_count = 0;
uint32_t count = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    time_user = get_time();
    if (htim->Instance == TIM2)
    {
        motor_count = (motor_count + 1) % 5;
        count = (count + 1) % 100;
        FDCAN_SendData(data, 0x12, 8, CAN_ID_STD, CAN_RTR_DATA, 0);
    }
}