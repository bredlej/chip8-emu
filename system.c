#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

CHIP8 *init_chip8() {
	CHIP8 *chip8 = malloc(sizeof *chip8);
	if (chip8 == NULL)
		return NULL;	
	
	/* clear whole memory */
	for (int i = 0; i < MEMORY_SIZE; i++) {
		chip8->memory[i] = 0x00; 
	}

	/* fill memory 0x0000 - 0x004F with font data*/
	for (int i = 0; i < FONT_AMOUNT; i++) {
		for (int j = 0; j < FONT_SIZE; j++) {		
			chip8->memory[j + (i * FONT_SIZE)] = font[i][j];
		}
	}

	chip8->pc = MEMORY_PROGRAM_START;
	chip8->I = 0;
	chip8->sp = 0;

	return chip8;
}

int free_memory(CHIP8* chip8) {
	free(chip8);

	return 0;
}

int dump_memory(uint8_t* memory) {
	uint16_t address = 0x00;
	for (uint16_t i = 0; i < MEMORY_SIZE; i++) {

		if (i % 0x0F == 0) {
			printf("\n%04X: ", address);
			address += 0x0F;
		}
		printf("%04X ", memory[i]);
	}

	return 0;
}

int main(int argc, char** argv) {
	CHIP8 *chip8 = init_chip8();	
	
	FILE* rom_file = fopen("roms/PONG", "rb");
	if (!rom_file) {
		printf("Can't open file.\n");
	}
	else {
		uint8_t buffer;
		uint16_t memory_address = MEMORY_PROGRAM_START;
		while (fread(&buffer, sizeof buffer, 1, rom_file)) {
			chip8->memory[memory_address++] = buffer;
		};
		fclose(rom_file);
	}
	
	if (dump_memory(&chip8->memory)) printf("Error dumping memory.\n");

	free_memory(chip8);
	return 0;
}