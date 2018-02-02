/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "st_service.h"
#include "gpio_management.h"

#define CENTRAL_LINK_COUNT               0                                          /**<number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT            1                                          /**<number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                      "Sommerzeit"                               /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                "DLR"                                      /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                 MSEC_TO_UNITS(25, UNIT_0_625_MS)           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS       180                                        /**< The advertising timeout in units of seconds. */

#define MIN_CONN_INTERVAL                MSEC_TO_UNITS(100, UNIT_1_25_MS)           /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL                MSEC_TO_UNITS(200, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                    0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                 MSEC_TO_UNITS(5000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds). */

#define DEAD_BEEF                        0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t                          m_conn_handle = BLE_CONN_HANDLE_INVALID;   /**< Handle of the current connection. */
                                   
/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    // Activate the wake up pin
    gpio_mgmt_wakeup_start_sensing();
    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    uint32_t err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_IDLE:
            gpio_mgmt_set_status(LED_STATUS_IDLE);
            sleep_mode_enter();
            break;
        default:
            break;
    }
}

static bool check_conn_param(ble_gap_conn_params_t* p_params)
{
    if (p_params->max_conn_interval >= MAX_CONN_INTERVAL
        || p_params->min_conn_interval <= MIN_CONN_INTERVAL)
//            || params->conn_sup_timeout <= CONN_SUP_TIMEOUT)
    {
        // If params not ok, write the values we would like to have
        p_params->min_conn_interval = MIN_CONN_INTERVAL;
        p_params->max_conn_interval = MAX_CONN_INTERVAL;
        p_params->slave_latency     = 0;
        p_params->conn_sup_timeout  = CONN_SUP_TIMEOUT;
    }

}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

    case BLE_GAP_EVT_CONNECTED:
    {
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        // Check if the connection parameters are acceptable
        ble_gap_conn_params_t* p_params = &p_ble_evt->evt.gap_evt.params.connected.conn_params;
        if (check_conn_param(p_params))
        {
            APP_ERROR_CHECK(sd_ble_gap_conn_param_update(m_conn_handle, p_params));
        }
        break;
    }
    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        ble_gap_conn_params_t* p_params = &p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params;
        if (!check_conn_param(p_params))
        {
            APP_ERROR_CHECK(sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE));
        }
        break;
    }
        default:
            // No implementation needed.
            break;
        }
    }


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    ble_st_service_on_ble_evt(p_ble_evt);
}

/**
 * Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    ble_advertising_on_sys_evt(sys_evt);
}

/**
 * Function for initializing the BLE stack.
 *
 * \details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    nrf_clock_lf_cfg_t clock_lf_cfg =
    {
        .source = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv = 0,
        .rc_temp_ctiv = 0,
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
    };
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
    uint32_t app_ram_base;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_base);
    // TODO: Check actual RAM-usage
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**
 * Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build advertising data struct to pass into ble_advertising_init().
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    // Create a scan response packet and include the list of UUIDs

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

int main(void)
{   
    uint32_t err_code;

    // Initialize.
    ble_stack_init();
    gap_params_init();
    st_service_init();
    advertising_init();
    gpio_mgmt_set_status(LED_STATUS_POWER_UP);

    // Start execution.
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    gpio_mgmt_set_status(LED_STATUS_ADVERTISING);

    // Enter main loop.
    while(1)
    {
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}

/**
 * @}
 */

