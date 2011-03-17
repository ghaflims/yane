#include "yane.h"

int
yane_ines_load(const char *file)
{
	FILE *f;
	ines_header_t h;
	
	yane_verbose("INES", "Loading iNES file `%s'", file);
	
	f = yane_fopen(file, "rb");
	
	/* From all the code I've compiled it seems that nobody every checks
	   the return value of fread... */
	if(fread(&h, sizeof(ines_header_t), 1, f) < 1)
	{
		fclose(f);
		yane_error("file is too small or corrupt");
		return -1;
	}
	
	if(h[0] != 'N' || h[1] != 'E' || h[2] != 'S' || h[3] != 0x1A)
	{
		fclose(f);
		yane_error("file is not a valid iNES ROM");
		return -1;
	}
	
	num_prg_banks = h[4];
	num_chr_banks = h[5];
	
	yane_verbose("INES", "PRG-ROM: %u bank(s)\tCHR-ROM: %u bank(s)",
		num_prg_banks, num_chr_banks);
	
	if(num_prg_banks == 0)
	{
		yane_error("invalid number of PRG-ROM banks");
		fclose(f);
		return -1;
	}
	
	if(num_chr_banks == 0)
	{
		yane_warn("bad number of CHR-ROM banks");
		num_chr_banks++; /* To keep the mappers happy */
	}
	
	prg_rom = yane_malloc(num_prg_banks * sizeof(prg_bank_t*));
	chr_rom = yane_malloc(num_chr_banks * sizeof(chr_bank_t*));

	fclose(f);
	return 0;
}

int
yane_ines_unload()
{
	if(prg_rom != NULL)
	{
		free(prg_rom);
	}
	
	if(chr_rom != NULL)
	{
		free(chr_rom);
	}
	
	return 0;
}

