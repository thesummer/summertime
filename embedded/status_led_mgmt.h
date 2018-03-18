#ifndef STATUS_LED_MGMT_H
#define STATUS_LED_MGMT_H

// TODO: Change to correct pin
#define PIN_POWER    7
#define PIN_RESET    8

typedef enum
{
    LED_STATUS_NONE,
    LED_STATUS_POWER_UP,            // power led continuous
    LED_STATUS_IDLE,                // toggle reset led every 1s
    LED_STATUS_ADVERTISING,         // power led toggle every 100ms
    LED_STATUS_CONNECTED,           // toggle power led every 1 s
    LED_STATUS_START_MEASUREMENT,   // both leds toggling every 50 ms for 1s
    LED_STATUS_ACTIVE_MEASUREMENT,  // tick both leds alternating on every tick for 50/100ms
    LED_STATUS_STOP_MEASUREMENT,    // both leds on for 1s
    LED_STATUS_ERROR                // both leds on
} led_status_t;

typedef void (*gpio_mgt_status_evt_handler_t)(led_status_t status);

void
status_led_mgmt_init(gpio_mgt_status_evt_handler_t callback);

void
status_led_mgmt_set_status(led_status_t status);


#endif // STATUS_LED_MGMT_H
