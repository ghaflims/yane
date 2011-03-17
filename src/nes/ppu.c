#include "yane.h"

/* First control register */
WDECL(ppu2000)
{
}

/* Second control register */
WDECL(ppu2001)
{
}

/* PPU status register */
RDECL(ppu2002)
{
	return 0;
}

/* PPU Sprite index */
WDECL(ppu2003)
{
}

/* PPU Sprite data */
WDECL(ppu2004)
{
}

/*         VRAM address register 1
	This one controls VRAM addressing lines */
WDECL(ppu2005)
{
}

/*         VRAM address register 2
	This one controls the internal VRAM address */
WDECL(ppu2006)
{
}

/* VRAM data register, both readable and writeable */
WDECL(ppu2007)
{
}

RDECL(ppu2007)
{
	return 0;
}

int
yane_ppu_init()
{
	unsigned int i;
	
	yane_verbose("PPU", "Initialization");

	/* Install PPU register callbacks, the registers
		are mirrored every 8 bytes starting at $2000 */
	for(i = 0x2000; i < 0x4000; i+= 8)
	{
		mem_add_write_func(i, write_ppu2000);
		mem_add_write_func(i+1, write_ppu2001);
		mem_add_read_func(i+2, read_ppu2002);
		mem_add_write_func(i+3, write_ppu2003);
		mem_add_write_func(i+4, write_ppu2004);
		mem_add_write_func(i+5, write_ppu2005);
		mem_add_write_func(i+6, write_ppu2006);
		mem_add_write_func(i+7, write_ppu2007);
		mem_add_read_func(i+7, read_ppu2007);
	}

	return 0;
}

void
yane_ppu_power()
{
}

void
yane_ppu_reset()
{
}

