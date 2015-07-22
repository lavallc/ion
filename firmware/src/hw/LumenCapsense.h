//
//  LumenCapsense.h
//  firmware
//
//  Created by Billy Lindeman on 1/20/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#ifndef __LumenCapsense__
#define __LumenCapsense__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>

extern "C" {
    #include "mpr121.h"
    #include "twi_master.h"
    #include "nrf_delay.h"
    #include "math.h"
}

using namespace std;

typedef function<void()> LumenCapsenseEvent;

class CLumenCapsense {
private:
    uint16_t lumen_capsense_ambient_setpoint;

public:
    CLumenCapsense();
    void init();
    bool is_touched();

    list<LumenCapsenseEvent> event_handlers;
    void add_on_touch_handler(LumenCapsenseEvent evt);
    void remove_on_touch_handler(LumenCapsenseEvent evt);

    uint16_t read();
    uint8_t get_proximity();


};

extern CLumenCapsense LumenCapsense;

#endif /* defined(__LumenCapsense__) */
