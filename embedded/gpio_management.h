#ifndef GPIO_MANAGEMENT_H
#define GPIO_MANAGEMENT_H

// TODO: Change to correct pin
#define PIN_PENDULUM 5
#define PIN_WAKE_UP  6

typedef void (*gpio_mgt_evt_handler_t)();

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

#endif // GPIO_MANAGEMENT_H
