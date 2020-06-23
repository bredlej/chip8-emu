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

#define OPCODE_VAR opcode
#define CHIP8_POINTER chip8_p
#define DECLARE_OPCODE_VAR uint16_t OPCODE_VAR;
#define OPCODE_CLASS_EQUALS(n) OPCODE_GROUP(opcode) == OPCODE_ ## n ##xxx
#define OPCODE_FUNC(opclass) void f_ ## opclass ## (CHIP8*, uint16_t)
#define OPCODE_IMPL(opclass) void f_ ## opclass ## (CHIP8* chip8_p, uint16_t opcode)
#define CALL_OPCODE(opcode) opcode_func[_ ## opcode ## ](chip8_p, OPCODE_VAR)
#define OPCODE_GROUP(opcode) (opcode & 0xF000)
#define VX (OPCODE_VAR & 0x0FFF) >> 8
#define VY (OPCODE_VAR & 0x00FF) >> 4
#define REGISTER(v) CHIP8_POINTER->registers[v] 
#define MEMORY(address) CHIP8_POINTER->memory[address]
#define PC CHIP8_POINTER->pc
#define SP CHIP8_POINTER->sp
#define ADDRESS_REGISTER CHIP8_POINTER->I
#define NNN (OPCODE_VAR & 0x0FFF)
#define NN  (OPCODE_VAR & 0x00FF)
#define N   (OPCODE_VAR & 0x000F)
#define MSB(v) REGISTER(v) && 0xC0
#define LSB(v) REGISTER(v) && 0x01
#define DECODE_0NNN(buffer) sprintf(buffer, "call NNN");
#define DECODE_00E0(buffer) sprintf(buffer, "clear_display()")
#define DECODE_00EE(buffer) sprintf(buffer, "return")
#define DECODE_1NNN(buffer) sprintf(buffer, "goto $%04X", NNN)
#define DECODE_2NNN(buffer) sprintf(buffer, "*($%04X)()", NNN)
#define DECODE_3XNN(buffer) sprintf(buffer, "if (V%01X == %02X)", VX, NN)
#define DECODE_4XNN(buffer) sprintf(buffer, "if (V%01X != %02X)", VX, NN)
#define DECODE_5XY0(buffer) sprintf(buffer, "if (V%01X == V%01X)", VX, VY)
#define DECODE_6XNN(buffer) sprintf(buffer, "V%01X = %02X", VX, NN)
#define DECODE_7XNN(buffer) sprintf(buffer, "V%01X += %02X", VX, NN)
#define DECODE_8XY0(buffer) sprintf(buffer, "V%01X = V%01X", VX, VY)
#define DECODE_8XY1(buffer) sprintf(buffer, "V%01X = V%01X OR V%01X", VX, VX, VY)
#define DECODE_8XY2(buffer) sprintf(buffer, "V%01X = V%01X AND V%01X", VX, VX, VY)
#define DECODE_8XY3(buffer) sprintf(buffer, "V%01X = V%01X XOR V%01X", VX, VX, VY)
#define DECODE_8XY4(buffer) sprintf(buffer, "V%01X = V%01X + V%01X", VX, VX, VY)
#define DECODE_8XY5(buffer) sprintf(buffer, "V%01X = V%01X - V%01X", VX, VX, VY)
#define DECODE_8XY6(buffer) sprintf(buffer, "V%01X = V%01X >> 1", VX, VY)
#define DECODE_8XY7(buffer) sprintf(buffer, "V%01X = V%01X - V%01X", VX, VX, VY)
#define DECODE_8XYE(buffer) sprintf(buffer, "V%01X = V%01X << 1", VX, VY)
#define DECODE_9XY0(buffer) sprintf(buffer, "if (V%01X != V%01X)", VX, NN)
#define DECODE_ANNN(buffer) sprintf(buffer, "I = $%04X", NNN)
#define DECODE_BNNN(buffer) sprintf(buffer, "PC = V0 + $%04X", NNN);
#define DECODE_CXNN(buffer) sprintf(buffer, "V%01X = rnd() AND %02X", VX, NN);
#define DECODE_DXYN(buffer) sprintf(buffer, "draw(V%01X, V%01X, %01X)", VX, VY, N);
#define DECODE_EX9E(buffer) sprintf(buffer, "if (key() == V%01X)", VX);
#define DECODE_EXA1(buffer) sprintf(buffer, "if (key() != V%01X)", VX);
#define DECODE_FX07(buffer) sprintf(buffer, "V%01X = get_delay_timer()", VX);
#define DECODE_FX0A(buffer) sprintf(buffer, "V%01X = get_key()", VX);
#define DECODE_FX15(buffer) sprintf(buffer, "set_delay_timer(V%01X)", VX);
#define DECODE_FX18(buffer) sprintf(buffer, "set_sound_timer(V%01X)", VX); 
#define DECODE_FX1E(buffer) sprintf(buffer, "I = I + V%01X", VX);
#define DECODE_FX29(buffer) sprintf(buffer, "I = sprite_addr[V%01X]", VX);
#define DECODE_FX33(buffer) sprintf(buffer, "set_BCD(V%01X)", VX);
#define DECODE_FX55(buffer) sprintf(buffer, "dump_registers(V%01X, $I)", VX);
#define DECODE_FX65(buffer) sprintf(buffer, "load_registers(V%01X, $I)", VX);

