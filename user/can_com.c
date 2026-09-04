#include "can_com.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
static uint8_t TxData[8];
uint8_t FreeTxNum;
uint32_t TxMailbox = 0;


HAL_StatusTypeDef FDCAN_SendData(uint8_t *data, uint32_t id, uint32_t length, uint32_t STD_EXT)
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
	header.DLC = length;
	header.TransmitGlobalTime = DISABLE;
	status = HAL_CAN_AddTxMessage(tx, &header, data, &tx_mailbox);
	return status;
}

void can_init(void)
{
	CAN_FilterTypeDef  sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14; // meaningless
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	if (HAL_CAN_Start(&hcan) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
	
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		/* Activation Error */
		Error_Handler();
	}
	
	
	TxHeader.StdId = 0x00;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8; 
	TxHeader.TransmitGlobalTime = DISABLE;
}