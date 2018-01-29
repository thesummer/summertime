#include "ble_thread.h"

extern "C"
{
#include <nrf_drv_clock.h>
#include <softdevice_handler.h>
#include <app_timer.h>
#include <ble_conn_state.h>
#include <peer_manager.h>
#include <ble_conn_params.h>
#include <ble_advertising.h>
}

const outpost::time::Duration SoftDeviceThread::MinConnectionInverval = outpost::time::Milliseconds(200);
const outpost::time::Duration SoftDeviceThread::MaxConnectionInverval = outpost::time::Milliseconds(600);
const outpost::time::Duration SoftDeviceThread::ConnectionTimeoutInverval = outpost::time::Seconds(3);
const outpost::time::Duration SoftDeviceThread::AdvertisingInterval = outpost::time::Milliseconds(50);
const outpost::time::Duration SoftDeviceThread::AdvertisingTimeout = outpost::time::Seconds(180);

outpost::rtos::Semaphore SoftDeviceThread::mEventSemaphore(0);


SoftDeviceThread::SoftDeviceThread() :
    // TODO: Use a constant for the priority and stack
    outpost::rtos::Thread(2, 256, "BLE")
{
}

void
SoftDeviceThread::initialize()
{
    uint32_t err_code;

    if (!nrf_drv_clock_init_check())
    {
        nrf_drv_clock_init();
    }
    APP_TIMER_INIT(0, 4, NULL);

    nrf_clock_lf_cfg_t clock_lf_cfg =
    {
        NRF_CLOCK_LF_SRC_RC, // Use internal RC-clock
        16,                  // Recalibarate every 4 s (16*0.25s)
         2,                  // Calibrate on temp change or every 2 cycles
         0                   // ignored f
    };

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, &SoftDeviceThread::handleNewBleEvent);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(0, // No central links
                                                    1, // At most 1 peripheral link
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(&SoftDeviceThread::dispatchBleEvent);
    APP_ERROR_CHECK(err_code);

    initializeGapParameters();
    initializeAdvertising();
    // TODO: Init services
    // TODO: Init characteristics

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

void
SoftDeviceThread::run()
{
    while(1)
    {
        while(!mEventSemaphore.acquire());

        // This function gets events from the SoftDevice and processes them by calling the function
        // registered by softdevice_ble_evt_handler_set during stack initialization.
        // In this code ble_evt_dispatch would be called for every event found.
        intern_softdevice_events_execute();
    }
}

void
SoftDeviceThread::dispatchBleEvent(ble_evt_t* p_ble_evt)
{
    /** The Connection state module has to be fed BLE events in order to function correctly
     * Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions. */
    ble_conn_state_on_ble_evt(p_ble_evt);
//    pm_on_ble_evt(p_ble_evt);   // Needed?
    ble_conn_params_on_ble_evt(p_ble_evt);
//    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}

void SoftDeviceThread::initializeGapParameters()
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    // No protection
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          reinterpret_cast<const uint8_t*>(DeviceName),
                                          strlen(reinterpret_cast<const char*>(DeviceName)));
    APP_ERROR_CHECK(err_code);

    // Set the appearance symbol to a clock.
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_CLOCK);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = connectionIntervalToUnits(MinConnectionInverval);
    gap_conn_params.max_conn_interval = connectionIntervalToUnits(MaxConnectionInverval);
    gap_conn_params.slave_latency     = 0; // Respond to each connection package
    gap_conn_params.conn_sup_timeout  = timeoutIntervalToUnits(ConnectionTimeoutInverval);

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void
SoftDeviceThread::initializeAdvertising()
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    // TODO: What to add here?
//    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
//    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = advertisingIntervalToUnits(AdvertisingInterval);
    options.ble_adv_fast_timeout  = AdvertisingTimeout.seconds();

    err_code = ble_advertising_init(&advdata, NULL, &options, NULL, NULL);
    APP_ERROR_CHECK(err_code);
}
