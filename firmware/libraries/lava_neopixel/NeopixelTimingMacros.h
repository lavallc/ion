//
//  CNeopixelTimingMacros.h
//  firmware
//
//  Created by Billy Lindeman on 1/17/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#ifndef _CNeopixelTimingMacros_h
#define _CNeopixelTimingMacros_h


//These defines are timed specific to a series of if statements and will need to be changed
//to compensate for different writing algorithms than the one in neopixel.c
#define NEOPIXEL_TIMING_MACRO_SEND_ONE	NRF_GPIO->OUTSET = (1UL << PIN); \
__ASM ( \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
); \
NRF_GPIO->OUTCLR = (1UL << PIN); \
__ASM ( \
" NOP\n\t" \
" NOP\n\t" \
);

#define NEOPIXEL_TIMING_MACRO_SEND_ZERO NRF_GPIO->OUTSET = (1UL << PIN); \
__ASM (  \
" NOP\n\t"  \
" NOP\n\t"  \
);  \
NRF_GPIO->OUTCLR = (1UL << PIN);  \
__ASM ( \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
" NOP\n\t" \
);

#endif
