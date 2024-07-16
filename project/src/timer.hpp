#pragma once

#include "at32f402_405.h"

extern crm_clocks_freq_type crm_clocks_freq_struct;

void timer_init(void)
{
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
    
    /* 8000hz */
    tmr_base_init(TMR1, 125 - 1, (crm_clocks_freq_struct.sclk_freq / 1000000) - 1);
    
    tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
    tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 1, 0);
    tmr_counter_enable(TMR1, TRUE);
}

extern "C" void TMR1_OVF_TMR10_IRQHandler(void)
{
    /* 判断溢出标志位是否置起 */
    if (tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET) {
        /* 清除溢出标志位 */
        tmr_flag_clear(TMR1, TMR_OVF_FLAG);

        // 这里是timer中断处理
        
        // tim_process();
    }
}