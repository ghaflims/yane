/* src/cpu/cpu.c - 6502 CPU core
 *
 * Copyright (c) 2011 Tosi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "yane.h"
#include "cpu.h"

static x6502 x;
static uint8_t zn_table[256];

extern uint8_t ram[0x800];

/* All stack operations on this page; the S register
   just wraps around at $00 or $FF */
#define S_BASE 0x0100

/* Define CPU operations as macros for speed.
   The only tradeoff is an increase in generated code size,
   which may cause slowdown on systems with a small cache size */
   
/* Flag operations */
#define SET_ZN(val) {x.p &= ~(F_NEG | F_ZER); x.p |= zn_table[(val)];}
#define SET_CAR_ADD(val) {x.p &= ~F_CAR; if(val > 0xFF) x.p |= F_CAR;}
#define SET_CAR_SUB(val) {x.p &= ~F_CAR; if((int16_t)val < 0) x.p |= F_CAR;}
#define CLC() {x.p &= ~F_CAR;}
#define CLD() {x.p &= ~F_DEC;}
#define CLI() {x.p &= ~F_CLI;}
#define CLV() {x.p &= ~F_OVR;}
#define SEI() {x.p |= F_CLI;}

/* Misc. operations */
#define FETCH() mem_read(x.pc)

/* Stack operations */
#define PUSH(val) {ram[S_BASE + x.s--] = (val);}
#define POP() ram[S_BASE + ++x.s]

/* Data transfer operations */
#define LDA(val) (x.a = (val); SET_ZN(x.a);}
#define LDX(val) {x.x = (val); SET_ZN(x.x);}
#define LDY(val) {x.y = (val); SET_ZN(x.y);}
#define STA_ZP(zpa) {ram[zpa] = x.a;}
#define STA_ABS(addr) {mem_write(addr, x.a);}

/* ALU operations */
#define AND(val) {x.a &= (val); SET_ZN(x.a);}

int
yane_cpu_init()
{
	unsigned int i;
	
	yane_verbose("CPU", "Initialization");
	
	/* Fill in the table of Z/N flags. */
	for(i = 0; i <= 0xFF; i++)
	{
		if(i == 0)
			zn_table[i] = F_ZER;
		else if(i & 0x80)
			zn_table[i] = F_NEG;
		else
			zn_table[i] = 0;
	}
	
	return 0;
}

int
yane_cpu_power()
{
	yane_verbose("CPU", "Powered on");

	x.p = F_XXX | F_BRK | F_CLI;
	x.a = x.x = x.y = 0x00;
	x.s = 0xFF;
	
	return 0;
}

int
yane_cpu_reset()
{
	yane_verbose("CPU", "Reset");

	x.p |= F_CLI;
	
	return 0;
}

int
yane_cpu_run(int cycles)
{
	uint8_t op;
	
	x.countdown = cycles;

	while(x.countdown > 0)
	{
		op = FETCH();
		
		switch(op)
		{
			case 0x00: break;
			case 0x18: CLC(); break;
			case 0x58: CLI(); break;
			case 0xD8: CLD(); break;
			
			/* Invalid/unimplemented opcode */
			default: return 1;
		}
	}
	
	return x.countdown;
}

