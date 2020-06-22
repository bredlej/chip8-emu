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

	return chip8;
}

OPCODES* init_opcodes() {
	OPCODES* opcodes;
	opcodes = malloc(sizeof * opcodes);
	if (!opcodes) return NULL;
	opcodes->_6XNN = _6XNN;
	opcodes->_7XNN = _7XNN;
	opcodes->_8XY0 = _8XY0;
	opcodes->_8XY1 = _8XY1;
	opcodes->_8XY2 = _8XY2;
	opcodes->_8XY3 = _8XY3;
	opcodes->_8XY4 = _8XY4;
	opcodes->_8XY5 = _8XY5;
	opcodes->_8XY7 = _8XY7;

	return opcodes;
}

/** Store value at register Vx */
void _6XNN(CHIP8 *chip8_p, uint16_t opcode) {		
	chip8_p->registers[VX(opcode)] = NN(opcode);
}

/** Add value to Vx */
void _7XNN(CHIP8* chip8_p, uint16_t opcode) {	
	chip8_p->registers[VX(opcode)] += NN(opcode);
}

/** Store value of Vy at register Vx */
void _8XY0(CHIP8* chip8_p, uint16_t opcode) {	
	chip8_p->registers[VX(opcode)] = chip8_p->registers[VY(opcode)];
}

/** Vx = Vx OR Vy */
void _8XY1(CHIP8* chip8_p, uint16_t opcode) {
	chip8_p->registers[VX(opcode)] |= chip8_p->registers[VY(opcode)];
}

/** Vx = Vx AND Vy */
void _8XY2(CHIP8* chip8_p, uint16_t opcode) {
	chip8_p->registers[VX(opcode)] &= chip8_p->registers[VY(opcode)];
}

/** Vx = Vx XOR Vy */
void _8XY3(CHIP8* chip8_p, uint16_t opcode) {
	chip8_p->registers[VX(opcode)] ^= chip8_p->registers[VY(opcode)];
}

void _8XY4(CHIP8* chip8_p, uint16_t opcode) {	
	chip8_p->registers[VX(opcode)] += chip8_p->registers[VY(opcode)];
	/* set VF carry flag if result > 0xFF */
	if ((uint16_t) (chip8_p->registers[VX(opcode)] + chip8_p->registers[VY(opcode)]) > 0xFF) 	
		chip8_p->registers[VF] = 1;	
	else
		chip8_p->registers[VF] = 0;			
}

void _8XY5(CHIP8* chip8_p, uint16_t opcode)
{
	chip8_p->registers[VX(opcode)] -= chip8_p->registers[VY(opcode)];
	/* set VF carry flag if Vx < Vy */
	if (chip8_p->registers[VX(opcode)] < chip8_p->registers[VY(opcode)])
		chip8_p->registers[VF] = 0;
	else
		chip8_p->registers[VF] = 1;
}

void _8XY7(CHIP8* chip8_p, uint16_t opcode)
{
	chip8_p->registers[VX(opcode)] = chip8_p->registers[VY(opcode)] - chip8_p->registers[VX(opcode)];
	/* set VF carry flag if Vy < Vx */
	if (chip8_p->registers[VY(opcode)] < chip8_p->registers[VX(opcode)])
		chip8_p->registers[VF] = 0;
	else
		chip8_p->registers[VF] = 1;
}

int free_memory(CHIP8 *chip8_p, OPCODES *opcodes_p) {
	free(opcodes_p);
	free(chip8_p);

	return 0;
}

int dump_memory(uint8_t *memory_p) {
	uint16_t address = 0x00;
	for (uint16_t i = 0; i < MEMORY_SIZE; i++) {

		if (i % 0x0F == 0) {
			printf("\n%04X: ", address);
			address += 0x0F;
		}
		printf("%02X ", memory_p[i]);
	}

	return 0;
}

