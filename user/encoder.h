#ifndef ENCODER_H_
#define ENCODER_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"
#include "stm32f1xx_hal_gpio.h"
#include "spi.h"
#define LPF_FACTOR 1 / 32.0f
#define LPF_FACTOR_SHIFT 5
#define ENCODER_SPI (&hspi1)
#define ENCODER_SPI_CS_PIN GPIO_PIN_15
#define ENCODER_SPI_CS_PORT GPIOA
#define ENCODER_ANGLE_DIV 14
#define MEG_DIR 1
typedef struct e
{
    // common encoder configuration
    int16_t angle;
    int16_t angle_last;
    bool no_mag_flag; // magnetic encoder flag

    int16_t angle_origin;

    float angle_filtered;
    float filter_coefficient;
    int32_t angle_absolute;
    int32_t angle_velocity;
    int32_t angle_velocity_filtered;
    int32_t angle_velocity_last;
    int32_t angle_acceleration;
    int16_t cycle;
} encoder_t;
extern encoder_t encoder;
void encoder_init(void);
void MT6816_Get_Angle(encoder_t *encoder_read);
#endif /* ENCODER_H_ */