
#include "time_measurement.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"
#include "st_service.h"

static const nrf_drv_rtc_t rtc1 = NRF_DRV_RTC_INSTANCE(1);
static const nrf_drv_timer_t timer1 = NRF_DRV_TIMER_INSTANCE(1);
static const nrf_drv_timer_t timer2 = NRF_DRV_TIMER_INSTANCE(2);

#define TICKS_PER_SECOND RTC_DEFAULT_CONFIG_FREQUENCY
#define TICKS_PER_MINUTE (TICKS_PER_SECOND*60)

// Internal variables representing the measurement values
static minute_rate_t m_minute_rate = {};
static live_measurement_t m_live_measurement = {};

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
    err_code = nrf_drv_timer_init(&timer1, &timer_config, NULL);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_timer_init(&timer2, &timer_config, NULL);
    APP_ERROR_CHECK(err_code);

    APP_ERROR_CHECK(nrf_drv_ppi_init());

    // Enable tick events to trigger timer1 later, but withou irq
    nrf_drv_rtc_tick_enable(&rtc1, false);
    // Connect tick events to timer1 count task
    nrf_ppi_channel_t ppi_channel;
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_rtc_event_address_get(&rtc1, NRF_RTC_EVENT_TICK),
                                          nrf_drv_timer_task_address_get(&timer1, NRF_TIMER_TASK_COUNT));
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ppi_channel));

    // Clear timer1 every 1s, i.e. compare to TICKS_PER_SECOND and clear
     nrf_drv_timer_extended_compare(&timer1, 0, TICKS_PER_SECOND,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // Call timer2 count on every timer1 compare0 event (i.e. count the seconds)
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_timer_event_address_get(&timer1, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_timer_task_address_get(&timer2, NRF_TIMER_TASK_COUNT));
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ppi_channel));

    // Clear timer2 every 60 s
    nrf_drv_timer_extended_compare(&timer2, 0, 60,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // Clear RTC every minute, i.e. compare to TICKS_PER_MINUTE and clear
    err_code = nrf_drv_rtc_cc_set(&rtc1, 0, TICKS_PER_MINUTE, true);
    APP_ERROR_CHECK(nrf_drv_ppi_channel_alloc(&ppi_channel));
    err_code = nrf_drv_ppi_channel_assign(ppi_channel,
                                          nrf_drv_rtc_event_address_get(&rtc1, NRF_RTC_EVENT_COMPARE_0),
                                          nrf_drv_rtc_task_address_get(&rtc1, NRF_RTC_TASK_CLEAR));
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ppi_channel));

    // Setup GPIOs
    // TODO: Enable GPIO port event on input change (make sure to only have one event per pendulum crossing)
    // TODO: Connect timer1 to capture1 on task to port event
    // TODO: Use GPIO-event interrupt handler to update live measurement characteristic
}

void
time_measurement_start()
{
    // Make sure everything is in start condition
    nrf_drv_rtc_counter_clear(&rtc1);
    nrf_drv_timer_clear(&timer1);
    nrf_drv_timer_clear(&timer2);
    // Start all timers
    nrf_drv_rtc_enable(&rtc1);
    nrf_drv_timer_enable(&timer1);
    nrf_drv_timer_enable(&timer2);
    // TODO: Enable GPIO?
}

void
time_measurement_stop()
{
    // Stop all timers
    nrf_drv_rtc_disable(&rtc1);
    nrf_drv_timer_disable(&timer1);
    nrf_drv_timer_disable(&timer2);
}
