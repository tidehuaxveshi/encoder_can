#include "encoder.h"
encoder_t encoder;

void spi_init(SPI_HandleTypeDef *init_channel)
{
  init_channel->Init.Mode = SPI_MODE_MASTER;
  init_channel->Init.Direction = SPI_DIRECTION_2LINES;
  init_channel->Init.DataSize = SPI_DATASIZE_8BIT;
  init_channel->Init.CLKPolarity = SPI_POLARITY_HIGH;
  init_channel->Init.CLKPhase = SPI_PHASE_2EDGE;
  init_channel->Init.NSS = SPI_NSS_SOFT;
  init_channel->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  init_channel->Init.FirstBit = SPI_FIRSTBIT_MSB;
  init_channel->Init.TIMode = SPI_TIMODE_DISABLE;
  init_channel->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  init_channel->Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(init_channel) != HAL_OK)
  {
    Error_Handler();
  }
}
void encoder_init(void)
{
  spi_init(ENCODER_SPI);
}

void MT6816_Get_Angle(encoder_t *encoder_read)
{

  static uint8_t data_t[2] = {(0x80 | 0x03), (0x80 | 0x04)};
  static uint8_t data_r[2];
   uint16_t sample_data;
  static bool pc_flag;
  static uint8_t h_count;

  for (uint8_t i = 0; i < 3; i++)
  {

    HAL_GPIO_WritePin(ENCODER_SPI_CS_PORT, ENCODER_SPI_CS_PIN, 0);
    // HAL_SPI_TransmitReceive(ENCODER_SPI, (uint8_t*)&data_t[0], (uint8_t*)&data_r[0], 1, 10);

    HAL_SPI_Transmit(ENCODER_SPI, (uint8_t *)&data_t[0], 1, 1000);
    HAL_SPI_Receive(ENCODER_SPI, (uint8_t *)&data_r[0], 1, 1000);

    HAL_GPIO_WritePin(ENCODER_SPI_CS_PORT, ENCODER_SPI_CS_PIN, 1);
    HAL_GPIO_WritePin(ENCODER_SPI_CS_PORT, ENCODER_SPI_CS_PIN, 0);
    // HAL_SPI_TransmitReceive(ENCODER_SPI, (uint8_t*)&data_t[1], (uint8_t*)&data_r[1], 1, 10);

    HAL_SPI_Transmit(ENCODER_SPI, (uint8_t *)&data_t[0] + 1, 1, 1000);
    HAL_SPI_Receive(ENCODER_SPI, (uint8_t *)&data_r[0] + 1, 1, 1000);

    HAL_GPIO_WritePin(ENCODER_SPI_CS_PORT, ENCODER_SPI_CS_PIN, 1);

    sample_data = ((data_r[0] & 0x00FF) << 8) | (data_r[1] & 0x00FF);

    h_count = 0;
    for (uint8_t j = 0; j < 16; j++)
    {
      if (sample_data & (0x0001 << j))
        h_count++;
    }
    if (h_count & 0x01)
    {
      pc_flag = false;
    }
    else
    {
      pc_flag = true;
      break;
    }
  }

  if (pc_flag)
  {
    encoder_read->angle_last = encoder_read->angle;
    encoder_read->angle = (int16_t)(sample_data >> 2);
    encoder_read->angle_velocity_last = encoder_read->angle_velocity;
    if (encoder_read->angle_last - encoder_read->angle > (1 << (ENCODER_ANGLE_DIV - 1)))
    {
      encoder_read->cycle++;
      encoder_read->angle_velocity = -((1 << ENCODER_ANGLE_DIV) - encoder_read->angle + encoder_read->angle);
    }
    else if (encoder_read->angle_last - encoder_read->angle < -(1 << (ENCODER_ANGLE_DIV - 1)))
    {
      encoder_read->cycle--;
      encoder_read->angle_velocity = ((1 << ENCODER_ANGLE_DIV) - encoder_read->angle_last + encoder_read->angle_last);
    }
    else
    {
      encoder_read->angle_velocity = encoder_read->angle - encoder_read->angle_last;
    }
    encoder_read->angle_absolute = (int32_t)(encoder_read->cycle * (1 << ENCODER_ANGLE_DIV)) + (int32_t)(encoder_read->angle);
    encoder_read->angle_absolute = encoder_read->angle_absolute;
    encoder_read->angle_acceleration = encoder_read->angle_velocity - encoder_read->angle_velocity_last;
   

    encoder_read->no_mag_flag = (bool)(sample_data & (0x0001 << 1));
  }
}

