#pragma once
#include "ws8212b/driver_ws2812b_interface.h"
#include "color.hpp"

template <uint32_t led_numbers>
class WS2812B
{

private:
    uint8_t gs_buffer[64 * led_numbers];
    ws2812b_handle_t gs_handle = {0};
    volatile bool changed      = true;

public:
    uint32_t gs_rgb[led_numbers];

    u8 Init()
    {
        DRIVER_WS2812B_LINK_INIT(&gs_handle, ws2812b_handle_t);
        DRIVER_WS2812B_LINK_SPI_10MHZ_INIT(&gs_handle, ws2812b_interface_spi_10mhz_init);
        DRIVER_WS2812B_LINK_SPI_DEINIT(&gs_handle, ws2812b_interface_spi_deinit);
        DRIVER_WS2812B_LINK_SPI_WRITE_COMMAND(&gs_handle, ws2812b_interface_spi_write_cmd);
        DRIVER_WS2812B_LINK_DELAY_MS(&gs_handle, ws2812b_interface_delay_ms);
        DRIVER_WS2812B_LINK_DEBUG_PRINT(&gs_handle, ws2812b_interface_debug_print);
        u8 res = ws2812b_init(&gs_handle);
        if (res != 0) {
            ws2812b_interface_debug_print("ws2812b: init failed.\n");
            return 1;
        }
        return 0;
    }

    u8 DeInit()
    {
        return ws2812b_deinit(&gs_handle);
    }

    u8 Refresh()
    {
        // if (changed) {
            u8 res = ws2812b_write(&gs_handle, (uint32_t *)gs_rgb, led_numbers, gs_buffer, sizeof(gs_buffer));
            if (res != 0) {
                ws2812b_interface_debug_print("ws2812b: write failed.\n");
                return 2;
            }
            changed = false;
            return 0;
        // }
        // return 1;
    }

    inline void SetColor(const uint32_t index, uint32_t color)
    {
        if (gs_rgb[index] != color) {
            gs_rgb[index] = color;
            changed       = true;
        }
    }

    inline void SetColor(const uint32_t index, Color::RGB color)
    {
        this->SetColor(index, color.toHEX());
    }
    
    inline void SetColor(const uint32_t index, Color::HSV color)
    {
        this->SetColor(index, color.toRGB());
    }

    inline void SetAllColor(uint32_t color)
    {
        for (uint32_t i = 0; i < led_numbers; ++i) {
            this->SetColor(i, color);
        }
    }

    inline void SetAllColor(Color::RGB color)
    {
        this->SetAllColor(color.toHEX());
    }

    inline void SetAllColor(Color::HSV color)
    {
        this->SetAllColor(color.toRGB());
    }

    ~WS2812B()
    {
        ws2812b_deinit(&gs_handle);
    }
};