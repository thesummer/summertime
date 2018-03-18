#include "status_led_mgmt.h"

#include "nrf_drv_timer.h"
#include "nrf_gpio.h"

#include "time_measurement.h"

#define MAX_TIMER_VALUE 0xFFFF
#define MAX_TOGGLE_TIME 0xFFFFFFFF
#define MS_TO_TICKS(ms) (32768*ms/1000)

static const nrf_drv_timer_t timer2 = NRF_DRV_TIMER_INSTANCE(2);
static gpio_mgt_status_evt_handler_t m_status_callback = NULL;
typedef struct
{
    uint32_t next_time;
    uint32_t toggle_time_ticks;
    uint32_t maximum_time_ticks;
} pin_toggle_config_t;

static const pin_toggle_config_t disabled = { 0, MAX_TIMER_VALUE+1, 0 };

static pin_toggle_config_t m_pin_power_config;
static pin_toggle_config_t m_pin_reset_config;
static led_status_t m_status = LED_STATUS_NONE;


static void
start_toggle(uint32_t pin_number)
{
    nrf_drv_timer_clear(&timer2);
    if (pin_number == PIN_POWER)
    {
        m_pin_power_config.next_time = m_pin_power_config.toggle_time_ticks;
        nrf_timer_cc_write(timer2.p_reg, NRF_TIMER_CC_CHANNEL0,
                           m_pin_power_config.next_time);
        nrf_drv_timer_compare_int_enable(&timer2, NRF_TIMER_CC_CHANNEL0);
        nrf_gpio_pin_set(PIN_POWER);
    }
    else
    {
        m_pin_reset_config.next_time = nrf_drv_timer_capture(&timer2, NRF_TIMER_CC_CHANNEL1)
                                              + m_pin_reset_config.toggle_time_ticks;
        m_pin_reset_config.next_time %= MAX_TIMER_VALUE;
        nrf_timer_cc_write(timer2.p_reg, NRF_TIMER_CC_CHANNEL1,
                           m_pin_reset_config.next_time);
        nrf_drv_timer_compare_int_enable(&timer2, NRF_TIMER_CC_CHANNEL1);
        nrf_gpio_pin_set(PIN_RESET);
    }
}

static void
stop_toggle()
{
//    nrf_drv_timer_disable(&timer2);
    nrf_drv_timer_compare_int_disable(&timer2, NRF_TIMER_CC_CHANNEL0);
    nrf_drv_timer_compare_int_disable(&timer2, NRF_TIMER_CC_CHANNEL1);
    m_pin_power_config = disabled;
    m_pin_reset_config = disabled;
    nrf_gpio_pin_clear(PIN_POWER);
    nrf_gpio_pin_clear(PIN_RESET);
}

static void
timer2_int_handler(nrf_timer_event_t event_type,
                   void* p_context)
{
    static volatile int count = 0;
    if (event_type == NRF_TIMER_EVENT_COMPARE0)
    {
        if (m_pin_power_config.next_time < m_pin_power_config.maximum_time_ticks)
        {
            m_pin_power_config.next_time += m_pin_power_config.toggle_time_ticks;
            volatile uint32_t nextTimer = m_pin_power_config.next_time & MAX_TIMER_VALUE;
            nrf_timer_cc_write(timer2.p_reg, NRF_TIMER_CC_CHANNEL0,
                               nextTimer/*m_pin_power_config.next_time & MAX_TIMER_VALUE*/);
            nrf_gpio_pin_toggle(PIN_POWER);
            count++;
        }
        else
        {
            stop_toggle();
            if (m_status_callback)
            {
                m_status_callback(m_status);
            }
            m_status = LED_STATUS_NONE;
        }
    }
    else
    {
        count = event_type + 10;
        if (m_pin_reset_config.next_time < m_pin_reset_config.maximum_time_ticks)
        {
            m_pin_reset_config.next_time += m_pin_reset_config.toggle_time_ticks;
            nrf_timer_cc_write(timer2.p_reg, NRF_TIMER_CC_CHANNEL1,
                               m_pin_reset_config.next_time & MAX_TIMER_VALUE);
            nrf_gpio_pin_toggle(PIN_RESET);
        }
        else
        {
            stop_toggle();
            if (m_status_callback)
            {
                m_status_callback(m_status);
            }
            m_status = LED_STATUS_NONE;
        }
    }
}

