#ifndef CAN_COM_H_
#define CAN_COM_H_

#include "can.h"
#include "stdint.h"
#include "stdio.h"

HAL_StatusTypeDef FDCAN_SendData(uint8_t *data, uint32_t id, uint32_t length, uint32_t STD_EXT);
void can_init(void);
void can_mask_filter_add(uint32_t id, uint32_t mask_setting, uint32_t fifo_setting, uint32_t filter_bank, uint32_t STD_EXT, uint32_t RTR);

#endif /* CAN_COM_H_ */