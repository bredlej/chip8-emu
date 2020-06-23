#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

CHIP8 *init_chip8() {
	CHIP8 *CHIP8_POINTER = malloc(sizeof *CHIP8_POINTER);
	if (CHIP8_POINTER == NULL)
		return NULL;	
	
	/* clear whole memory */
	for (int i = 0; i < MEMORY_SIZE; i++) {
		MEMORY(i) = 0x00; 
	}

	/* clear registers */
	for (int i = 0; i < 16; i++)
		REGISTER(i) = 0x00;

	/* fill memory 0x0000 - 0x004F with font data*/
	for (int i = 0; i < FONT_AMOUNT; i++) {
		for (int j = 0; j < FONT_SIZE; j++) {		
			MEMORY(j + (i * FONT_SIZE)) = font[i][j];
		}
	}

	PC = MEMORY_PROGRAM_START;
	ADDRESS_REGISTER = 0;
	SP = 0;		

	return CHIP8_POINTER;
}

static void (* const opcode_func[]) (CHIP8*, uint16_t) = 
	{f_6XNN, f_7XNN, f_8XY0, f_8XY1, f_8XY2, f_8XY3, f_8XY4, f_8XY5, f_8XY6, f_8XY7, f_8XYE};
enum OPCODES_ORDER 
	{ _6XNN,  _7XNN,  _8XY0,  _8XY1,  _8XY2,  _8XY3,  _8XY4,  _8XY5,  _8XY6,  _8XY7,  _8XYE};

/** 
	Vx = NN
*/
OPCODE_IMPL(6XNN) 
{
	REGISTER(VX) = NN;
}

/** 
	Vx = Vx + NN
*/
OPCODE_IMPL(7XNN) 
{	
	REGISTER(VX) += NN;
}

/** 
	Vx = Vy 
*/
OPCODE_IMPL(8XY0) 
{
	REGISTER(VX) = REGISTER(VY);
}

/** 
	Vx = Vx OR Vy 
*/
OPCODE_IMPL(8XY1) 
{
	REGISTER(VX) |= REGISTER(VY);
}

/** 
	Vx = Vx AND Vy 
*/
OPCODE_IMPL(8XY2) 
{
	REGISTER(VX) &= REGISTER(VY);
}

/** 
	Vx = Vx XOR Vy 
*/
OPCODE_IMPL(8XY3) 
{
	REGISTER(VX) ^= REGISTER(VY);
}

/** 
	Vx = Vx + Vy
	set VF carry flag if result > 0xFF 
*/
OPCODE_IMPL(8XY4) 
{
	REGISTER(VX) += REGISTER(VY);
	REGISTER(VF) = (uint16_t) (REGISTER(VX) + REGISTER(VY)) > 0xFF ? 1 : 0;	
}
/** 
	Vx = Vx - Vy
	set VF carry flag if Vx < Vy 
*/
OPCODE_IMPL(8XY5) 
{
	REGISTER(VX) -= REGISTER(VY);
	REGISTER(VF) = REGISTER(VX) < REGISTER(VY) ? 0 : 1;
}

/**
	Vx = Vy >> 1
	VF = least significant bit prior to shift (0000 0001) 
*/
OPCODE_IMPL(8XY6)
{
	REGISTER(VX) = REGISTER(VY) >> 1;	
	REGISTER(VF) = LSB(VY); 
}

/* 
	Vx = Vy - Vx
	set VF carry flag if Vy < Vx 
*/
OPCODE_IMPL(8XY7)
{
	REGISTER(VX) = REGISTER(VY) - REGISTER(VX);
	REGISTER(VF) = REGISTER(VY) < REGISTER(VX) ? 0 : 1;	
}

/**  
	VX = VY << 1
	VF = most significant bit prior to shift (1000 0000) 
*/
OPCODE_IMPL(8XYE)
{
	REGISTER(VX) = REGISTER(VY) << 1;	
	REGISTER(VF) = MSB(VY);
}

int free_memory(CHIP8 *CHIP8_POINTER) {	
	free(CHIP8_POINTER);

	return 0;
}

int dump_memory(CHIP8* CHIP8_POINTER) {
	uint16_t address = 0x00;
	for (uint16_t i = 0; i < MEMORY_SIZE; i++) {

		if (i % 0x0F == 0) {
			printf("\n%04X: ", address);
			address += 0x0F;
		}
		printf("%02X ", MEMORY(i));
	}

	return 0;
}

int dump_registers(CHIP8* CHIP8_POINTER) {
	printf("Registers\n");
	for (int i = 0; i < 16; i++) {
		printf("V%01X=[%02X]\n", i, REGISTER(i));
	}
	printf("PC: %04X", PC);

	return 0;
}

