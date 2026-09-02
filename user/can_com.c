#include "can_com.h"

/**
 * @brief can init setting
 * @return null
 * @note the clock frequence of can must not be changed in cubemx
 */
void can_setting_init(CAN_HandleTypeDef *hcan)
{
	hcan->Instance = CAN1;
	hcan->Init.Prescaler = 16;
	hcan->Init.Mode = CAN_MODE_NORMAL;
	hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan->Init.TimeSeg1 = CAN_BS1_1TQ;
	hcan->Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan->Init.TimeTriggeredMode = DISABLE;
	hcan->Init.AutoBusOff = DISABLE;
	hcan->Init.AutoWakeUp = DISABLE;
	hcan->Init.AutoRetransmission = DISABLE;
	hcan->Init.ReceiveFifoLocked = DISABLE;
	hcan->Init.TransmitFifoPriority = DISABLE;

	if (HAL_CAN_Init(hcan) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief update can rx filter configration in mask mode
 * @param tx pointer to can tx header
 * @return null
 * @note null
 */
void can_mask_filter_add(uint32_t id, uint32_t mask_setting, uint32_t fifo_setting, uint32_t filter_bank, uint32_t STD_EXT, uint32_t RTR)
{
	CAN_FilterTypeDef filter_config;

	filter_config.FilterBank = filter_bank;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.FilterFIFOAssignment = fifo_setting;
	filter_config.FilterActivation = ENABLE;
	filter_config.SlaveStartFilterBank = 14;
	if (STD_EXT == CAN_ID_STD)
	{
		uint32_t filter_id;
		uint32_t filter_mask;
		filter_id = (id << 21);
		if (RTR == CAN_RTR_REMOTE)
		{
			filter_id |= (1U << 1);
		}
		filter_mask = (mask_setting << 21);
		filter_mask |= (1U << 2);
		filter_config.FilterIdHigh = (uint16_t)(filter_id >> 16);
		filter_config.FilterIdLow = (uint16_t)(filter_id & 0xFFFF);
		filter_config.FilterMaskIdHigh = (uint16_t)(filter_mask >> 16);
		filter_config.FilterMaskIdLow = (uint16_t)(filter_mask & 0xFFFF);
	}
	else
	{
		uint32_t filter_id;
		uint32_t filter_mask;
		filter_id = (id << 3);
		filter_id |= (1U << 2);
		if (RTR == CAN_RTR_REMOTE)
		{
			filter_id |= (1U << 1);
		}
		filter_mask = (mask_setting << 3);
		filter_mask |= (1U << 2);
		filter_config.FilterIdHigh = (uint16_t)(filter_id >> 16);
		filter_config.FilterIdLow = (uint16_t)(filter_id & 0xFFFF);
		filter_config.FilterMaskIdHigh = (uint16_t)(filter_mask >> 16);
		filter_config.FilterMaskIdLow = (uint16_t)(filter_mask & 0xFFFF);
	}

	if (HAL_CAN_ConfigFilter(&hcan, &filter_config) != HAL_OK)
	{
		Error_Handler();
	}
}

uint32_t FDCAN_GetDLC(uint8_t length)
{
	if (length <= 8)
	{
		return length;
	}

	return 8;
}

HAL_StatusTypeDef FDCAN_SendData(uint8_t *data, uint32_t id, uint32_t length, uint32_t STD_EXT, uint32_t BRS, uint32_t FD)
{
	CAN_TxHeaderTypeDef header;
	HAL_StatusTypeDef status;
	uint32_t tx_mailbox;
	CAN_HandleTypeDef *tx = &hcan;
	if (data == NULL)
	{
		return HAL_ERROR;
	}
	if (length > 8)
	{
		return HAL_ERROR;
	}
	if (HAL_CAN_GetTxMailboxesFreeLevel(tx) == 0U)
	{
		return HAL_BUSY;
	}
	if (STD_EXT == CAN_ID_STD)
	{
		header.StdId = id;
		header.ExtId = 0;
		header.IDE = CAN_ID_STD;
	}
	else
	{
		header.StdId = 0;
		header.ExtId = id;
		header.IDE = CAN_ID_EXT;
	}
	header.RTR = CAN_RTR_DATA;
	header.DLC = FDCAN_GetDLC(length);
	header.TransmitGlobalTime = DISABLE;
	status = HAL_CAN_AddTxMessage(tx, &header, data, &tx_mailbox);
	return status;
}

void can_init(void)
{
	can_setting_init(&hcan);
	can_mask_filter_add(0, 0, CAN_FILTER_FIFO0, 0, CAN_ID_STD, CAN_RTR_DATA);
	can_mask_filter_add(0, 0, CAN_FILTER_FIFO0, 1, CAN_ID_EXT, CAN_RTR_DATA);
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}