void
status_led_mgmt_init(gpio_mgt_status_evt_handler_t callback)
{
    m_status_callback = callback;
    nrf_gpio_cfg(
        PIN_POWER,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1,     // Drive PIN in high current mode
        NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
        PIN_RESET,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1,     // Drive PIN in high current mode
        NRF_GPIO_PIN_NOSENSE);

    nrf_drv_timer_config_t timer_config = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_config.mode = NRF_TIMER_MODE_COUNTER;
    uint32_t err_code = nrf_drv_timer_init(&timer2, &timer_config, timer2_int_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_timer_enable(&timer2);
    time_measurement_rtc_start();
}

void
status_led_mgmt_set_status(led_status_t status)
{
    stop_toggle();
    switch (status)
    {
    case LED_STATUS_POWER_UP: // power led continuous
        nrf_gpio_pin_set(PIN_POWER);
        break;
    case LED_STATUS_IDLE:          // toggle reset led every 1s
        m_pin_reset_config.maximum_time_ticks = MAX_TOGGLE_TIME;
        m_pin_reset_config.toggle_time_ticks  = MS_TO_TICKS(1000);
        start_toggle(PIN_RESET);
        break;
    case LED_STATUS_ADVERTISING:   // power led toggle every 100ms
        m_pin_power_config.maximum_time_ticks = MAX_TOGGLE_TIME;
        m_pin_power_config.toggle_time_ticks  = MS_TO_TICKS(100);
        start_toggle(PIN_POWER);
        break;
    case LED_STATUS_CONNECTED:
        m_pin_power_config.maximum_time_ticks = MAX_TOGGLE_TIME;
        m_pin_power_config.toggle_time_ticks  = MS_TO_TICKS(1000);
        start_toggle(PIN_POWER);
        break;
    case LED_STATUS_START_MEASUREMENT:   // both leds toggling every 50 ms for 1s
        m_pin_power_config.maximum_time_ticks = MS_TO_TICKS(1000);
        m_pin_power_config.toggle_time_ticks  = MS_TO_TICKS(50);
        m_pin_reset_config.maximum_time_ticks = MS_TO_TICKS(1000);
        m_pin_reset_config.toggle_time_ticks  = MS_TO_TICKS(50);
        start_toggle(PIN_POWER);
        start_toggle(PIN_RESET);
        break;
    case LED_STATUS_ACTIVE_MEASUREMENT:  // tick both leds alternating on every tick for 100ms
    {
        static uint32_t last_led = PIN_POWER;
        if (last_led == PIN_POWER)
        {
            m_pin_reset_config.maximum_time_ticks = MS_TO_TICKS(100);
            m_pin_reset_config.toggle_time_ticks  = MS_TO_TICKS(100);
            last_led = PIN_RESET;
            start_toggle(PIN_RESET);
        }
        else
        {
            m_pin_power_config.maximum_time_ticks = MS_TO_TICKS(100);
            m_pin_power_config.toggle_time_ticks  = MS_TO_TICKS(100);
            last_led = PIN_POWER;
            start_toggle(PIN_POWER);
        }
        break;
    }
    case LED_STATUS_STOP_MEASUREMENT:    // both leds on for 1s
        m_pin_power_config.maximum_time_ticks = MS_TO_TICKS(1000);
        m_pin_power_config.toggle_time_ticks  = MS_TO_TICKS(1000);
        m_pin_reset_config.maximum_time_ticks = MS_TO_TICKS(1000);
        m_pin_reset_config.toggle_time_ticks  = MS_TO_TICKS(1000);
        start_toggle(PIN_POWER);
        start_toggle(PIN_RESET);
        break;
    case LED_STATUS_ERROR:               // both leds on
        nrf_gpio_pin_set(PIN_POWER);
        nrf_gpio_pin_set(PIN_RESET);
        break;
    }
}
