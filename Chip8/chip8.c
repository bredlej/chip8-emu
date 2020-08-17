#include "chip8.h"
#include "pcg_basic.h"
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

pcg32_random_t rng;
uint32_t rnd(void) { return pcg32_random_r(&rng); }

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
  REGISTER(VF) = ((uint16_t)REGISTER(VX) + (uint16_t)REGISTER(VY)) > 0xFF
                     ? CARRY
                     : NO_CARRY;
  REGISTER(VX) = REGISTER(VX) + REGISTER(VY);
}
/**
        Vx = Vx - Vy
        set VF carry flag if Vx < Vy
*/
DEFINE_CALL(8XY5) {
  REGISTER(VF) = ((uint16_t)REGISTER(VX) - (uint16_t)REGISTER(VY)) < 0x00
                     ? CARRY
                     : NO_CARRY;
  REGISTER(VX) -= REGISTER(VY);
}

/**
        Vx = Vy >> 1
        VF = least significant bit prior to shift (0000 0001)
*/
DEFINE_CALL(8XY6) {
  REGISTER(VF) = LSB(VY);
  REGISTER(VX) = (uint8_t)REGISTER(VY) >> 1;
}

/*
        Vx = Vy - Vx
        set VF carry flag if Vy < Vx
*/
DEFINE_CALL(8XY7) {
  REGISTER(VF) = ((uint16_t)REGISTER(VY) - (uint16_t)REGISTER(VX)) < 0x00
                     ? CARRY
                     : NO_CARRY;
  REGISTER(VX) = REGISTER(VY) - REGISTER(VX);
}

/**
        VX = VY << 1
        VF = most significant bit prior to shift (1000 0000)
*/
DEFINE_CALL(8XYE) {
  REGISTER(VF) = MSB(VY);
  REGISTER(VX) = REGISTER(VY) << 1;
}

DEFINE_CALL(9XY0) {
  if (REGISTER(VX) != REGISTER(VY))
    STEP;
}
DEFINE_CALL(ANNN) { ADDRESS_REGISTER = NNN; }
DEFINE_CALL(BNNN) { PC = NNN + REGISTER(V0); }

/**
        VX = rnd() AND NN
*/
DEFINE_CALL(CXNN) { REGISTER(VX) = (uint8_t)(rnd() % 0xFF) & NN; }

/**
    Draws N following bytes of address referenced by I to x,y coordinates
   declared in VX, VY registers. Pixels wrap around display if their coordinates
   are greater than width & height.
*/
DEFINE_CALL(DXYN) {
  uint16_t x_coord = REGISTER(VX) % GFX_WIDTH;
  uint16_t y_coord = REGISTER(VY) % GFX_HEIGHT;
  uint8_t has_collision_occured = 0;

  for (uint8_t address_offset = 0; address_offset < N; address_offset++) {
    /*
        Determine y after offsetting (Wrap around to 0 if y > display height)
    */
    y_coord =
        (REGISTER(VY) + address_offset > GFX_HEIGHT ? address_offset - 1
                                                   : (REGISTER(VY) % GFX_HEIGHT) + address_offset );
    /*
        Determine location of x,y in the framebuffer table
    */
    uint8_t first_byte = y_coord * BITMAP_BYTE_WIDTH + (uint8_t)(x_coord / 8);
    uint8_t pixels_to_insert = MEMORY(ADDRESS_REGISTER + address_offset);

    if (x_coord % 8 != 0) {
      /*
          If pixels don't start at leftmost bit they will wrap to the next
          byte:
          Example:
          0        8
          00000000 00000000
          -> insert FF at position 4, 0 will result in:
          00001111 11110000
      */
      uint8_t first_byte_pixels = pixels_to_insert >> (x_coord % 8);
      uint8_t second_byte_pixels = pixels_to_insert << (8 - (x_coord % 8));
      /*
        Determine position of x+1, y in the framebuffer table
        (wrap around if x > display width)
      */
      uint8_t second_byte = y_coord * BITMAP_BYTE_WIDTH +
                            ((uint8_t)(x_coord / 8) == BITMAP_BYTE_WIDTH - 1
                                 ? 0
                                 : (uint8_t)(x_coord / 8) + 1);
      /*
        Check if there are pixels that will be changed by the XOR of new pixels
      */
      has_collision_occured =
          IS_COLLISION(FRAMEBUFFER(first_byte), first_byte_pixels) ||
          IS_COLLISION(FRAMEBUFFER(second_byte), second_byte_pixels);
      /*
        XOR pixels in left and right framebuffer bytes
      */
      FRAMEBUFFER(first_byte) ^= first_byte_pixels;
      FRAMEBUFFER(second_byte) ^= second_byte_pixels;
    } else {
      /*
          If pixels start at leftmost bit they will occupy one byte:
          Example:
          0        8
          00000000 00000000
          -> insert FF at position 0, 0 will result in:
          11111111 00000000
      */
      /*
        Check if any bits will be changed by XOR of new pixels
      */
      has_collision_occured =
          IS_COLLISION(FRAMEBUFFER(first_byte), pixels_to_insert);
      /*
        XOR pixels into framebuffer
      */
      FRAMEBUFFER(first_byte) ^= pixels_to_insert;
    }
  }
  REGISTER(VF) = has_collision_occured ? CARRY : NO_CARRY;
}

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

  for (int i = 0; i < BITMAP_SIZE; i++)
    FRAMEBUFFER(i) = 0x00;

  PC = MEMORY_PROGRAM_START;
  ADDRESS_REGISTER = 0;
  SP = 0;
  DELAY_TIMER = 0;

  sprintf(INSTRUCTION_BUFFER, "");

  pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, (intptr_t)&dump_memory);

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
         "%01X:%02X %01X:%02X %01X:%02X %01X:%02X DT:%02X I:%04X]\n",
         0, REGISTER(V0), 1, REGISTER(V1), 2, REGISTER(V2), 3, REGISTER(V3), 4,
         REGISTER(V4), 5, REGISTER(V5), 6, REGISTER(V6), 7, REGISTER(V7), 8,
         REGISTER(V8), 9, REGISTER(V9), 10, REGISTER(VA), 11, REGISTER(VB), 12,
         REGISTER(VC), 13, REGISTER(VD), 14, REGISTER(VE), 15, REGISTER(VF),
         DELAY_TIMER, ADDRESS_REGISTER);

  return 0;
}

