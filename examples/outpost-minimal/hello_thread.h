#ifndef HELLO_THRAD_H
#define HELLO_THRAD_H

#include <outpost/rtos/thread.h>

extern "C"
{
#include "esp_common.h"
}

class HelloThread : public outpost::rtos::Thread
{
public:
  
    HelloThread() :
      outpost::rtos::Thread(1, 1024, "test")
    {}
    
    virtual void
    run()
    {
        while(1)
        {
          os_printf("Hallo Welt!!\n");
          outpost::rtos::Thread::sleep(outpost::time::Seconds(1));
        }
    }
};


#endif // HELLO_THRAD_H