#define CALL_AND_DECODE(opcode, buffer)\
CALL_OPCODE(opcode);\
DECODE_ ## opcode ## (buffer);

#define OPCODE_0xxx 0x0000
#define OPCODE_1xxx 0x1000
#define OPCODE_2xxx 0x2000
#define OPCODE_3xxx 0x3000
#define OPCODE_4xxx 0x4000
#define OPCODE_5xxx 0x5000
#define OPCODE_6xxx 0x6000
#define OPCODE_7xxx 0x7000
#define OPCODE_8xxx 0x8000
#define OPCODE_9xxx 0x9000
#define OPCODE_Axxx 0xA000
#define OPCODE_Bxxx 0xB000
#define OPCODE_Cxxx 0xC000
#define OPCODE_Dxxx 0xD000
#define OPCODE_Exxx 0xE000
#define OPCODE_Fxxx 0xF000

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
} CHIP8;

OPCODE_FUNC(0NNN);
OPCODE_FUNC(00E0);
OPCODE_FUNC(00EE);
OPCODE_FUNC(1NNN);
OPCODE_FUNC(2NNN);
OPCODE_FUNC(3XNN);
OPCODE_FUNC(4XNN);
OPCODE_FUNC(5XY0);
OPCODE_FUNC(6XNN); /* (&Vx, NN) - store NN at register Vx */
OPCODE_FUNC(7XNN); /* (&Vx, NN) - add NN to Vx */
OPCODE_FUNC(8XY0); /* (&Vx, &Vy) - store value of Vy at Vx */
OPCODE_FUNC(8XY1); /* (&Vx, &Vy) - Vx = Vx OR Vy */
OPCODE_FUNC(8XY2); /* (&Vx, &Vy) - Vx = Vx AND Vy */
OPCODE_FUNC(8XY3); /* (&Vx, &Vy) - Vx = Vx XOR Vy */
OPCODE_FUNC(8XY4); /* (&Vx, &Vy) - add Vx to Vy, set VF=1 if carry flag, or VF=0 if not */
OPCODE_FUNC(8XY5); /* (&Vx, &Vy) - Vx = Vx - Vy, set VF=1 if carry flag, or VF=0 if not */
OPCODE_FUNC(8XY6);
OPCODE_FUNC(8XY7); /* (&Vx, &Vy) - Vx = Vy - Vx, set VF=1 if carry flag, or VF=0 if not  */
OPCODE_FUNC(8XYE);
OPCODE_FUNC(9XY0);
OPCODE_FUNC(ANNN);
OPCODE_FUNC(BNNN);
OPCODE_FUNC(CXNN);
OPCODE_FUNC(DXYN);
OPCODE_FUNC(EX9E);
OPCODE_FUNC(EXA1);
OPCODE_FUNC(FX07);
OPCODE_FUNC(FX0A);
OPCODE_FUNC(FX15);
OPCODE_FUNC(FX18);
OPCODE_FUNC(FX1E);
OPCODE_FUNC(FX29);
OPCODE_FUNC(FX33);
OPCODE_FUNC(FX55);
OPCODE_FUNC(FX65);

#endif