uint16_t fetch_opcode(CHIP8 *CHIP8_POINTER) {
  uint16_t opcode = (uint16_t)MEMORY(PC) << 8;
  opcode |= MEMORY(PC + 1);

  return opcode;
}

uint16_t fetch_opcode_at(CHIP8 *CHIP8_POINTER, uint16_t address) {
  uint16_t opcode = (uint16_t)MEMORY(address) << 8;
  opcode |= MEMORY(address + 1);

  return opcode;
}

void step(CHIP8 *CHIP8_POINTER) { PC = PC + 2; }

void decode_opcode_at(CHIP8 *chip8_p, uint16_t address, char *buffer) {
  DECLARE_OPCODE_VAR;
  OPCODE_VAR = fetch_opcode_at(CHIP8_POINTER, address);

  if (IS_OPCODE_GROUP(0)) {
    if (NN == 0x00E0) {
      DECODE_00E0(buffer);
    } else if (NN == 0x00EE) {
      DECODE_00EE(buffer);
    } else {
      DECODE_0NNN(buffer)
    }
  } else if (IS_OPCODE_GROUP(1)) {
    DECODE_1NNN(buffer);
  } else if (IS_OPCODE_GROUP(2)) {
    DECODE_2NNN(buffer);
  } else if (IS_OPCODE_GROUP(3)) {
    DECODE_3XNN(buffer);
  } else if (IS_OPCODE_GROUP(4)) {
    DECODE_4XNN(buffer);
  } else if (IS_OPCODE_GROUP(5)) {
    DECODE_5XY0(buffer);
  } else if (IS_OPCODE_GROUP(6)) {
    DECODE_6XNN(buffer);
  } else if (IS_OPCODE_GROUP(7)) {
    DECODE_7XNN(buffer);
  } else if (IS_OPCODE_GROUP(8)) {
    if (N == 0x0000) {
      DECODE_8XY0(buffer);
    } else if (N == 0x0001) {
      DECODE_8XY1(buffer);
    } else if (N == 0x0002) {
      DECODE_8XY2(buffer);
    } else if (N == 0x0003) {
      DECODE_8XY3(buffer);
    } else if (N == 0x0004) {
      DECODE_8XY4(buffer);
    } else if (N == 0x0005) {
      DECODE_8XY5(buffer);
    } else if (N == 0x0006) {
      DECODE_8XY6(buffer);
    } else if (N == 0x0007) {
      DECODE_8XY7(buffer);
    } else if (N == 0x000E) {
      DECODE_8XYE(buffer);
    }
  } else if (IS_OPCODE_GROUP(9)) {
    DECODE_9XY0(buffer);

  } else if (IS_OPCODE_GROUP(A)) {
    DECODE_ANNN(buffer);
  } else if (IS_OPCODE_GROUP(B)) {
    DECODE_BNNN(buffer);
  } else if (IS_OPCODE_GROUP(C)) {
    DECODE_CXNN(buffer);
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
      DECODE_FX07(buffer);
    } else if (NN == 0x000A) {
      DECODE_FX0A(buffer);
    } else if (NN == 0x0015) {
      DECODE_FX15(buffer);
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
}

int run(CHIP8 *CHIP8_POINTER) {

  DECLARE_OPCODE_VAR;
  uint16_t jump = 0;

  OPCODE_VAR = fetch_opcode(CHIP8_POINTER);

  if (IS_OPCODE_GROUP(0)) {
    if (NN == 0x00E0) {
      DECODE_00E0(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x00EE) {
      DECODE_00EE(CHIP8_POINTER->instruction_buffer);
    } else {
      DECODE_0NNN(CHIP8_POINTER->instruction_buffer)
    }
  } else if (IS_OPCODE_GROUP(1)) {
    CALL_AND_DECODE(1NNN, CHIP8_POINTER->instruction_buffer)
    jump = NNN;
  } else if (IS_OPCODE_GROUP(2)) {
    DECODE_2NNN(CHIP8_POINTER->instruction_buffer);
  } else if (IS_OPCODE_GROUP(3)) {
    CALL_AND_DECODE(3XNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(4)) {
    CALL_AND_DECODE(4XNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(5)) {
    CALL_AND_DECODE(5XY0, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(6)) {
    CALL_AND_DECODE(6XNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(7)) {
    CALL_AND_DECODE(7XNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(8)) {
    if (N == 0x0000) {
      CALL_AND_DECODE(8XY0, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0001) {
      CALL_AND_DECODE(8XY1, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0002) {
      CALL_AND_DECODE(8XY2, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0003) {
      CALL_AND_DECODE(8XY3, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0004) {
      CALL_AND_DECODE(8XY4, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0005) {
      CALL_AND_DECODE(8XY5, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x0006) {
      CALL_AND_DECODE(8XY6, CHIP8_POINTER->instruction_buffer);
    } else if (N == 0x0007) {
      CALL_AND_DECODE(8XY7, CHIP8_POINTER->instruction_buffer)
    } else if (N == 0x000E) {
      CALL_AND_DECODE(8XYE, CHIP8_POINTER->instruction_buffer);
    }
  } else if (IS_OPCODE_GROUP(9)) {
    CALL_AND_DECODE(9XY0, CHIP8_POINTER->instruction_buffer)
    jump = 1;
  } else if (IS_OPCODE_GROUP(A)) {
    CALL_AND_DECODE(ANNN, CHIP8_POINTER->instruction_buffer);
  } else if (IS_OPCODE_GROUP(B)) {
    CALL_AND_DECODE(BNNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(C)) {
    CALL_AND_DECODE(CXNN, CHIP8_POINTER->instruction_buffer)
  } else if (IS_OPCODE_GROUP(D)) {
    CALL_AND_DECODE(DXYN, CHIP8_POINTER->instruction_buffer);
  } else if (IS_OPCODE_GROUP(E)) {
    if (NN == 0x009E) {
      DECODE_EX9E(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x00A1) {
      DECODE_EXA1(CHIP8_POINTER->instruction_buffer);
    }
  } else if (IS_OPCODE_GROUP(F)) {
    if (NN == 0x0007) {
      CALL_AND_DECODE(FX07, CHIP8_POINTER->instruction_buffer)
    } else if (NN == 0x000A) {
      DECODE_FX0A(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x0015) {
      CALL_AND_DECODE(FX15, CHIP8_POINTER->instruction_buffer)
    } else if (NN == 0x0018) {
      DECODE_FX18(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x001E) {
      DECODE_FX1E(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x0029) {
      DECODE_FX29(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x0033) {
      DECODE_FX33(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x0055) {
      DECODE_FX55(CHIP8_POINTER->instruction_buffer);
    } else if (NN == 0x0065) {
      DECODE_FX65(CHIP8_POINTER->instruction_buffer);
    }
  } else {
    sprintf(CHIP8_POINTER->instruction_buffer, "");
  }

  if (OPCODE_VAR != 0x0000) {
    printf("[$%04X]: %04X --> %s ", PC, OPCODE_VAR,
           CHIP8_POINTER->instruction_buffer);
    debug_registers(CHIP8_POINTER);
    printf("\n");
  }

  if (jump) {
    PC = jump;
    jump = 0;
  } else
    STEP;

  if (DELAY_TIMER > 0) {
    DELAY_TIMER--;
  } else {
    DELAY_TIMER = 0;
  }
  //}
  return 0;
}
