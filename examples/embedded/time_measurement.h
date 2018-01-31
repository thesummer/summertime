#ifndef TIME_MEASUREMENT_H
#define TIME_MEASUREMENT_H

/*
 *   The general setup for measuring the pendulum events and update the
 *   corresponding service characteristics works like this:
 *
 *                  +---------------------+
 *                  |                     |
 *                  v                     |
 *                Task                    |
 *                Clear                   |           ISR
 *               +-------------+          |    (update rate data)
 *               |     RTC1    |Event     |            ^
 *               |  (32728 Hz) |compare +-+------------+
 *               +-------------+(1 min)
 *                Event
 *                Tick
 *                 +                           ISR
 *                 |                     (update live data)
 *                 |                             ^
 *                 v                        +----+
 *                Task                      |
 *                Count                     |
 *               +-------------+            +  +----------+
 *           Task|             |Task <---+Event|          | <---+PIN_PENDULUM
 *          Clear|   Timer1    |Capture   Port |   GPIO   |
 *           ^   |             |               |          |
 *           |   +-------------+               +----------+
 *           |   Event
 *           +-+ compare
 *               (1 s)
 *                 +
 *                 |
 *                 v
 *                ISR
 *           (count to 60)
 *
 *   - RTC1 clears every minute
 *   - RTC1 triggers Timer1 count every tick, i.e. RTC1 and Timer1 are synchronous
 *   - Timer1 clears every second and keeps the count of the current second of RTC1's minute
 *   - PIN_PENDULUM triggers a port event when the pendulum passes the light sensor
 *   - Port event triggers task capture1 of Timer1 --> The time of the port event is saved
 *   - Port event triggers ISR which takes the captured time, triggers live data updaet and
 *     increases the pendulum counter for the current minute
 *   - RTC1 Event compare triggers ISR which takes the count of the last minute and
 *     triggers rate data update
 */

void
time_measurement_init();

void
time_measurement_start();

void
time_measurement_stop();

#endif // TIME_MEASUREMENT_H
