
#include "at32f402_405_board.h"
#include "at32f402_405_clock.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "hid/hid_iap_class.h"
#include "hid/hid_iap_desc.h"
#include "hid_iap_user.h"
#include "utils.hpp"
#include "gpio.hpp"
#include "spi.hpp"

/** @addtogroup AT32F405_periph_examples
 * @{
 */

/** @addtogroup 405_USB_device_mouse USB_device_mouse
 * @{
 */

/* usb global struct define */
otg_core_type otg_core_struct;
crm_clocks_freq_type crm_clocks_freq_struct;
void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);
void system_clock_recover(void);
void button_exint_init(void);

/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void)
{
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_2);

    system_clock_config();

    // gpio和spi的初始化
    // gpio_init();
    // spi2_init();
    // 这个如果启用了后，在跳转之前记得把中断和时钟关了

    iap_init();

    // 跳转
    if(iap_get_upgrade_flag() == IAP_SUCCESS)
    {
        jump_to_app(FLASH_APP_ADDRESS);
    }

    // 非跳转，正常初始化设备

    at32_board_init();

    crm_clocks_freq_get(&crm_clocks_freq_struct);

    uart_print_init(115200);

    /* usb gpio config */
    usb_gpio_config();

#ifdef USB_LOW_POWER_WAKUP
    /* enable pwc and bpr clock */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    button_exint_init();
    usb_low_power_wakeup_config();
#endif

    /* enable otg clock */
    crm_periph_clock_enable(OTG_CLOCK, TRUE);

    /* select usb 48m clcok source */
    usb_clock48m_select(USB_CLK_HEXT);


    // at32_led_on(LED2);
    // at32_led_on(LED3);
    // at32_led_on(LED4);

    // flash读写测试
    // 扇区127 2kb ~ 0x0803FFFF

    // u8 _buffer[16];
    // u8 tempbuf[16] = {'c', 's', 'v', 'w', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};

    // flash_read(hall_config_flash_addr, _buffer, sizeof(_buffer));
    // for (int i = 0; i < sizeof(_buffer) / sizeof(_buffer[0]); i++) {
    //     printf("read1: %u\n", _buffer[i]);
    // }

    // // set buf
    // flash_write(hall_config_flash_addr, tempbuf, sizeof(tempbuf));
    // for (int i = 0; i < sizeof(tempbuf) / sizeof(tempbuf[0]); i++) {
    //     printf("write1: %u\n", tempbuf[i]);
    // }

    // flash_read(hall_config_flash_addr, _buffer, sizeof(_buffer));
    // for (int i = 0; i < sizeof(_buffer) / sizeof(_buffer[0]); i++) {
    //     printf("read2: %u\n", _buffer[i]);
    // }

    /* enable otg irq */
    nvic_irq_enable(OTG_IRQ, 0, 1);

    /* init usb */
    usbd_init(&otg_core_struct,
              USB_SPEED_CORE_ID,
              USB_ID,
              &hid_iap_class_handler,
              &hid_iap_desc_handler);


    printf("device init finished\n");

    while (1) {
        iap_loop();
    }
}

/**
 * @brief  usb 48M clock select
 * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
 * @retval none
 */
void usb_clock48m_select(usb_clk48_s clk_s)
{
    if (clk_s == USB_CLK_HICK) {
        crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

        /* enable the acc calibration ready interrupt */
        crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

        /* update the c1\c2\c3 value */
        acc_write_c1(7980);
        acc_write_c2(8000);
        acc_write_c3(8020);

        /* open acc calibration */
        acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
    } else {
        crm_pllu_output_set(TRUE);
        /* wait till pll is ready */
        while (crm_flag_get(CRM_PLLU_STABLE_FLAG) != SET) {
        }
        crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLLU);
    }
}

/**
 * @brief  this function config gpio.
 * @param  none
 * @retval none
 */
void usb_gpio_config(void)
{
    gpio_init_type gpio_init_struct;

    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull           = GPIO_PULL_NONE;

    crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
#ifdef USB_SOF_OUTPUT_ENABLE
    crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
    gpio_init_struct.gpio_pins = OTG_PIN_SOF;
    gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
    gpio_pin_mux_config(OTG_PIN_SOF_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

    /* use vbus pin */
#ifndef USB_VBUS_IGNORE
    gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
    gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
    gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
    gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif
}
#ifdef USB_LOW_POWER_WAKUP
/**
 * @brief  usb low power wakeup interrupt config
 * @param  none
 * @retval none
 */
void usb_low_power_wakeup_config(void)
{
    exint_init_type exint_init_struct;

    crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
    exint_default_para_init(&exint_init_struct);

    exint_init_struct.line_enable   = TRUE;
    exint_init_struct.line_mode     = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select   = OTG_WKUP_EXINT_LINE;
    exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
    exint_init(&exint_init_struct);

    nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
 * @brief  system clock recover.
 * @param  none
 * @retval none
 */
void system_clock_recover(void)
{
    /* enable external high-speed crystal oscillator - hext */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

    /* wait till hext is ready */
    while (crm_hext_stable_wait() == ERROR);

    /* config pllu div */
    crm_pllu_div_set(CRM_PLL_FU_18);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while (crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}

/**
 * @brief  configure button exint
 * @param  none
 * @retval none
 */
void button_exint_init(void)
{
    exint_init_type exint_init_struct;

    crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
    scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);

    exint_default_para_init(&exint_init_struct);
    exint_init_struct.line_enable   = TRUE;
    exint_init_struct.line_mode     = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select   = EXINT_LINE_0;
    exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
    exint_init(&exint_init_struct);

    nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
 * @brief  this function handles otgfs wakup interrupt.
 * @param  none
 * @retval none
 */
extern "C" void OTG_WKUP_HANDLER(void)
{
    exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

/**
 * @brief  exint0 interrupt handler
 * @param  none
 * @retval none
 */
extern "C" void EXINT0_IRQHandler(void)
{
    exint_flag_clear(EXINT_LINE_0);
}

#endif

/**
 * @brief  this function handles otgfs interrupt.
 * @param  none
 * @retval none
 */
extern "C" void OTG_IRQ_HANDLER(void)
{
    usbd_irq_handler(&otg_core_struct);
}

/**
 * @brief  usb delay millisecond function.
 * @param  ms: number of millisecond delay
 * @retval none
 */
void usb_delay_ms(uint32_t ms)
{
    /* user can define self delay function */
    delay_ms(ms);
}

/**
 * @brief  usb delay microsecond function.
 * @param  us: number of microsecond delay
 * @retval none
 */
void usb_delay_us(uint32_t us)
{
    delay_us(us);
}

/**
 * @}
 */

/**
 * @}
 */