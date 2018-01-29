#ifndef SOFT_DEVICE_THREAD_H
#define SOFT_DEVICE_THREAD_H

#include <outpost/rtos/thread.h>
#include <outpost/rtos/semaphore.h>
#include <outpost/time/duration.h>

extern "C"
{
#include <ble.h>
#include <ble_advertising.h>
}


class SoftDeviceThread : public outpost::rtos::Thread
{
public:
    static constexpr const char* DeviceName = "Summertime";
    static const outpost::time::Duration MinConnectionInverval;
    static const outpost::time::Duration MaxConnectionInverval;
    static const outpost::time::Duration ConnectionTimeoutInverval;
    static const outpost::time::Duration AdvertisingInterval;
    static const outpost::time::Duration AdvertisingTimeout;
    SoftDeviceThread();
    
    void
    initialize();

    virtual void
    run();


private:
    static uint32_t
    handleNewBleEvent()
    {
        mEventSemaphore.release();
    }

    /**
     *  The nrf51-SDK expects the interval in units of 1.25 ms in us
     */
    static inline uint16_t
    connectionIntervalToUnits(outpost::time::Duration interval)
    {
        return interval.milliseconds() * 1250;
    }

    /**
     *  The nrf51-SDK expects the timeout in units of 10 ms in us
     */
    static inline uint16_t
    timeoutIntervalToUnits(outpost::time::Duration interval)
    {
        return interval.milliseconds() * 10000;
    }

    /**
     *  The nrf51-SDK expects the advertising interval in units of 0.625 ms in us
     */
    static inline uint16_t
    advertisingIntervalToUnits(outpost::time::Duration interval)
    {
        return static_cast<uint16_t>(interval.milliseconds() * 1.6);
    }

    static void
    dispatchBleEvent(ble_evt_t* p_ble_evt);

    void
    initializeGapParameters();

    void
    initializeAdvertising();

    static outpost::rtos::Semaphore mEventSemaphore;
};


#endif // SOFT_DEVICE_THREAD_H
