#ifndef GPIO_MANAGEMENT_H
#define GPIO_MANAGEMENT_H

// TODO: Change to correct pin
#define PIN_PENDULUM 5

typedef void (*gpio_mgt_pendulum_handler_t)();

void
gpio_mgmt_init(gpio_mgt_pendulum_handler_t pendulum_evt_handler);

void
gpio_mgmt_start_sensing_pendulum();

void
gpio_mgmt_stop_sensing_pendulum();

#endif // GPIO_MANAGEMENT_H
