#pragma once

#include "at32f402_405.h"


void gpio_init(void)
{
    /* enable gpiob periph clock */
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_bits_reset(GPIOB, GPIO_PINS_10);

    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_10;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);
}
