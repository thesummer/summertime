#ifndef GPIO_MANAGEMENT_H
#define GPIO_MANAGEMENT_H

// TODO: Change to correct pin
#define PIN_PENDULUM 5
#define PIN_WAKE_UP  6
#define PIN_POWER    7
#define PIN_RESET    8

typedef enum
{
    LED_STATUS_NONE,
    LED_STATUS_POWER_UP,            // power led continuous
    LED_STATUS_IDLE,                // toggle reset led every 1s
    LED_STATUS_ADVERTISING,         // power led toggle every 100ms
    LED_STATUS_START_MEASUREMENT,   // both leds toggling every 50 ms for 1s
    LED_STATUS_ACTIVE_MEASUREMENT,  // tick both leds alternating on every tick for 50/100ms
    LED_STATUS_STOP_MEASUREMENT,    // both leds on for 1s
    LED_STATUS_ERROR                // both leds on
} led_status_t;

typedef void (*gpio_mgt_evt_handler_t)();
typedef void (*gpio_mgt_status_evt_handler_t)(led_status_t);

void
gpio_mgmt_pendulum_init(gpio_mgt_evt_handler_t pendulum_evt_handler);

void
gpio_mgmt_pendulum_start_sensing();

void
gpio_mgmt_pendulum_stop_sensing();

void
gpio_mgmt_wakeup_init(gpio_mgt_evt_handler_t wake_up_evt_handler);

void
gpio_mgmt_wakeup_start_sensing();

void
gpio_mgmt_wakeup_stop_sensing();

void
gpio_mgmt_status_init(gpio_mgt_status_evt_handler_t callback);

void
gpio_mgmt_set_status(led_status_t status);

#endif // GPIO_MANAGEMENT_H
