#include "yane.h"

mem_read_func _read_callbacks[65536];
mem_write_func _write_callbacks[65536];

/* RAM is global because the CPU emulation optimizes zero page
   and stack accesses by going to it directly, with no need to
   call the callback function */
uint8_t ram[0x800];

/* Use an array of pointers to PRG and CHR ROM banks 
	in order to simplify bank-switching */
prg_bank_t **prg_rom;
chr_bank_t **chr_rom;
unsigned int num_prg_banks;
unsigned int num_chr_banks;

/* null callback read and writes; basically, do nothing */
RDECL(null)
{
	return 0;
}

WDECL(null)
{
}

/* Read and write NES ram */
RDECL(ram)
{
	return ram[addr];
}

WDECL(ram)
{
	ram[addr] = val;
}

/* Read and write NES ram, mirrors */
RDECL(ram2)
{
	return ram[addr - 0x800];
}

WDECL(ram2)
{
	ram[addr - 0x800] = val;
}

RDECL(ram3)
{
	return ram[addr - 0x1000];
}

WDECL(ram3)
{
	ram[addr - 0x1000] = val;
}

RDECL(ram4)
{
	return ram[addr - 0x1800];
}

WDECL(ram4)
{
	ram[addr - 0x1800] = val;
}

int
yane_mem_init()
{
	unsigned int i;
	
	/* These are set up by the ROM file loader */
	prg_rom = NULL; chr_rom = NULL;
	num_prg_banks = num_chr_banks = 0;
	
	/* Fill RAM read and write callbacks */
	for(i = 0x0000; i < 0x800; i++)
	{
		_read_callbacks[i] = read_ram;
		_write_callbacks[i] = write_ram;
		_read_callbacks[i + 0x800] = read_ram2;
		_write_callbacks[i + 0x800] = write_ram2;
		_read_callbacks[i + 0x1000] = read_ram3;
		_write_callbacks[i + 0x1000] = write_ram3;
		_read_callbacks[i + 0x1800] = read_ram4;
		_write_callbacks[i + 0x1800] = write_ram4;
	}
	
	/* All of the other callbacks are null;
	   They will be initialized by their subsystem */
	for(i = 0x2000; i <= 0xFFFF; i++)
	{
		_read_callbacks[i] = read_null;
		_write_callbacks[i] = write_null;
	}
	
	return 0;
}

