#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t chip8_font[FONT_AMOUNT][FONT_SIZE] = {
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
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  /* F */
};

DEFINE_CALL(0NNN) {}
DEFINE_CALL(00E0) {}
DEFINE_CALL(00EE) {}
DEFINE_CALL(1NNN) { PC = NNN; }
DEFINE_CALL(2NNN) {}
DEFINE_CALL(3XNN) {
  if (REGISTER(VX) == NN)
    STEP
}
DEFINE_CALL(4XNN) {
  if (REGISTER(VX) != NN)
    STEP
}
DEFINE_CALL(5XY0) {
  if (REGISTER(VX) == REGISTER(VY))
    STEP
}

/**
        Vx = NN
*/
DEFINE_CALL(6XNN) { REGISTER(VX) = NN; }

/**
        Vx = Vx + NN
*/
DEFINE_CALL(7XNN) { REGISTER(VX) += NN; }

/**
        Vx = Vy
*/
DEFINE_CALL(8XY0) { REGISTER(VX) = REGISTER(VY); }

/**
        Vx = Vx OR Vy
*/
DEFINE_CALL(8XY1) { REGISTER(VX) |= REGISTER(VY); }

/**
        Vx = Vx AND Vy
*/
DEFINE_CALL(8XY2) { REGISTER(VX) &= REGISTER(VY); }

/**
        Vx = Vx XOR Vy
*/
DEFINE_CALL(8XY3) { REGISTER(VX) ^= REGISTER(VY); }

/**
        Vx = Vx + Vy
        set VF carry flag if result > 0xFF
*/
DEFINE_CALL(8XY4) {
  REGISTER(VX) += REGISTER(VY);
  REGISTER(VF) = (uint16_t)(REGISTER(VX) + REGISTER(VY)) > 0xFF ? 1 : 0;
}
/**
        Vx = Vx - Vy
        set VF carry flag if Vx < Vy
*/
DEFINE_CALL(8XY5) {
  REGISTER(VX) -= REGISTER(VY);
  REGISTER(VF) = REGISTER(VX) < REGISTER(VY) ? 0 : 1;
}

/**
        Vx = Vy >> 1
        VF = least significant bit prior to shift (0000 0001)
*/
DEFINE_CALL(8XY6) {
  REGISTER(VX) = (uint8_t) REGISTER(VY) >> 1;
  REGISTER(VF) = LSB(VY);
}

/*
        Vx = Vy - Vx
        set VF carry flag if Vy < Vx
*/
DEFINE_CALL(8XY7) {
  REGISTER(VX) = REGISTER(VY) - REGISTER(VX);
  REGISTER(VF) = REGISTER(VY) < REGISTER(VX) ? 0 : 1;
}

/**
        VX = VY << 1
        VF = most significant bit prior to shift (1000 0000)
*/
DEFINE_CALL(8XYE) {
  REGISTER(VX) = REGISTER(VY) << 1;
  REGISTER(VF) = MSB(VY);
}

DEFINE_CALL(9XY0) {
  if (REGISTER(VX) != REGISTER(VY))
    STEP;
}
DEFINE_CALL(ANNN) {}
DEFINE_CALL(BNNN) { PC = NNN + REGISTER(V0); }

/**
        VX = rnd() AND NN
*/
DEFINE_CALL(CXNN) {
  REGISTER(VX) = (uint8_t)(rand() / (RAND_MAX + 1.0) * 0xFF) & NN;
}

DEFINE_CALL(DXYN) {}
DEFINE_CALL(EX9E) {}
DEFINE_CALL(EXA1) {}
DEFINE_CALL(FX07) { REGISTER(VX) = DELAY_TIMER; }
DEFINE_CALL(FX0A) {}
DEFINE_CALL(FX15) { DELAY_TIMER = REGISTER(VX); }
DEFINE_CALL(FX18) {}
DEFINE_CALL(FX1E) {}
DEFINE_CALL(FX29) {}
DEFINE_CALL(FX33) {}
DEFINE_CALL(FX55) {}
DEFINE_CALL(FX65) {}

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
      MEMORY(j + (i * FONT_SIZE)) = chip8_font[i][j];
    }
  }

  PC = MEMORY_PROGRAM_START;
  ADDRESS_REGISTER = 0;
  SP = 0;

  time_t tt;
  srand((uint32_t)time(&tt));

  return CHIP8_POINTER;
}

int free_memory(CHIP8 *CHIP8_POINTER) {
  free(CHIP8_POINTER);

  return 0;
}

