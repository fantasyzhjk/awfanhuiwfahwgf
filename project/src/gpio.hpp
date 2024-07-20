#pragma once

#include "at32f402_405.h"

void board_gpio_init(void)
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

void gpio_deinit(gpio_type *gpio_x, uint16_t gpio_pins)
{
    uint16_t pinx_value, pin_index = 0;
    pinx_value = gpio_pins;
    while(pinx_value > 0)
    {
        if(pinx_value & 0x01)
        {
            gpio_x->cfgr  &= ~(0x03 << (pin_index * 2));
            gpio_x->omode &= ~(0x01 << pin_index);
            gpio_x->odrvr &= ~(0x03 << (pin_index * 2));
            gpio_x->pull  &= ~(0x03 << (pin_index * 2));
        }
        pinx_value >>= 1;
        pin_index++;
    }
}

void gpio_deinit_PB()
{
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, FALSE);
    // uint16_t gpio_pins = (1 << 16) - 1; //全部引脚
    uint16_t gpio_pins = (1 << 10); //pb10
    gpio_deinit(GPIOB, gpio_pins);
}