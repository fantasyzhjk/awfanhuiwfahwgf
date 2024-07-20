#pragma once

#include "at32f402_405.h"
#include "gpio.hpp"

void board_spi2_init(void)
{
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
    
    gpio_init_type gpio_init_struct;
    spi_init_type spi_init_struct;

    gpio_default_para_init(&gpio_init_struct);
    spi_default_para_init(&spi_init_struct);

    /* configure the SCK pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_1;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);

    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE1, GPIO_MUX_6);

    /* configure the MOSI pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_10;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE10, GPIO_MUX_5);

    /* configure param */
    spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_3;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPI2, &spi_init_struct);

    spi_enable(SPI2, TRUE);

}

void spi_deinit(spi_type *spi_x)
{
    spi_x->i2sctrl_bit.i2smsel = FALSE;

    spi_x->ctrl1_bit.slben = FALSE;
    spi_x->ctrl1_bit.slbtd = FALSE;
    spi_x->ctrl1_bit.ora = FALSE;
    spi_x->ctrl1_bit.swcsen = 0;
    spi_x->ctrl1_bit.swcsil = FALSE;
    spi_x->ctrl1_bit.msten = 0;

    spi_x->ctrl2_bit.mdiv3en = FALSE;
    spi_x->ctrl2_bit.mdiv_h = FALSE;
    spi_x->ctrl1_bit.mdiv_l = 0;

    spi_x->ctrl1_bit.ltf = 0;
    spi_x->ctrl1_bit.fbn = 0;
    spi_x->ctrl1_bit.clkpol = 0;
    spi_x->ctrl1_bit.clkpha = 0;
}

void gpio_pin_mux_unconfig(gpio_type *gpio_x, gpio_pins_source_type gpio_pin_source)
{
    uint32_t bit_offset = ((uint32_t)gpio_pin_source & (uint32_t)0x07) * 4;
    if (gpio_pin_source >> 0x03) {
        gpio_x->muxh &= ~(0xF << bit_offset);
    } else {
        gpio_x->muxl &= ~(0xF << bit_offset);
    }
}


void spi_deinit_2(void)
{
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, FALSE);
    
    spi_enable(SPI2, FALSE);
    spi_deinit(SPI2);

    uint16_t gpio_pins = (1 << 1);
    gpio_deinit(GPIOB, gpio_pins);
    gpio_pins = (1 << 10);
    gpio_deinit(GPIOA, gpio_pins);
    gpio_pin_mux_unconfig(GPIOB, GPIO_PINS_SOURCE1);
    gpio_pin_mux_unconfig(GPIOA, GPIO_PINS_SOURCE10);  
}