int dump_memory(CHIP8 *CHIP8_POINTER) {
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

int dump_registers(CHIP8 *CHIP8_POINTER) {
  printf("Registers\n");
  for (int i = 0; i < 16; i++) {
    printf("V%01X=[%02X]\n", i, REGISTER(i));
  }
  printf("PC: %04X", PC);

  return 0;
}

int debug_registers(CHIP8 *CHIP8_POINTER) {
  printf("[%01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X "
         "%01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X %01X:%02X "
         "%01X:%02X %01X:%02X %01X:%02X %01X:%02X]",
         0, REGISTER(V0), 1, REGISTER(V1), 2, REGISTER(V2), 3, REGISTER(V3), 4,
         REGISTER(V4), 5, REGISTER(V5), 6, REGISTER(V6), 7, REGISTER(V7), 8,
         REGISTER(V8), 9, REGISTER(V9), 10, REGISTER(VA), 11, REGISTER(VB), 12,
         REGISTER(VC), 13, REGISTER(VD), 14, REGISTER(VE), 15, REGISTER(VF));

  return 0;
}

uint16_t fetch_opcode(CHIP8 *CHIP8_POINTER) {
  uint16_t opcode = (uint16_t) MEMORY(PC) << 8;
  opcode |= MEMORY(PC + 1);

  return opcode;
}

void step(CHIP8 *CHIP8_POINTER) { PC = PC + 2; }

int run(CHIP8 *CHIP8_POINTER) {

  DECLARE_OPCODE_VAR
  char buffer[50];
  DELAY_TIMER = 0x3C;

  uint16_t jump = 0;

  for (PC = MEMORY_PROGRAM_START; PC < MEMORY_SIZE;) {
    OPCODE_VAR = fetch_opcode(CHIP8_POINTER);

    if (IS_OPCODE_GROUP(0)) {
      if (NN == 0x00E0) {
        DECODE_00E0(buffer);
      } else if (NN == 0x00EE) {
        DECODE_00EE(buffer);
      } else {
        DECODE_0NNN(buffer)
      }
    } else if (IS_OPCODE_GROUP(1)) {
      CALL_AND_DECODE(1NNN, buffer)
      jump = NNN;
    } else if (IS_OPCODE_GROUP(2)) {
      DECODE_2NNN(buffer);
    } else if (IS_OPCODE_GROUP(3)) {
      CALL_AND_DECODE(3XNN, buffer)
    } else if (IS_OPCODE_GROUP(4)) {
      CALL_AND_DECODE(4XNN, buffer)
    } else if (IS_OPCODE_GROUP(5)) {
      CALL_AND_DECODE(5XY0, buffer)
    } else if (IS_OPCODE_GROUP(6)) {
      CALL_AND_DECODE(6XNN, buffer)
    } else if (IS_OPCODE_GROUP(7)) {
      CALL_AND_DECODE(7XNN, buffer)
    } else if (IS_OPCODE_GROUP(8)) {
      if (N == 0x0000) {
        CALL_AND_DECODE(8XY0, buffer)
      } else if (N == 0x0001) {
        CALL_AND_DECODE(8XY1, buffer)
      } else if (N == 0x0002) {
        CALL_AND_DECODE(8XY2, buffer)
      } else if (N == 0x0003) {
        CALL_AND_DECODE(8XY3, buffer)
      } else if (N == 0x0004) {
        CALL_AND_DECODE(8XY4, buffer)
      } else if (N == 0x0005) {
        CALL_AND_DECODE(8XY5, buffer)
      } else if (N == 0x0006) {
        DECODE_8XY6(buffer);
      } else if (N == 0x0007) {
        CALL_AND_DECODE(8XY7, buffer)
      } else if (N == 0x000E) {
        DECODE_8XYE(buffer);
      }
    } else if (IS_OPCODE_GROUP(9)) {
      CALL_AND_DECODE(9XY0, buffer)
      jump = 1;
    } else if (IS_OPCODE_GROUP(A)) {
      DECODE_ANNN(buffer);
    } else if (IS_OPCODE_GROUP(B)) {
      CALL_AND_DECODE(BNNN, buffer)
    } else if (IS_OPCODE_GROUP(C)) {
      CALL_AND_DECODE(CXNN, buffer)
    } else if (IS_OPCODE_GROUP(D)) {
      DECODE_DXYN(buffer);
    } else if (IS_OPCODE_GROUP(E)) {
      if (NN == 0x009E) {
        DECODE_EX9E(buffer);
      } else if (NN == 0x00A1) {
        DECODE_EXA1(buffer);
      }
    } else if (IS_OPCODE_GROUP(F)) {
      if (NN == 0x0007) {
        CALL_AND_DECODE(FX07, buffer)
      } else if (NN == 0x000A) {
        DECODE_FX0A(buffer);
      } else if (NN == 0x0015) {
        CALL_AND_DECODE(FX15, buffer)
      } else if (NN == 0x0018) {
        DECODE_FX18(buffer);
      } else if (NN == 0x001E) {
        DECODE_FX1E(buffer);
      } else if (NN == 0x0029) {
        DECODE_FX29(buffer);
      } else if (NN == 0x0033) {
        DECODE_FX33(buffer);
      } else if (NN == 0x0055) {
        DECODE_FX55(buffer);
      } else if (NN == 0x0065) {
        DECODE_FX65(buffer);
      }
    } else {
      sprintf(buffer, "");
    }

    if (OPCODE_VAR != 0x0000) {
      printf("[$%04X]: %04X --> %s ", PC, OPCODE_VAR, buffer);
      debug_registers(CHIP8_POINTER);
      printf("\n");
    }

    if (jump) {
      PC = jump;
    } else
      STEP

    if (DELAY_TIMER > 0) {
      DELAY_TIMER--;
    } else {
      DELAY_TIMER = 0;
    }
  }
  return 0;
}
