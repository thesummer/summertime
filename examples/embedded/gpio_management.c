#include "gpio_management.h"

#include "nrf_drv_gpiote.h"

static gpio_mgt_pendulum_handler_t m_pendulum_callback;

static void
port_event_int_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    // In case the pendulum pin triggered the ISR send a live measurement packet
    if (pin == PIN_PENDULUM)
    {
        m_pendulum_callback();
    }
}

void
gpio_mgmt_init(gpio_mgt_pendulum_handler_t pendulum_evt_handler)
{
    APP_ERROR_CHECK(nrf_drv_gpiote_init());
    ret_code_t err_code;

    nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    err_code = nrf_drv_gpiote_in_init(PIN_PENDULUM,
                                     &pin_config,
                                     port_event_int_handler);
    APP_ERROR_CHECK(err_code);

    m_pendulum_callback = pendulum_evt_handler;
}

void
gpio_mgmt_start_sensing_pendulum()
{
    nrf_drv_gpiote_in_event_enable(PIN_PENDULUM, true);
}

void
gpio_mgmt_stop_sensing_pendulum()
{
    nrf_drv_gpiote_in_event_disable(PIN_PENDULUM);
}
