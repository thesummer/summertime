#ifndef ST_SERVICE_H_
#define ST_SERVICE_H_

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

// FROM_SERVICE_TUTORIAL: Defining 16-bit service and 128-bit base UUIDs
#define BLE_UUID_OUR_BASE_UUID              {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID
#define BLE_UUID_OUR_SERVICE_UUID                0xF00D // Just a random, but recognizable value

// ALREADY_DONE_FOR_YOU: Defining 16-bit characteristic UUID
#define BLE_UUID_LIVE_MEASUREMENT      0x0A0A
#define BLE_UUID_MINUTE_RATE           0x0B0B

typedef struct
{
    uint16_t minute;
    uint32_t counter;
} live_measurement_t;

typedef struct
{
    uint16_t minute;
    uint16_t rate_count;
} minute_rate_t;

/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_st_service_on_ble_evt(ble_evt_t* p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void st_service_init();

/**
 * Function for updating and sending the live measurement value
 *
 * \param[in]   measurement     New live measurement value
 */
void st_live_measurement_update(live_measurement_t* measurement);

/**
 * Function for updating and sending the live measurement value
 *
 * \param[in]   measurement     New live measurement value
 */
void st_minute_rate_update(minute_rate_t* rate);

#endif  /* _ ST_SERVICE_H_ */
