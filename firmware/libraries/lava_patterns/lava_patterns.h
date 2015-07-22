#ifndef LAVA_PATTERNS_H
#define LAVA_PATTERNS_H


typedef enum {
	kLavaPatternMatrix = 0,
	kLavaPatternLines,
	kLavaPatternFlow,
	kLavaPatternVector,
	kLavaPatternSun,
	kLavaPatternLoop,
	kLavaPatternOff,
	kLavaPatternBlocks,
	kLavaPatternBadFlash,
	kLavaPatternBadRead,
	kLavaPatternEndOfList
}pattern_t;

void lava_patterns_init(void);
void lava_patterns_load(void);
void lava_patterns_save(void);
void lava_patterns_step(void);
void lava_patterns_next_pattern(void);
#endif

