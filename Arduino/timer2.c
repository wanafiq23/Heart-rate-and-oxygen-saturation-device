#include "global.h"

#if defined(__cplusplus)
extern "C" {
#endif

volatile uint32_t SYS_TICK;

uint16_t TIMER2_Init(void) {
	TCCR2A = 0x02;
	TCCR2B = 0x04;
	OCR2A = 249;
	TIFR2 = 0x02;
	TIMSK2 = 0x02;
	SYS_TICK = 0;

	return TIMER2_SUCCESS;
	}

ISR(TIMER2_COMPA_vect) {
	SYS_TICK++;
}

void delay_ms(uint32_t ms) {
	uint32_t start_time = SYS_TICK;
	
	// Wait until the difference between current time and start time 
	// reaches the desired millisecond count
	while ((SYS_TICK - start_time) < ms) {
		// Do nothing, just wait for the ISR to increment SYS_TICK
	}
}

#if defined(__cplusplus)
}
#endif