int dump_registers(CHIP8* chip8_p) {
	printf("Registers\n");
	for (int i = 0; i < 16; i++) {
		printf("V%01X=[%02X]\n", i, chip8_p->registers[i]);
	}

	return 0;
}

int debug_registers(CHIP8* chip8_p) {

	printf("[%01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:\%02X]",
		0, chip8_p->registers[0],
		1, chip8_p->registers[1],
		2, chip8_p->registers[2],
		3, chip8_p->registers[3],
		4, chip8_p->registers[4],
		5, chip8_p->registers[5],
		6, chip8_p->registers[6],
		7, chip8_p->registers[7],
		8, chip8_p->registers[8],
		9, chip8_p->registers[9],
		10, chip8_p->registers[10],
		11, chip8_p->registers[11],
		12, chip8_p->registers[12],
		13, chip8_p->registers[13],
		14, chip8_p->registers[14],
		15, chip8_p->registers[15]
	);
	
	return 0;
}

uint16_t fetch_opcode(CHIP8* chip8_p, const uint16_t address) {
	uint8_t *memory = chip8_p->memory;
	uint16_t opcode = memory[address] << 8;
	opcode |= memory[address + 1];
	
	return opcode;
}


int parse_opcodes(CHIP8 *chip8_p, OPCODES* opcodes_p) {
	
	uint16_t opcode;
	char decoded[50];	
	for (uint16_t address = MEMORY_PROGRAM_START; address < MEMORY_SIZE; address = address + 2) {
		opcode = fetch_opcode(chip8_p, address);

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
			opcodes_p->_6XNN(chip8_p, opcode);
			sprintf(decoded, "V%01X = %02X", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x7000) {
			opcodes_p->_7XNN(chip8_p, opcode);
			sprintf(decoded, "V%01X += %02X", VX(opcode), NN(opcode));
		}
		else if (OPCODE_GROUP(opcode) == 0x8000) {
			if ((opcode & 0x000F) == 0x0000) {
				
				opcodes_p->_8XY0(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X", VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0001) {
				opcodes_p->_8XY1(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X OR V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0002) {
				opcodes_p->_8XY2(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X AND V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0003) {
				opcodes_p->_8XY3(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X XOR V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0004) {				
				opcodes_p->_8XY4(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X + V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0005) {
				opcodes_p->_8XY5(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X - V%01X", VX(opcode), VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0006) {
				sprintf(decoded, "V%01X = V%01X >> 1", VX(opcode), VY(opcode));
			}
			else if ((opcode & 0x000F) == 0x0007) {
				opcodes_p->_8XY7(chip8_p, opcode);
				sprintf(decoded, "V%01X = V%01X - V%01X", VX(opcode), VY(opcode), VX(opcode));
			}			
			else if ((opcode & 0x000F) == 0x000E) {
				sprintf(decoded, "V%01X = V%01X << 1", VX(opcode), VY(opcode));
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
			
			printf("[$%04X]: %04X --> %s ", address, opcode, decoded);
			debug_registers(chip8_p);
			printf("\n");
		}
	}
	return 0;
}

int main(int argc, char** argv) {
	CHIP8 *chip8_p = init_chip8();	
	OPCODES* opcodes_p = init_opcodes();

	FILE* rom_file_p = fopen("roms/PONG", "rb");
	if (!rom_file_p) {
		printf("Can't open file.\n");
	}
	else {
		uint8_t buffer;
		uint16_t memory_address = MEMORY_PROGRAM_START;
		while (fread(&buffer, sizeof buffer, 1, rom_file_p)) {
			chip8_p->memory[memory_address++] = buffer;
		};
		fclose(rom_file_p);
	}
	
	//if (dump_memory(&chip8->memory)) printf("Error dumping memory.\n");
	parse_opcodes(chip8_p, opcodes_p);
	dump_registers(chip8_p);
	free_memory(chip8_p, opcodes_p);
	return 0;
}