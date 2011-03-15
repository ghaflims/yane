#ifndef __6502_CPU_H__
#define __6502_CPU_H__

#include "yane.h"
#include <stdint.h>

#define F_NEG 0x80
#define F_OVR 0x40
#define F_XXX 0x20
#define F_BRK 0x10
#define F_DEC 0x08
#define F_CLI 0x04
#define F_ZER 0x02
#define F_CAR 0x00

typedef struct
{
	uint8_t a, x, y;
	uint8_t s, p;
	uint16_t pc;
	/* When a 16-bit value is loaded,
		the processor keeps the low byte
		in this register
	 */
	uint8_t dbtmp;
	/* Store interrupt vectors here
		to keep from having to read them
		whenever an interrupt is asserted */
	uint16_t nmi_vector, irq_vector, reset_vector;
	int countdown;
} x6502;

int yane_cpu_init();
int yane_cpu_power();
int yane_cpu_reset();

#endif /* not __6502_CPU_H__ */

