
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "st_service.h"
#include "ble_srv_common.h"
#include "app_error.h"

#include "time_measurement.h"

// This structure contains various status information for our service.
// The name is based on the naming convention used in Nordics SDKs.
// 'ble' indicates that it is a Bluetooth Low Energy relevant structure and
// st is short for summertime Service).
typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    char_live_handles;
    ble_gatts_char_handles_t    char_rate_handles;
}ble_os_t;

static ble_os_t m_handle = {};

/**@brief Function for handling BLE GATTS EVENTS
 * 
 * This function prints out data that is received when you try to write to your characteristic or CCCD. 
 * In general it is a bad idea to do so much printf stuff and UART transfer inside the BLE handler,
 * but this is just for demonstrate purposes.
 *
 * @param[in]   p_our_service        Our Service structure.
 * @param[in]   p_ble_evt            BLE event passed from BLE stack
 *
 */
static void on_ble_write(ble_evt_t* p_ble_evt)
{
    // Declare buffer variable to hold received data. The data can only be 32 bit long.
    uint32_t data_buffer;
    // Populate ble_gatts_value_t structure to hold received data and metadata.
    ble_gatts_value_t rx_data;
    rx_data.len = sizeof(uint32_t);
    rx_data.offset = 0;
    rx_data.p_value = (uint8_t*) &data_buffer;
    
    if(p_ble_evt->evt.gatts_evt.params.write.handle == m_handle.char_live_handles.cccd_handle)
    {
        // Get data
        sd_ble_gatts_value_get(m_handle.conn_handle, m_handle.char_live_handles.cccd_handle, &rx_data);
    }
    else if (p_ble_evt->evt.gatts_evt.params.write.handle == m_handle.char_rate_handles.cccd_handle)
    {
        // Get data
        sd_ble_gatts_value_get(m_handle.conn_handle, m_handle.char_rate_handles.cccd_handle, &rx_data);
    }
    // Print handle and value
    if(data_buffer == 0x0001)
    {
        printf("Notification enabled\r\n");
        // TODO: Start measurement
    }
    else if(data_buffer == 0x0000)
    {
        printf("Notification disabled\r\n");
        // TODO: Stop measurement
    }
}

void ble_st_service_on_ble_evt(ble_evt_t* p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {        
        case BLE_GATTS_EVT_WRITE:
            on_ble_write(p_ble_evt);
            break;
        case BLE_GAP_EVT_CONNECTED:
            m_handle.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            m_handle.conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
        default:
            // No implementation needed.
            break;
    }
}

/**
 * Function for adding our new characterstic to the service.
 *
 * \param uuid the uuid for the new characteristic
 * \param value buffer holding the initial value for the characteristic
 * \param len lenght of the buffer \ref value (will also be the size of the characteristic)
 */
static uint32_t st_char_add(uint16_t uuid, ble_gatts_char_handles_t* char_handle, uint8_t* value, size_t len)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    // OUR_JOB: Step 2.A, Add a custom characteristic UUID
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid      = uuid;
    sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);
    
    // Add read properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    
    // Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
   
    
    // Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md)); 
    attr_md.vloc        = BLE_GATTS_VLOC_STACK;   
    // Set read security level to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
        
    // Configure the characteristic value attribute
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));        
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    
    // Set characteristic length in number of bytes
    attr_char_value.max_len     = len;
    attr_char_value.init_len    = len;
    attr_char_value.p_value     = value;

    // Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(m_handle.service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       char_handle);
    APP_ERROR_CHECK(err_code);
    
    return NRF_SUCCESS;
}


/**
 * Function for initiating the st service.
 */
void st_service_init()
{
    time_measurement_init();

    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    // Set our service connection handle to default value. I.e. an invalid handle since we are not yet in a connection.
    m_handle.conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &m_handle.service_handle);
    APP_ERROR_CHECK(err_code);
    
    uint8_t buffer[6] = {};
    // Add 6 byte characteristic for live measurement
    // 2 bytes for current minute, 4 bytes for current counter value
    // Characteristic value initialized to 0
    st_char_add(BLE_UUID_LIVE_MEASUREMENT, &m_handle.char_live_handles, buffer, sizeof(buffer));

    // Add 4 byte characteristic for rate count per minute
    // 2 byte current minute, 2 bytes tick count of last minute
    // Characteristic initialized to 0
    st_char_add(BLE_UUID_MINUTE_RATE, &m_handle.char_rate_handles, buffer, 4);

    // TODO: For later stages add characteristic for loading values from memory
}

// Function to be called when updating characteristic value
void st_live_measurement_update(live_measurement_t* measurement)
{
    // Update characteristic value
    if (m_handle.conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = sizeof(live_measurement_t);
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = m_handle.char_live_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)measurement;

        sd_ble_gatts_hvx(m_handle.conn_handle, &hvx_params);
    }   
} 

void st_minute_rate_update(minute_rate_t *rate)
{
    // Update characteristic value
    if (m_handle.conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = sizeof(live_measurement_t);
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = m_handle.char_rate_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)rate;

        sd_ble_gatts_hvx(m_handle.conn_handle, &hvx_params);
    }
}
