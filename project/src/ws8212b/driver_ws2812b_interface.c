/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ws2812b_interface_template.c
 * @brief     driver ws2812b interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-11-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/11/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ws2812b_interface.h"
#include "at32f402_405.h"
#include "stdarg.h"
#include "at32f402_405_board.h"


/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
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
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPI2, &spi_init_struct);

    spi_enable(SPI2, TRUE);
}




/**
 * @brief  interface spi 10mhz bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init 10mhz failed
 * @note   none
 */
uint8_t ws2812b_interface_spi_10mhz_init(void)
{
    SPI_FullDuplex_Init();
    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t ws2812b_interface_spi_deinit(void)
{   
    spi_enable(SPI2, FALSE);
    return 0;
}

/**
 * @brief     interface spi bus write command
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ws2812b_interface_spi_write_cmd(uint8_t *buf, uint16_t len)
{
    u8 j = 0;

    if (len > 0)
    {
        for (uint16_t i = 0; i < len; i++)
        {
            while(spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == RESET)
            {
                j++;
                if(j > 200)
                {
                    return 1;
                }
            }

            spi_i2s_data_transmit(SPI2, buf[i]);
            j = 0;
        }
    }

    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void ws2812b_interface_delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void ws2812b_interface_debug_print(const char *const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);
}
