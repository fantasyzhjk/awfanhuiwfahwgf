#pragma once

#include <cstdlib>
#include "stddef.h"
#include "stdint.h"
#include "math.h"


namespace utils
{
    // 来源于雷神之锤开源代码
    inline float Q_rsqrt(float number)
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wstrict-aliasing"
        int32_t i = 0; // 00000000 00000000 00000000
        // IEEE 754   //1       8(-127~128)   (23)
        float x2 = 0, y = 0; // 0(sign) 00000000(exp) 000000000000000(tails)
        const float threehalfs = 1.5f;
        x2                     = number * 0.5f;
        y                      = number;
        i                      = *(int32_t *)&y;
        i                      = 0x5f3759df - (i >> 1);
        y                      = *(float *)&i; 
        y                      = y * (threehalfs - (x2 * y * y));
        // y = y * (threehalfs - (x2 * y * y));
        // y = y * (threehalfs - (x2 * y * y));
        return y;
        #pragma GCC diagnostic pop
    }

    inline uint8_t bool2u8(bool stat) { return stat ? 1 : 0; }
    
    // 随机值
    inline uint32_t randomNum(uint32_t minNum, uint32_t maxNum)
    {
        return rand() % (maxNum - minNum + 1) + minNum;
    }

    struct KalmanFilter {
        float x; // 状态估计
        float P; // 估计误差协方差

        float F; // 状态转移矩阵
        float Q; // 状态转移噪声协方差

        float H; // 观测矩阵
        float R; // 观测噪声协方差

        float K; // 卡尔曼增益

        inline void predict()
        {
            // 预测步骤
            x = F * x;
            P = F * P * F + Q;
        }

        inline void update(float measurement)
        {
            // 更新步骤
            K = P * H / (H * P * H + R);
            x = x + K * (measurement - H * x);
            P = (1 - K * H) * P;
        }
    };

}
