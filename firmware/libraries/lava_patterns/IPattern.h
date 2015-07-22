//
//  IPattern.h
//  firmware
//
//  Created by Billy Lindeman on 1/18/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#ifndef _IPattern_h
#define _IPattern_h
#include <cstdint>

	typedef struct {
		uint16_t data;
		uint8_t config_id;
	} LumenConfigData;

	typedef struct {
		LumenConfigData config_data[10];
		uint32_t mood_id;
	} LumenMoodConfig;

class IPattern {
private:

public:
    virtual uint32_t get_pattern_id();
    virtual void get_pattern_config(LumenMoodConfig * configs);
    virtual void start(LumenMoodConfig * configs, bool restore_to_defaults);
    virtual void step();
    virtual void handle_config_data(uint8_t config_id, uint32_t data);
};

#endif
