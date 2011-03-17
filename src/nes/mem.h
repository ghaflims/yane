#ifndef __6502_MEM_H__
#define __6502_MEM_H__

#include "yane.h"
#include <stdint.h>

typedef uint8_t prg_bank_t[16384];
typedef uint8_t chr_bank_t[8192];
typedef uint8_t playchoice_hint_t[8192];

typedef uint8_t (*mem_read_func)(uint16_t);
typedef void (*mem_write_func)(uint16_t, uint8_t);

extern mem_read_func _read_callbacks[];
extern mem_write_func _write_callbacks[];

#define mem_read(addr) _read_callbacks[addr](addr)
#define mem_write(addr, val) _write_callbacks[addr](addr, val)
#define mem_readw(addr) (_read_callbacks[addr](addr) | (_read_callbacks[addr+1](addr+1) << 8))

#define mem_add_read_func(addr, func) _read_callbacks[addr] = func
#define mem_add_write_func(addr, func) _write_callbacks[addr] = func

#define RDECL(name) uint8_t read_##name(uint16_t addr)
#define WDECL(name) void write_##name(uint16_t addr, uint8_t val)

extern prg_bank_t **prg_rom;
extern chr_bank_t **chr_rom;
extern unsigned int num_prg_banks;
extern unsigned int num_chr_banks;

int yane_mem_init();

#endif /* not __6502_MEM_H__ */