int debug_registers(CHIP8* CHIP8_POINTER) {

	printf("[%01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X]",		
		0, REGISTER(V0),
		1, REGISTER(V1),
		2, REGISTER(V2),
		3, REGISTER(V3),
		4, REGISTER(V4),
		5, REGISTER(V5),
		6, REGISTER(V6),
		7, REGISTER(V7),
		8, REGISTER(V8),
		9, REGISTER(V9),
		10, REGISTER(VA),
		11, REGISTER(VB),
		12, REGISTER(VC),
		13, REGISTER(VD),
		14, REGISTER(VE),
		15, REGISTER(VF)
	);
	
	return 0;
}

uint16_t fetch_opcode(CHIP8* CHIP8_POINTER) {	
	uint16_t opcode = MEMORY(PC++) << 8;
	opcode |= MEMORY(PC++);
	
	return opcode;
}

int run(CHIP8 *CHIP8_POINTER) {
	
	DECLARE_OPCODE_VAR;
	char buffer[50];	
	
	for (PC = MEMORY_PROGRAM_START; PC < MEMORY_SIZE;) {
		OPCODE_VAR = fetch_opcode(CHIP8_POINTER);

		if (OPCODE_CLASS_EQUALS(0)) {
			if (NN == 0x00E0) { DECODE_00E0(buffer); }
			else if (NN == 0x00EE) { DECODE_00EE(buffer); }
			else { DECODE_0NNN(buffer); }
		}
		else if (OPCODE_CLASS_EQUALS(1)) { DECODE_1NNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(2)) { DECODE_2NNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(3)) { DECODE_3XNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(4)) { DECODE_4XNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(5)) { DECODE_5XY0(buffer); }
		else if (OPCODE_CLASS_EQUALS(6)) { CALL_AND_DECODE(6XNN, buffer); }
		else if (OPCODE_CLASS_EQUALS(7)) { CALL_AND_DECODE(7XNN, buffer); }
		else if (OPCODE_CLASS_EQUALS(8)) {
			if (N == 0x0000) { CALL_AND_DECODE(8XY0, buffer); }
			else if (N == 0x0001) { CALL_AND_DECODE(8XY1, buffer); }
			else if (N == 0x0002) { CALL_AND_DECODE(8XY2, buffer); }
			else if (N == 0x0003) { CALL_AND_DECODE(8XY3, buffer); }
			else if (N == 0x0004) { CALL_AND_DECODE(8XY4, buffer); }
			else if (N == 0x0005) { CALL_AND_DECODE(8XY5, buffer); }
			else if (N == 0x0006) { DECODE_8XY6(buffer); }
			else if (N == 0x0007) { CALL_AND_DECODE(8XY7, buffer); }
			else if (N == 0x000E) { DECODE_8XYE(buffer); }
		}
		else if (OPCODE_CLASS_EQUALS(9)) { DECODE_9XY0(buffer); }
		else if (OPCODE_CLASS_EQUALS(A)) { DECODE_ANNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(B)) { DECODE_BNNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(C)) { DECODE_CXNN(buffer); }
		else if (OPCODE_CLASS_EQUALS(D)) { DECODE_DXYN(buffer); }
		else if (OPCODE_CLASS_EQUALS(E)) {
			if (NN == 0x009E) { DECODE_EX9E(buffer); }
			else if (NN == 0x00A1) { DECODE_EXA1(buffer); }
		}
		else if (OPCODE_CLASS_EQUALS(F)) {
			if (NN == 0x0007) { DECODE_FX07(buffer); }
			else if (NN == 0x000A) { DECODE_FX0A(buffer); }
			else if (NN == 0x0015) { DECODE_FX15(buffer); }
			else if (NN == 0x0018) { DECODE_FX18(buffer); }
			else if (NN == 0x001E) { DECODE_FX1E(buffer); }
			else if (NN == 0x0029) { DECODE_FX29(buffer); }
			else if (NN == 0x0033) { DECODE_FX33(buffer); }
			else if (NN == 0x0055) { DECODE_FX55(buffer); }
			else if (NN == 0x0065) { DECODE_FX65(buffer); }
		}		
		else {
			sprintf(buffer, "");
		}

		if (opcode != 0x0000) {			
			printf("[$%04X]: %04X --> %s ", PC, OPCODE_VAR, buffer);
			debug_registers(CHIP8_POINTER);
			printf("\n");
		}
	}
	return 0;
}

int main(int argc, char** argv) {
	CHIP8 *CHIP8_POINTER = init_chip8();		

	FILE* rom_file_p = fopen("roms/PONG", "rb");
	if (!rom_file_p) {
		printf("Can't open file.\n");
	}
	else {
		uint8_t buffer;
		uint16_t memory_address = MEMORY_PROGRAM_START;
		while (fread(&buffer, sizeof buffer, 1, rom_file_p)) {
			MEMORY(memory_address++) = buffer;
		};
		fclose(rom_file_p);
	}
	
	if (dump_memory(&CHIP8_POINTER->memory)) printf("Error dumping memory.\n");
	run(CHIP8_POINTER);
	dump_registers(CHIP8_POINTER);
	free_memory(CHIP8_POINTER);
	return 0;
}