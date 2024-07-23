#pragma once

#define CALCULATE_AVERAGE(name, number, result, type, AVG_NUM) \
        do { \
            static type numbers_##name[AVG_NUM] = {0}; \
            static uint32_t count_##name = 0; \
            numbers_##name[count_##name % AVG_NUM] = number; \
            count_##name++; \
            int numCount_##name = (count_##name < AVG_NUM) ? count_##name : AVG_NUM; \
            type sum_##name = 0; \
            for (int i_##name = 0; i_##name < numCount_##name; i_##name++) { \
                sum_##name += numbers_##name[i_##name]; \
            } \
            result = sum_##name / (type)numCount_##name; \
        } while (0)
            
#define C_MAX(a,b) ((a) > (b) ? (a) : (b))
#define C_MIN(a,b) ((a) < (b) ? (a) : (b))
#define C_MAX3(a,b,c) (((a) > (b) ? (a) : (b)) > (c) ? ((a) > (b) ? (a) : (b)) : (c))
#define C_MIN3(a,b,c) (((a) < (b) ? (a) : (b)) < (c) ? ((a) < (b) ? (a) : (b)) : (c))
#define C_MAXF3(a,b,c) (fmaxf(fmaxf(a, b), c))
#define C_MINF3(a,b,c) (fminf(fminf(a, b), c))

#define KEY_NUMS 64
#define flag_flash_addr 0x3C000
#define keyboard_config_flash_addr 0x3C800
#define device_config_flash_addr 0x3D000

#define IAP_UPGRADE_COMPLETE_FLAG        0x41554B57

