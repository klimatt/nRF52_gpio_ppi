
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6) // example for nrf52840 USB Dongle pca10059
#define PIN_GREEN_LED_ONBOARD NRF_GPIO_PIN_MAP(0, 6) // example for nrf52840 USB Dongle pca10059

void gpio_ppi_map(uint8_t src_pin, uint8_t dest_pin)
{
    uint32_t compare_evt_addr;
    uint32_t gpiote_task_addr;
    nrf_ppi_channel_t ppi_channel;
    ret_code_t err_code;

    nrf_drv_gpiote_in_config_t src_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    err_code = nrf_drv_gpiote_in_init(src_pin, &src_config, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t dest_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
    err_code = nrf_drv_gpiote_out_init(dest_pin, &dest_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(err_code);

    compare_evt_addr = nrf_drv_gpiote_in_event_addr_get(src_pin);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(dest_pin);

    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(src_pin, true);
    nrf_drv_gpiote_out_task_enable(dest_pin);
}

int main(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    gpio_ppi_map(BUTTON_PIN, PIN_GREEN_LED_ONBOARD);

    while (true)
    {
        __WFE();
        __SEV();
        __WFE();
    }
}

