#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

#define CHIP8_INSTR_SET                                                        \
  OP(0NNN)                                                                     \
  OP(00E0)                                                                     \
  OP(00EE) OP(1NNN) OP(2NNN) OP(3XNN) OP(4XNN) OP(5XY0) OP(6XNN) OP(7XNN)      \
      OP(8XY0) OP(8XY1) OP(8XY2) OP(8XY3) OP(8XY4) OP(8XY5) OP(8XY6) OP(8XY7)  \
          OP(8XYE) OP(9XY0) OP(ANNN) OP(BNNN) OP(CXNN) OP(DXYN) OP(EX9E)       \
              OP(EXA1) OP(FX07) OP(FX0A) OP(FX15) OP(FX18) OP(FX1E) OP(FX29)   \
                  OP(FX33) OP(FX55) OP(FX65)

#define MEMORY_SIZE 0x1000           // 4096 bytes memory total
#define MEMORY_INTERPRETER 0x000     // 000 - 200 : 512 bytes
#define MEMORY_PROGRAM_START 0x200   // 200 - EA0 : 3232 bytes
#define MEMORY_CALL_STACK 0xEA0      // EA0 - EFF - 96 bytes
#define MEMORY_DISPLAY_REFRESH 0xF00 // F00 - FFF - 256 bytes

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
#define GFX_RESOLUTION GFX_WIDTH *GFX_HEIGHT

#define FONT_AMOUNT 16
#define FONT_SIZE 5

#define CHIP8_POINTER chip8_p
#define OPCODE_VAR opcode_uint16
#define DECLARE_OPCODE_VAR uint16_t OPCODE_VAR;
#define IS_OPCODE_GROUP(n) OPCODE_GROUP(OPCODE_VAR) == OPCODE_##n##xxx
#define DECLARE_OPCODE_FUNC(opclass) void f_##opclass(CHIP8 *, uint16_t);
#define DEFINE_CALL(opclass)                                                   \
  void f_##opclass(CHIP8 *CHIP8_POINTER, uint16_t OPCODE_VAR)
#define CALL(opcode) Opcode_Callbacks[_##opcode](chip8_p, OPCODE_VAR)
#define OPCODE_GROUP(opcode) (opcode & 0xF000)
#define VX (OPCODE_VAR & 0x0F00) >> 8
#define VY (OPCODE_VAR & 0x00F0) >> 4
#define DELAY_TIMER CHIP8_POINTER->delay_timer
#define REGISTER(v) CHIP8_POINTER->registers[v]
#define MEMORY(address) CHIP8_POINTER->memory[address]
#define PC CHIP8_POINTER->pc
#define SP CHIP8_POINTER->sp
#define ADDRESS_REGISTER CHIP8_POINTER->I
#define NNN (OPCODE_VAR & 0x0FFF)
#define NN (OPCODE_VAR & 0x00FF)
#define N (OPCODE_VAR & 0x000F)
#define MSB(v) REGISTER(v) && 0xC0
#define LSB(v) REGISTER(v) && 0x01
#define STEP step(CHIP8_POINTER);

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
#define DECODE_8XY1(buffer)                                                    \
  sprintf(buffer, "V%01X = V%01X OR V%01X", VX, VX, VY)
#define DECODE_8XY2(buffer)                                                    \
  sprintf(buffer, "V%01X = V%01X AND V%01X", VX, VX, VY)
#define DECODE_8XY3(buffer)                                                    \
  sprintf(buffer, "V%01X = V%01X XOR V%01X", VX, VX, VY)
#define DECODE_8XY4(buffer) sprintf(buffer, "V%01X = V%01X + V%01X", VX, VX, VY)
#define DECODE_8XY5(buffer) sprintf(buffer, "V%01X = V%01X - V%01X", VX, VX, VY)
#define DECODE_8XY6(buffer) sprintf(buffer, "V%01X = V%01X >> 1", VX, VY)
#define DECODE_8XY7(buffer) sprintf(buffer, "V%01X = V%01X - V%01X", VX, VX, VY)
#define DECODE_8XYE(buffer) sprintf(buffer, "V%01X = V%01X << 1", VX, VY)
#define DECODE_9XY0(buffer) sprintf(buffer, "if (V%01X != V%01X)", VX, NN)
#define DECODE_ANNN(buffer) sprintf(buffer, "I = $%04X", NNN)
#define DECODE_BNNN(buffer) sprintf(buffer, "PC = V0 + $%04X", NNN)
#define DECODE_CXNN(buffer) sprintf(buffer, "V%01X = rnd() AND %02X", VX, NN)
#define DECODE_DXYN(buffer)                                                    \
  sprintf(buffer, "draw(V%01X, V%01X, %01X)", VX, VY, N)
#define DECODE_EX9E(buffer) sprintf(buffer, "if (key() == V%01X)", VX)
#define DECODE_EXA1(buffer) sprintf(buffer, "if (key() != V%01X)", VX)
#define DECODE_FX07(buffer) sprintf(buffer, "V%01X = get_delay_timer()", VX)
#define DECODE_FX0A(buffer) sprintf(buffer, "V%01X = get_key()", VX)
#define DECODE_FX15(buffer) sprintf(buffer, "set_delay_timer(V%01X)", VX)
#define DECODE_FX18(buffer) sprintf(buffer, "set_sound_timer(V%01X)", VX)
#define DECODE_FX1E(buffer) sprintf(buffer, "I = I + V%01X", VX)
#define DECODE_FX29(buffer) sprintf(buffer, "I = sprite_addr[V%01X]", VX)
#define DECODE_FX33(buffer) sprintf(buffer, "set_BCD(V%01X)", VX)
#define DECODE_FX55(buffer) sprintf(buffer, "dump_registers(V%01X, $I)", VX)
#define DECODE_FX65(buffer) sprintf(buffer, "load_registers(V%01X, $I)", VX)

#define CALL_AND_DECODE(opcode, buffer)                                        \
  CALL(opcode);                                                                \
  DECODE_##opcode(buffer);

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

extern uint8_t chip8_font[FONT_AMOUNT][FONT_SIZE];

typedef struct {
  uint8_t memory[MEMORY_SIZE];
  uint8_t registers[REGISTERS_AMOUNT];
  uint8_t gfx[GFX_RESOLUTION];
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t input;
  uint16_t I;  // address register
  uint16_t pc; // program counter
  uint16_t stack[STACK_SIZE];
  uint16_t sp; // stack pointer
} CHIP8;

/* Declare opcode callback function type -> void F(chip8_handler, opcode) */
typedef void (*const opcode_callback_f)(CHIP8 *, const uint16_t);

/* Declare a C function for each opcode */
#define OP(x) DECLARE_OPCODE_FUNC(x);
CHIP8_INSTR_SET;
#undef OP

/* Declare array of callback functions assigned to CHIP-8 opcodes */
#define OP(x) f_##x,
static opcode_callback_f Opcode_Callbacks[] = {CHIP8_INSTR_SET};
#undef OP

/* Declare enumeration of CHIP-8 opcodes */
#define OP(x) _##x,
enum opcodes_order_e { CHIP8_INSTR_SET };
#undef OP

CHIP8 *init_chip8();
int free_memory(CHIP8 *);
int dump_memory(CHIP8 *);
int dump_registers(CHIP8 *);
int debug_registers(CHIP8 *);
void step(CHIP8 *);
int run(CHIP8 *);

#endif
