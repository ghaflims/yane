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

/* Addressing modes */
#define IMM {x.db = mem_read(x.pc); x.pc++;}
#define ZP {x.dp = mem_read(x.pc); x.db = ram[x.dp]; x.pc++;}
#define ZPX {x.dp = mem_read(x.pc) + x.x; x.db = ram[x.dp]; x.pc++;}
#define ZPY {x.dp = mem_read(x.pc) + x.y; x.db = ram[x.dp]; x.pc++;}
#define ABS {x.dw = mem_readw(x.pc); x.db = mem_read(x.dw); x.pc += 2;}
#define ABS2 {x.dw = mem_readw(x.pc); x.pc += 2;}

/* Addressing mode write-backs */
#define WZP {ram[x.dp] = x.db;}
#define WABS {ram[x.dw] = x.db;}

/* Flag operations */
#define SET_ZN(val) {x.p &= ~(F_NEG | F_ZER); x.p |= zn_table[(val)];}
#define CLC() {x.p &= ~F_CAR; CYC(2);}
#define SEC() {x.p |= F_CAR; CYC(2); }
#define CLD() {x.p &= ~F_DEC; CYC(2);}
#define SED() {x.p |= F_DEC; CYC(2); }
#define CLI() {x.p &= ~F_CLI; CYC(2);}
#define SEI() {x.p |= F_CLI; CYC(2);}
#define CLV() {x.p &= ~F_OVR; CYC(2);}

/* Misc. operations */
#define CYC(amt) x.countdown -= (amt)
#define FETCH() mem_read(x.pc)
#define NOP() CYC(2)

/* Stack operations */
#define PUSH(val) {ram[S_BASE + x.s--] = (val);}
#define PUSHW(val) {PUSH((val >> 8) & 0xFF); PUSH(val & 0xFF);}
#define POP() ram[S_BASE + ++x.s]
#define POPW() (POP() | (POP() << 8))
#define TXS() {x.s = x.x; CYC(2);}
#define TSX() {x.x = x.s; CYC(2);}
#define PHA() {PUSH(x.a); CYC(3);}
#define PLA() {x.a = POP(); CYC(4);}
#define PHP() {PUSH(x.p); CYC(3);}
#define PLP() {x.p = POP(); CYC(4);}

/* Control transfer operations */
#define RTS() {x.pc = POPW() + 1; CYC(6);}
#define RTI() {x.p = POP(); x.pc = POPW(); CYC(6);}
#define JMP() {x.pc = x.dw;}
#define JSR() {x.dw = mem_readw(x.pc); ++x.pc; PUSHW(x.pc); x.pc = x.dw; CYC(6);}

/* Data transfer operations */
#define LDA() {x.a = x.db; SET_ZN(x.a);}
#define LDX() {x.x = x.db; SET_ZN(x.x);}
#define LDY() {x.y = x.db; SET_ZN(x.y);}
#define TAX() {x.x = x.a; SET_ZN(x.x); CYC(2);}
#define TXA() {x.a = x.x; SET_ZN(x.a); CYC(2);}
#define DEX() {--x.x; SET_ZN(x.x); CYC(2);}
#define INX() {++x.x; SET_ZN(x.x); CYC(2);}
#define TAY() {x.y = x.a; SET_ZN(x.y); CYC(2);}
#define TYA() {x.a = x.y; SET_ZN(x.a); CYC(2);}
#define DEY() {--x.y; SET_ZN(x.y); CYC(2);}
#define INY() {++x.y; SET_ZN(x.y); CYC(2);}

/* ALU operations */
#define AND() {x.a &= x.db; SET_ZN(x.a);}
#define ASLA() {if(x.a & 0x80) x.p |= F_CAR; else x.p &= ~F_CAR; x.a <<=1; SET_ZN(x.a);}
#define ASL() {if(x.db & 0x80) x.p |= F_CAR; else x.p &= ~F_CAR; x.db <<=1; SET_ZN(x.db);}
#define BIT() {x.db &= x.a; SET_ZN(x.db); if(x.db & 0x40) x.p |= F_OVR; else x.p &= ~F_OVR;}
#define DEC() {--x.db; SET_ZN(x.db);}
#define EOR() {x.a ^= x.db; SET_ZN(x.a);}
#define INC() {++x.db; SET_ZN(x.db);}
#define LSRA() {if(x.a & 1) x.p |= F_CAR; else x.p &= ~F_CAR; x.a >>= 1; SET_ZN(x.a);}
#define LSR() {if(x.db & 1) x.p |= F_CAR; else x.p &= ~F_CAR; x.db >>= 1; SET_ZN(x.db);}
#define ORA() {x.a |= x.db; SET_ZN(x.a);}

/* Setup internal data structures that may be used by the 6502 emulation */
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
	
	x.countdown = 0;
	
	return 0;
}

/* Simulate assertion of the CPU's RES pin */
int
yane_cpu_power()
{
	yane_verbose("CPU", "Powered on");

	x.p = F_XXX | F_BRK | F_CLI;
	x.a = x.x = x.y = 0x00;
	x.s = 0xFF;
	CYC(6);
	
	return yane_cpu_reset();
}

int
yane_cpu_reset()
{
	yane_verbose("CPU", "Reset");

	x.p |= F_CLI;
	x.s -= 2;		/* Hopefully nothing depends on this */
	x.pc = mem_readw(RESET_VECT);
	return 0;
}

