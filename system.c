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

	/* clear registers */
	for (int i = 0; i < 16; i++)
		chip8->registers[i] = 0x00;

	/* fill memory 0x0000 - 0x004F with font data*/
	for (int i = 0; i < FONT_AMOUNT; i++) {
		for (int j = 0; j < FONT_SIZE; j++) {		
			chip8->memory[j + (i * FONT_SIZE)] = font[i][j];
		}
	}

	chip8->pc = MEMORY_PROGRAM_START;
	chip8->I = 0;
	chip8->sp = 0;

	OPCODES *opcodes;
	opcodes = malloc(sizeof * opcodes);
	if (!opcodes) return NULL;
	opcodes->_6XNN = _6XNN;
	opcodes->_8XY0 = _8XY0;

	chip8->opcodes = opcodes;

	return chip8;
}

/** Store value at register Vx */
void _6XNN(uint32_t *p_VX, uint8_t value) {
	*(p_VX) = value;
}

/** Store value of Vy at register Vx */
void _8XY0(uint32_t* p_VX, uint32_t* p_VY) {
	*(p_VX) = (*p_VY);
}

int free_memory(CHIP8 *chip8) {
	free(chip8);

	return 0;
}

int dump_memory(uint8_t *memory) {
	uint16_t address = 0x00;
	for (uint16_t i = 0; i < MEMORY_SIZE; i++) {

		if (i % 0x0F == 0) {
			printf("\n%04X: ", address);
			address += 0x0F;
		}
		printf("%02X ", memory[i]);
	}

	return 0;
}

int dump_registers(CHIP8* chip8) {
	printf("Registers\n");
	for (int i = 0; i < 16; i++) {
		printf("V%01X=[%02X]\n", i, chip8->registers[i]);
	}

	return 0;
}

uint16_t fetch_opcode(CHIP8* chip8, uint16_t address) {
	uint16_t opcode = chip8->memory[address] << 8;
	opcode |= chip8->memory[address + 1];
	
	return opcode;
}


int parse_opcodes(CHIP8 *chip8) {
	
	uint16_t opcode;
	char decoded[50];	
	for (uint16_t address = MEMORY_PROGRAM_START; address < MEMORY_SIZE; address = address + 2) {
		opcode = fetch_opcode(chip8, address);

		if (OPCODE_GROUP(opcode) == 0x0000) {
			if (NN(opcode) == 0x00E0) 
				sprintf(decoded, "clear_display()");
			else if (NN(opcode) == 0x00EE)
				sprintf(decoded, "return");
			else
				sprintf(decoded, "call NNN");
		}
		else if (OPCODE_GROUP(opcode) == 0x1000) {
			sprintf(decoded, "goto $%04X", NNN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x2000) {
			sprintf(decoded, "*($%04X)()", NNN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x3000) {
			sprintf(decoded, "if (V%01X == %02X)", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x4000) {
			sprintf(decoded, "if (V%01X != %02X)", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x5000) {
			sprintf(decoded, "if (V%01X == V%01X)", VX(opcode), VY(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x6000) {			
			chip8->opcodes->_6XNN(&chip8->registers[VX(opcode)], NN(opcode));
			sprintf(decoded, "V%01X = %02X", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x7000) {
			sprintf(decoded, "V%01X += %02X", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x8000) {
			if ((N(opcode)) == 0x0000) {
				chip8->opcodes->_8XY0(&chip8->registers[VX(opcode)], &chip8->registers[VY(opcode)]);
				sprintf(decoded, "V%01X = V%01X", VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0001) {
				sprintf(decoded, "V%01X = V%01X OR V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0002) {
				sprintf(decoded, "V%01X = V%01X AND V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0003) {
				sprintf(decoded, "V%01X = V%01X XOR V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0004) {
				sprintf(decoded, "V%01X = V%01X + V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0005) {
				sprintf(decoded, "V%01X = V%01X - V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((N(opcode)) == 0x0006) {
				sprintf(decoded, "V%01X = V%01X >> 1", VX(opcode), VX(opcode));
			}
			else if ((N(opcode)) == 0x0007) {
				sprintf(decoded, "V%01X = V%01X - V%01X", VX(opcode), VY(opcode), VX(opcode));
			}			
			else if ((N(opcode)) == 0x000E) {
				sprintf(decoded, "V%01X = V%01X << 1", VX(opcode), VX(opcode));
			}
		}
		else if (OPCODE_GROUP(opcode) == 0x9000) {
			sprintf(decoded, "if (V%01X != V%01X)", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0xA000) {
			sprintf(decoded, "I = $%04X", NNN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0xB000) {
			sprintf(decoded, "PC = V0 + $%04X", NNN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0xC000) {
			sprintf(decoded, "V%01X = rnd() AND %02X", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0xD000) {
			sprintf(decoded, "draw(V%01X, V%01X, %01X)", VX(opcode), VY(opcode), N(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0xE000) {
			if (NN(opcode) == 0x009E) {
				sprintf(decoded, "if (key() == V%01X)", VX(opcode));
			}
			else if (NN(opcode) == 0x00A1) {
				sprintf(decoded, "if (key() != V%01X)", VX(opcode));
			}
		}
		else if (OPCODE_GROUP(opcode) == 0xF000) {
			if (NN(opcode) == 0x0007) {
				sprintf(decoded, "V%01X = get_delay_timer()", VX(opcode));
			}
			else if (NN(opcode) == 0x000A) {
				sprintf(decoded, "V%01X = get_key()", VX(opcode));
			}
			else if (NN(opcode) == 0x0015) {
				sprintf(decoded, "set_delay_timer(V%01X)", VX(opcode));
			}
			else if (NN(opcode) == 0x0018) {
				sprintf(decoded, "set_sound_timer(V%01X)", VX(opcode));
			}
			else if (NN(opcode) == 0x001E) {
				sprintf(decoded, "I = I + V%01X", VX(opcode));
			}
			else if (NN(opcode) == 0x0029) {
				sprintf(decoded, "I = sprite_addr[V%01X]", VX(opcode));
			}
			else if (NN(opcode) == 0x0033) {
				sprintf(decoded, "set_BCD(V%01X)", VX(opcode));
			}
			else if (NN(opcode) == 0x0055) {
				sprintf(decoded, "dump_registers(V%01X, $I)", VX(opcode));
			}
			else if (NN(opcode) == 0x0065) {
				sprintf(decoded, "load_registers(V%01X, $I)", VX(opcode));
			}
		}		
		else {
			sprintf(decoded, "");
		}

		if (opcode != 0x0000) {
			printf("[$%04X]: %04X --> %s\n", address, opcode, decoded);
		}
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
	
	//if (dump_memory(&chip8->memory)) printf("Error dumping memory.\n");
	parse_opcodes(chip8);
	dump_registers(chip8);
	free_memory(chip8);
	return 0;
}