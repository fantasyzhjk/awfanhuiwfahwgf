#pragma once
#include <cmath>
#include <cstdint>
#include "define.h"

#include "utils.hpp"

namespace Color
{
    // pre define
    struct HSV;
    struct RGB;

    struct RGB {
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        // 从rgb数值创建16进制的rgb值
        inline uint32_t toHEX() const
        {
            return ((this->red & 0xff) << 16) + ((this->green & 0xff) << 8) + (this->blue & 0xff); // RGB
        }

        // factor 0-1
        inline void changeBrightness(float factor)
        {
            red *= factor;
            green *= factor;
            blue *= factor;
        }

        // RGB转HSV
        HSV toHSV() const;

        bool operator==(const RGB &other) const
        {
            return this->red == other.red && this->green == other.green && this->blue == other.blue;
        }
    };

    // 16进制的rgb值创建RGB数值
    inline RGB fromHEX(uint32_t color)
    {
        RGB c   = {0};
        c.red   = color >> 16 & 0xff;
        c.green = color >> 8 & 0xff;
        c.blue  = color >> 0 & 0xff;
        return c;
    }

    struct HSV {
        float hue;
        float saturation;
        float value;

        // HSV调整
        inline HSV adjustValue(float v_offset)
        {
            value += v_offset;
            if (value >= 1.0)
                value = 1.0;
            else if (value <= 0)
                value = 0;

            return *this;
        }

        inline HSV applyValue(float v)
        {
            value = v;
            if (value >= 1.0)
                value = 1.0;
            else if (value <= 0)
                value = 0;

            return *this;
        }

        // HSV转RGB
        RGB toRGB() const;

        bool operator==(const HSV &other) const
        {
            return this->hue == other.hue && this->saturation == other.saturation && this->value == other.value;
        }
    };

    HSV RGB::toHSV() const
    {
        HSV hsv;
        float r = this->red / 255.0f;
        float g = this->green / 255.0f;
        float b = this->blue / 255.0f;

        float max  = C_MAXF3(r, g, b);
        float min  = C_MINF3(r, g, b);
        float diff = max - min;

        // 计算Hue
        if (diff == 0) {
            hsv.hue = 0; // 无色
        } else if (max == r) {
            hsv.hue = 60 * ((g - b) / diff);
            if (hsv.hue < 0) {
                hsv.hue += 360;
            }
        } else if (max == g) {
            hsv.hue = 60 * ((b - r) / diff + 2);
        } else if (max == b) {
            hsv.hue = 60 * ((r - g) / diff + 4);
        } else {
            hsv.hue = 0;
        }

        // 计算Saturation
        if (max == 0) {
            hsv.saturation = 0;
        } else {
            hsv.saturation = diff / max;
        }

        // 计算Value
        hsv.value = max;

        return hsv;
    }

    RGB HSV::toRGB() const
    {
        RGB rgb;

        float c = this->value * this->saturation;
        float x = c * (1 - std::fabs(std::fmod(this->hue / 60.0f, 2) - 1));
        float m = this->value - c;

        float r, g, b;

        if (this->hue >= 0 && this->hue < 60) {
            r = c;
            g = x;
            b = 0;
        } else if (this->hue >= 60 && this->hue < 120) {
            r = x;
            g = c;
            b = 0;
        } else if (this->hue >= 120 && this->hue < 180) {
            r = 0;
            g = c;
            b = x;
        } else if (this->hue >= 180 && this->hue < 240) {
            r = 0;
            g = x;
            b = c;
        } else if (this->hue >= 240 && this->hue < 300) {
            r = x;
            g = 0;
            b = c;
        } else {
            r = c;
            g = 0;
            b = x;
        }

        rgb.red   = (r + m) * 255;
        rgb.green = (g + m) * 255;
        rgb.blue  = (b + m) * 255;

        return rgb;
    }

    inline RGB linearInterpolation(RGB startColor, RGB endColor, float ratio)
    {
        RGB interpolatedColor;

        // 对每个颜色通道进行线性插值
        interpolatedColor.red   = startColor.red + (endColor.red - startColor.red) * ratio;
        interpolatedColor.green = startColor.green + (endColor.green - startColor.green) * ratio;
        interpolatedColor.blue  = startColor.blue + (endColor.blue - startColor.blue) * ratio;

        return interpolatedColor;
    }

    inline Color::HSV linearInterpolation(Color::HSV startColor, Color::HSV endColor, float progress)
    {
        Color::HSV result;
        result.hue        = startColor.hue + (endColor.hue - startColor.hue) * progress;
        result.saturation = startColor.saturation + (endColor.saturation - startColor.saturation) * progress;
        result.value      = startColor.value + (endColor.value - startColor.value) * progress;
        return result;
    }

    // 设置随机颜色
    inline HSV Random(float value = 1.0)
    {
        HSV hsv;
        uint32_t hue = utils::randomNum(0, 359);

        // hue = hue - (hue % 5); // 将hue向下取整到最接近的可被5整除的数

        hsv.hue        = hue;
        hsv.saturation = 1.00;
        hsv.value      = value;
        return hsv;
    }

    const RGB NONE   = {.red = 0, .green = 0, .blue = 0};
    const RGB WHITE  = {.red = 255, .green = 255, .blue = 255};
    const RGB RED    = {.red = 255, .green = 0, .blue = 0};
    const RGB GREEN  = {.red = 0, .green = 255, .blue = 0};
    const RGB BLUE   = {.red = 0, .green = 0, .blue = 255};
    const RGB YELLOW = {.red = 255, .green = 255, .blue = 0};
    const RGB PURPLE = {.red = 255, .green = 0, .blue = 255};
    const RGB CYAN   = {.red = 0, .green = 255, .blue = 255};
    const RGB ORANGE = {.red = 255, .green = 85, .blue = 0};
    const RGB PINK   = {.red = 255, .green = 192, .blue = 203};
    const RGB GRAY   = {.red = 128, .green = 128, .blue = 128};
    const RGB BROWN  = {.red = 165, .green = 42, .blue = 42};
    const RGB GOLD   = {.red = 255, .green = 215, .blue = 0};
    const RGB SILVER = {.red = 192, .green = 192, .blue = 192};

} // namespace Color