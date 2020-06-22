#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define MEMORY_SIZE 0x1000             // 4096 bytes memory total
#define MEMORY_INTERPRETER 0x000       // 000 - 200 : 512 bytes
#define MEMORY_PROGRAM_START 0x200     // 200 - EA0 : 3232 bytes
#define MEMORY_CALL_STACK 0xEA0        // EA0 - EFF - 96 bytes 
#define MEMORY_DISPLAY_REFRESH  0xF00  // F00 - FFF - 256 bytes

#define STACK_SIZE 16
#define REGISTERS_AMOUNT 16
#define V0 0
#define V1 1
#define V2 2
#define V3 3
#define V4 4
#define V5 5
#define V6 6
#define V7 7
#define V8 8
#define V9 9
#define VA 10
#define VB 11
#define VC 12
#define VD 13
#define VE 14
#define VF 15

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_RESOLUTION GFX_WIDTH * GFX_HEIGHT

#define FONT_AMOUNT 16
#define FONT_SIZE 5

#define OPCODE_GROUP(opcode) (opcode & 0xF000)
#define VX(opcode) (opcode & 0x0FFF) >> 8
#define VY(opcode) (opcode & 0x00FF) >> 4
#define NNN(opcode) (opcode & 0x0FFF)
#define NN(opcode) (opcode & 0x00FF)
#define N(opcode) (opcode & 0x000F)


uint8_t font[FONT_AMOUNT][FONT_SIZE] = {
	{0xF0, 0x90, 0x90, 0x90, 0xF0}, /* 0 */
	{0x20, 0x60, 0x20, 0x20, 0x70}, /* 1 */
	{0xF0, 0x10, 0xF0, 0x80, 0xF0}, /* 2 */
	{0xF0, 0x10, 0xF0, 0x10, 0xF0}, /* 3 */
	{0x90, 0x90, 0xF0, 0x10, 0x10}, /* 4 */
	{0xF0, 0x80, 0xF0, 0x10, 0xF0}, /* 5 */
	{0xF0, 0x80, 0xF0, 0x90, 0xF0}, /* 6 */
	{0xF0, 0x10, 0x20, 0x40, 0x40}, /* 7 */
	{0xF0, 0x90, 0xF0, 0x90, 0xF0}, /* 8 */
	{0xF0, 0x90, 0xF0, 0x10, 0xF0}, /* 9 */
	{0xF0, 0x90, 0xF0, 0x90, 0x90}, /* A */
	{0xE0, 0x90, 0xE0, 0x90, 0xE0}, /* B */
	{0xF0, 0x80, 0x80, 0x80, 0xF0}, /* C */
	{0xE0, 0x90, 0x90, 0x90, 0xE0}, /* D */
	{0xF0, 0x80, 0xF0, 0x80, 0xF0}, /* E */
	{0xF0, 0x80, 0xF0, 0x80, 0x80}	/* F */
};

typedef struct {
	void (*_6XNN) (uint32_t *, const uint8_t); /* (&VX, value) - store value at register VX*/
	void (*_8XY0) (uint32_t *, uint32_t *); /* (&VX, &VY) - store value of VY at VX*/
} OPCODES;

typedef struct {
	uint8_t memory[MEMORY_SIZE];
	uint8_t registers[REGISTERS_AMOUNT];
	uint8_t gfx[GFX_RESOLUTION];
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t input;
	uint16_t I; // address register
	uint16_t pc; // program counter
	uint16_t stack[STACK_SIZE];
	uint16_t sp; // stack pointer
	OPCODES *opcodes;
} CHIP8;

void _6XNN(uint32_t *, const uint8_t);
void _8XY0(uint32_t *, uint32_t *);

#endif