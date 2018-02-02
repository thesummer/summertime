#include "gpio_management.h"

#include "nrf_drv_gpiote.h"

static bool m_is_initialized = false;
static gpio_mgt_evt_handler_t m_pendulum_callback = NULL;
static gpio_mgt_evt_handler_t m_wake_up_callback = NULL;

static void
port_event_int_handler(nrf_drv_gpiote_pin_t pin,
                       nrf_gpiote_polarity_t action)
{
    // In case the pendulum pin triggered the ISR send a live measurement packet
    if (pin == PIN_PENDULUM)
    {
        m_pendulum_callback();
    }
    else // PIN_WAKE_UP
    {
        m_wake_up_callback();
    }
}

void
gpio_mgmt_pendulum_init(gpio_mgt_evt_handler_t pendulum_evt_handler)
{
    if (!m_is_initialized)
    {
        APP_ERROR_CHECK(nrf_drv_gpiote_init());
        m_is_initialized = true;
    }

    // Configure pendulum pin to fire on a transition from high to low
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    err_code = nrf_drv_gpiote_in_init(PIN_PENDULUM,
                                     &pin_config,
                                     port_event_int_handler);
    APP_ERROR_CHECK(err_code);

    m_pendulum_callback = pendulum_evt_handler;
}

void
gpio_mgmt_pendulum_start_sensing()
{
    nrf_drv_gpiote_in_event_enable(PIN_PENDULUM, true);
}

void
gpio_mgmt_pendulum_stop_sensing()
{
    nrf_drv_gpiote_in_event_disable(PIN_PENDULUM);
}

void
gpio_mgmt_wakeup_init(gpio_mgt_evt_handler_t wake_up_evt_handler)
{
    if (!m_is_initialized)
    {
        APP_ERROR_CHECK(nrf_drv_gpiote_init());
        m_is_initialized = true;
    }

    // Configure wake up pin to fire on a transition from low to high
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    pin_config.pull = GPIO_PIN_CNF_PULL_Pulldown;
    err_code = nrf_drv_gpiote_in_init(PIN_WAKE_UP,
                                     &pin_config,
                                     port_event_int_handler);
    m_wake_up_callback = wake_up_evt_handler;
}

void
gpio_mgmt_wakeup_start_sensing()
{
nrf_drv_gpiote_in_event_enable(PIN_WAKE_UP, true);
}

void
gpio_mgmt_wakeup_stop_sensing()
{
    nrf_drv_gpiote_in_event_disable(PIN_WAKE_UP);
}