/* Software interrupt */
void
yane_cpu_brk()
{
	/* This shouldn't cause a performance hit 
	   because most games don't use BRK.
	   It will also aid in debugging, since the BRK
	   opcode of $00 will often signify uninitialized memory
	   being jumped to */
	yane_verbose("CPU", "Software BRK");

	x.pc++;			/* Skip over the extra byte after the BRK opcode */
	PUSHW(x.pc);
	x.p |= F_BRK;
	PUSH(x.p);
	x.p |= F_CLI;
	x.pc = mem_readw(BRK_VECT);
	CYC(7);
}

/* Simulate assertion of IRQ pin */
void
yane_cpu_irq()
{
	/* IRQs can be masked by this flag */
	if(x.p & F_CLI)
		return;

	PUSHW(x.pc);
	PUSH(x.p);
	x.p |= F_CLI;
	x.pc = mem_readw(IRQ_VECT);
	CYC(7);
}

/* Simulate assertion of NMI pin */
void
yane_cpu_nmi()
{
	PUSHW(x.pc);
	PUSH(x.p);
	x.p |= F_CLI;
	x.pc = mem_readw(NMI_VECT);
	CYC(7);
}

int
yane_cpu_run(int cycles)
{
	uint8_t op;
	
	/* For more accurate cycle counting */
	x.countdown += cycles;

	while(x.countdown > 0)
	{
		op = FETCH();
		
		switch(op)
		{
			case 0x00: yane_cpu_brk(); break;
			case 0x05: ZP ORA(); CYC(2); break;
			case 0x06: ZP ASL(); WZP; CYC(5); break;
			case 0x08: PHP(); break;
			case 0x09: IMM ORA(); CYC(2); break;
			case 0x0A: ASLA(); CYC(2); break;
			case 0x0D: ABS ORA(); CYC(4); break;
			case 0x0E: ABS ASL(); WABS; CYC(6); break;
			case 0x15: ZPX ORA(); CYC(3);
			case 0x16: ZPX ASL(); WZP; CYC(6); break;
			case 0x18: CLC(); break;
			case 0x20: JSR(); break;
			case 0x24: ZP BIT(); CYC(3); break;
			case 0x25: ZP AND(); CYC(2); break;
			case 0x28: PLP(); break;
			case 0x29: IMM AND(); CYC(2); break;
			case 0x2C: ABS BIT(); CYC(4); break;
			case 0x2D: ABS AND(); CYC(4); break;
			case 0x35: ZPX AND(); CYC(3); break;
			case 0x38: SEC(); break;
			case 0x40: RTI(); break;
			case 0x45: ZP EOR(); CYC(3); break;
			case 0x46: ZP LSR(); WZP; CYC(5); break;
			case 0x48: PHA(); break;
			case 0x49: IMM EOR(); CYC(2); break;
			case 0x4A: LSRA(); CYC(2); break;
			case 0x4C: ABS2 JMP(); CYC(3); break;
			case 0x4D: ABS EOR(); CYC(4); break;
			case 0x4E: ABS LSR(); WABS; CYC(6); break;
			case 0x55: ZPX EOR(); CYC(4); break;
			case 0x56: ZPX LSR(); WZP; CYC(6);
			case 0x58: CLI(); break;
			case 0x60: RTS(); break;
			case 0x68: PLA(); break;
			case 0x78: SEI(); break;
			case 0x88: DEY(); break;
			case 0x8A: TXA(); break;
			case 0x98: TYA(); break;
			case 0x9A: TXS(); break;
			case 0xA0: IMM LDY(); CYC(2); break;
			case 0xA2: IMM LDX(); CYC(2); break;
			case 0xA4: ZP LDY(); CYC(3); break;
			case 0xA5: ZP LDA(); CYC(3); break;
			case 0xA6: ZP LDX(); CYC(3); break;
			case 0xA8: TAY(); break;
			case 0xA9: IMM LDA(); CYC(2); break;
			case 0xAA: TAX(); break;
			case 0xAC: ABS LDY(); CYC(4); break;
			case 0xAD: ABS LDA(); CYC(4); break;
			case 0xAE: ABS LDX(); CYC(4); break;
			case 0xB4: ZPX LDY(); CYC(4); break;
			case 0xB5: ZPX LDA(); CYC(4); break;
			case 0xB6: ZPY LDX(); CYC(4); break;
			case 0xBA: TSX(); break;
			case 0xC6: ZP DEC(); WZP; CYC(5); break;
			case 0xC8: INY(); break;
			case 0xCA: DEX(); break;
			case 0xCE: ABS DEC(); WABS; CYC(6); break;
			case 0xD6: ZPX DEC(); WZP; CYC(6); break;
			case 0xD8: CLD(); break;
			case 0xE6: ZP INC(); WZP; CYC(5); break;
			case 0xE8: INX(); break;
			case 0xEA: NOP(); break;
			case 0xEE: ABS INC(); WABS; CYC(6); break;
			case 0xF6: ZPX INC(); WZP; CYC(6); break;
			case 0xF8: SED(); break;
			
			/* Invalid/unimplemented opcode */
			default: return 1;
		}
	}
	
	return x.countdown;
}

