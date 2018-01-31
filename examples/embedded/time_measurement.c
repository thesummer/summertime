
#include "time_measurement.h"
#include "gpio_management.h"

#include "nrf_drv_rtc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"
#include "st_service.h"

static const nrf_drv_rtc_t rtc1 = NRF_DRV_RTC_INSTANCE(1);
static const nrf_drv_timer_t timer1 = NRF_DRV_TIMER_INSTANCE(1);

#define TICKS_PER_SECOND RTC_DEFAULT_CONFIG_FREQUENCY
#define TICKS_PER_MINUTE (TICKS_PER_SECOND*60)

// Internal variables representing the measurement values
static minute_rate_t m_minute_rate = {};
static live_measurement_t m_live_measurement = {};
// Current second count of timer1
static uint16_t m_current_second = 0;

// PPI channel which connects RTC1's compare1 event to RTC1 clear task
nrf_ppi_channel_t m_ppi_ch_cmp1_clear;
// PPI channel which connects RTC1's compare1 event to RTC2 start task
nrf_ppi_channel_t m_ppi_ch_cmp1_rtc2_start;

/**
 *  When RTC1 hits the compare0 event every minute, trigger a notification update
 *  for the rate measurement
 */
static void
rtc1_int_handler(nrf_drv_rtc_int_type_t int_type)
{
        st_minute_rate_update(&m_minute_rate);
        m_live_measurement.minute++;
        m_minute_rate.minute++;
        m_minute_rate.rate_count = 0;
}

/**
 * Timer1 hits the compare event every seconds
 * count the secounds of the current minute
 */
static void
timer1_int_handler(nrf_timer_event_t event_type,
                   void* p_context)
{
    m_current_second++;
    if (m_current_second == 60)
    {
        m_current_second = 0;
    }
}

static void
timer1_pendulum_int_handler()
{
    m_minute_rate.rate_count++;
    m_live_measurement.counter = m_current_second * TICKS_PER_SECOND
                                 + nrf_drv_timer_capture_get(&timer1, NRF_TIMER_CC_CHANNEL1);
    st_live_measurement_update(&m_live_measurement);
}

void
time_measurement_init()
{
    // Initialize all timers:
    uint32_t err_code;
    nrf_drv_rtc_config_t rtc_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    // RTC 1 will use it's interrupt to update the minute rate
    err_code = nrf_drv_rtc_init(&rtc1, &rtc_config, rtc1_int_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_config = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_config.mode = NRF_TIMER_MODE_COUNTER;
    err_code = nrf_drv_timer_init(&timer1, &timer_config, timer1_int_handler);
    APP_ERROR_CHECK(err_code);

    APP_ERROR_CHECK(nrf_drv_ppi_init());

    // Enable tick events to trigger timer1 later; don't fire IRQ on RTC tick
    nrf_drv_rtc_tick_enable(&rtc1, false);
    // Connect tick events to timer1 count task
    nrf_ppi_channel_t ppi_channel;
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_rtc_event_address_get(&rtc1, NRF_RTC_EVENT_TICK),
                                          nrf_drv_timer_task_address_get(&timer1, NRF_TIMER_TASK_COUNT));
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ppi_channel));

    // Clear timer1 every 1s, i.e. compare to TICKS_PER_SECOND and clear; Use interrupt to count seconds
     nrf_drv_timer_extended_compare(&timer1, 0, TICKS_PER_SECOND,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    // Clear RTC every minute, i.e. compare to TICKS_PER_MINUTE and clear
    err_code = nrf_drv_rtc_cc_set(&rtc1, 0, TICKS_PER_MINUTE, true);
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_rtc_event_address_get(&rtc1, NRF_RTC_EVENT_COMPARE_0),
                                          nrf_drv_rtc_task_address_get(&rtc1, NRF_RTC_TASK_CLEAR));
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ppi_channel));

    // Setup GPIOs
    gpio_mgmt_init(timer1_pendulum_int_handler);

    // Connect Port event to capture1 task of timer1
    // Will later tell at which time the pendulum passed
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_gpiote_in_event_addr_get(PIN_PENDULUM),
                                          nrf_drv_timer_task_address_get(&timer1, NRF_TIMER_TASK_CAPTURE1));
}

void
time_measurement_start()
{
    // Make sure everything is in start condition
    nrf_drv_rtc_counter_clear(&rtc1);
    nrf_drv_timer_clear(&timer1);
    // Start all timers
    nrf_drv_rtc_enable(&rtc1);
    nrf_drv_timer_enable(&timer1);
    gpio_mgmt_start_sensing_pendulum();
}

void
time_measurement_stop()
{
    // Stop all timers
    gpio_mgmt_stop_sensing_pendulum();
    nrf_drv_rtc_disable(&rtc1);
    nrf_drv_timer_disable(&timer1);
}
