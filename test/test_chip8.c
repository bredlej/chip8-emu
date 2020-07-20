#include <chip8.h>
#include <unity.h>

CHIP8 *CHIP8_POINTER;

void setUp(void) { CHIP8_POINTER = init_chip8(); }
void tearDown(void) { free_memory(CHIP8_POINTER); }

void test_Should_Initialize_Chip8(void) {
  TEST_ASSERT_NOT_NULL(CHIP8_POINTER);
  for (int i = MEMORY_PROGRAM_START; i < MEMORY_SIZE; i++) {
    TEST_ASSERT_EQUAL_HEX8(0x00, MEMORY(i));
  }
  for (int i = 0; i < REGISTERS_AMOUNT; i++) {
    TEST_ASSERT_EQUAL_HEX8(0x00, REGISTER(i));
  }
  TEST_ASSERT_EQUAL_HEX8(0x00, ADDRESS_REGISTER);
  TEST_ASSERT_EQUAL_HEX8(0x00, PC);
}

void test_Step_Should_Increase_PC_By_Two(void) {
  PC = MEMORY_PROGRAM_START;
  STEP;
  TEST_ASSERT_EQUAL_HEX8(MEMORY_PROGRAM_START + 2, PC);
}

void test_Should_Group_Opcodes(void) {
  DECLARE_OPCODE_VAR
  OPCODE_VAR = 0x0000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(0));
  OPCODE_VAR = 0x00E0;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(0));
  OPCODE_VAR = 0x00EE;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(0));
  OPCODE_VAR = 0x1000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(1));
  OPCODE_VAR = 0x2000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(2));
  OPCODE_VAR = 0x3000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(3));
  OPCODE_VAR = 0x4000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(4));
  OPCODE_VAR = 0x5000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(5));
  OPCODE_VAR = 0x6000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(6));
  OPCODE_VAR = 0x7000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(7));
  OPCODE_VAR = 0x8000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8001;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8002;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8003;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8004;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8005;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8006;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x8007;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x800E;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(8));
  OPCODE_VAR = 0x9000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(9));
  OPCODE_VAR = 0xA000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(A));
  OPCODE_VAR = 0xB000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(B));
  OPCODE_VAR = 0xC000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(C));
  OPCODE_VAR = 0xD000;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(D));
  OPCODE_VAR = 0xE09E;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(E));
  OPCODE_VAR = 0xE0A1;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(E));
  OPCODE_VAR = 0xF007;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF00A;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF015;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF018;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF01E;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF029;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF033;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF055;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
  OPCODE_VAR = 0xF065;
  TEST_ASSERT_TRUE(IS_OPCODE_GROUP(F));
}

void test_Should_Parse_Opcodes(void) {
  DECLARE_OPCODE_VAR;
  OPCODE_VAR = 0x1234;
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(NNN, 0x234, "NNN parsing incorrect.");
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(NN, 0x034, "NN parsing incorrect.");
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(N, 0x004, "N parsing incorrect.");
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(VX, 0x002, "VX parsing incorrect.");
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(VY, 0x003, "VY parsing incorrect.");
}

void test_Should_Fetch_Opcode_From_PC(void) {
  PC = MEMORY_PROGRAM_START;
  MEMORY(MEMORY_PROGRAM_START + 0) = 0x12;
  MEMORY(MEMORY_PROGRAM_START + 1) = 0x00;
  MEMORY(MEMORY_PROGRAM_START + 2) = 0x64;
  MEMORY(MEMORY_PROGRAM_START + 3) = 0x15;
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(0x1200, fetch_opcode(CHIP8_POINTER),
                                  "Wrong opcode fetched from PC");
  STEP;
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(0x6415, fetch_opcode(CHIP8_POINTER),
                                  "Wrong opcode fetched from PC");
}

void test_Should_Determine_Most_Significant_Bit(void) {
  REGISTER(V0) = 0b00000001;
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, MSB(V0), "Wrong MSB(x) calculation.");
  REGISTER(V0) = 0b10000000;
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, MSB(V0), "Wrong MSB(x) calculation.");
}

void test_Should_Determine_Least_Significant_Bit(void) {
  REGISTER(V0) = 0b00000001;
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, LSB(V0), "Wrong LSB(x) calculation.");
  REGISTER(V0) = 0b10000000;
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, LSB(V0), "Wrong LSB(x) calculation.");
}

void test_Instruction_0NNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_00E0() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_00EE() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_1NNN() {
  PC = 0x000;
  f_1NNN(CHIP8_POINTER, (uint16_t)0x1200);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(0x0200, PC, "Jumped to wrong address.");
  f_1NNN(CHIP8_POINTER, (uint16_t)0x1E01);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(0x0E01, PC, "Jumped to wrong address.");
}
void test_Instruction_2NNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_3XNN() {
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x01;
  f_3XNN(CHIP8_POINTER, 0x30FF);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START, PC,
                                  "Wrong PC calculation result.");
  f_3XNN(CHIP8_POINTER, 0x3001);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START + 2, PC,
                                  "Wrong PC calculation result.");
}
void test_Instruction_4XNN() {
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x01;
  f_4XNN(CHIP8_POINTER, 0x4001);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START, PC,
                                  "Wrong PC calculation result.");
  f_4XNN(CHIP8_POINTER, 0x40FF);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START + 2, PC,
                                  "Wrong PC calculation result.");
}
void test_Instruction_5XY0() {
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x01;
  REGISTER(V1) = 0x00;
  f_5XY0(CHIP8_POINTER, 0x5010);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START, PC,
                                  "Wrong PC calculation result.");
  REGISTER(V1) = 0x01;
  f_5XY0(CHIP8_POINTER, 0x5010);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START + 2, PC,
                                  "Wrong PC calculation result.");
}
void test_Instruction_6XNN() {
  f_6XNN(CHIP8_POINTER, (uint16_t)0x6010);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x10, REGISTER(V0),
                                 "Wrong Vx == NN instruction.");
  f_6XNN(CHIP8_POINTER, (uint16_t)0x6501);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, REGISTER(V5),
                                 "Wrong Vx == NN instruction.");
  f_6XNN(CHIP8_POINTER, (uint16_t)0x6FCF);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xCF, REGISTER(VF),
                                 "Wrong Vx == NN instruction.");
}
void test_Instruction_7XNN() {
  REGISTER(V0) = 0x01;
  REGISTER(V1) = 0x01;
  REGISTER(V2) = 0xFF;
  f_7XNN(CHIP8_POINTER, (uint16_t)0x7001);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x02, REGISTER(V0),
                                 "Wrong Vx += NN instruction.");
  f_7XNN(CHIP8_POINTER, (uint16_t)0x7001);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x03, REGISTER(V0),
                                 "Wrong Vx += NN instruction.");
  f_7XNN(CHIP8_POINTER, (uint16_t)0x71FF);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x03, REGISTER(V0),
                                 "Wrong Vx += NN instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, REGISTER(V1),
                                 "Wrong Vx += NN instruction.");
  f_7XNN(CHIP8_POINTER, (uint16_t)0x7200);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xFF, REGISTER(V2),
                                 "Wrong Vx += NN instruction.");
}
void test_Instruction_8XY0() {
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x01;
  f_8XY0(CHIP8_POINTER, (uint16_t)0x8010);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(REGISTER(V1), REGISTER(V0),
                                 "Wrong Vx == Vy instruction.");
  REGISTER(VF) = 0xFF;
  f_8XY0(CHIP8_POINTER, (uint16_t)0x81F0);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(REGISTER(VF), REGISTER(V1),
                                 "Wrong Vx == Vy instruction.");
}
void test_Instruction_8XY1() {
  REGISTER(V0) = 0b00000001;
  REGISTER(V1) = 0b10000000;
  f_8XY1(CHIP8_POINTER, 0x8011);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b10000001, REGISTER(V0),
                                 "Wrong Vx |= Vy instruction.");
  REGISTER(V2) = 0b10010001;
  f_8XY1(CHIP8_POINTER, 0x8021);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b10010001, REGISTER(V0),
                                 "Wrong Vx |= Vy instruction.");
}
void test_Instruction_8XY2() {
  REGISTER(V0) = 0b00000001;
  REGISTER(V1) = 0b10000000;
  f_8XY2(CHIP8_POINTER, 0x8012);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b00000000, REGISTER(V0),
                                 "Wrong Vx &= Vy instruction.");
  REGISTER(V0) = 0b00010001;
  REGISTER(V2) = 0b10010001;
  f_8XY2(CHIP8_POINTER, 0x8022);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b00010001, REGISTER(V0),
                                 "Wrong Vx &= Vy instruction.");
}
void test_Instruction_8XY3() {
  REGISTER(V0) = 0b00000001;
  REGISTER(V1) = 0b10000000;
  f_8XY3(CHIP8_POINTER, 0x8013);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b10000001, REGISTER(V0),
                                 "Wrong Vx ^= Vy instruction.");
  REGISTER(V0) = 0b00010001;
  REGISTER(V2) = 0b10010001;
  f_8XY3(CHIP8_POINTER, 0x8023);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0b10000000, REGISTER(V0),
                                 "Wrong Vx ^= Vy instruction.");
}
void test_Instruction_8XY4() {
  REGISTER(V0) = 0x01;
  REGISTER(V1) = 0x01;
  REGISTER(VF) = NO_CARRY;
  f_8XY4(CHIP8_POINTER, 0x8014);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x02, REGISTER(V0),
                                 "Wrong Vx = Vx + Vy instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(NO_CARRY, REGISTER(VF), "Wrong carry set.");
  REGISTER(V2) = 0xFF;
  f_8XY4(CHIP8_POINTER, 0x8124);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, REGISTER(V1),
                                 "Wrong Vx = Vx + Vy instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(CARRY, REGISTER(VF), "Wrong carry set.");
}
void test_Instruction_8XY5() {
  REGISTER(V0) = 0x03;
  REGISTER(V1) = 0x01;
  f_8XY5(CHIP8_POINTER, 0x8015);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x02, REGISTER(V0),
                                 "Wrong Vx = Vx - Vy instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(NO_CARRY, REGISTER(VF), "Wrong carry set.");
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x01;
  f_8XY5(CHIP8_POINTER, 0x8015);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xFF, REGISTER(V0),
                                 "Wrong Vx = Vx - Vy instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(CARRY, REGISTER(VF), "Wrong carry set.");
}
void test_Instruction_8XY6() {
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x81; // 1000 0001
  f_8XY6(CHIP8_POINTER, 0x8016);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x40, REGISTER(V0),
                                 "Wrong Vx = Vy >> 1 instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, REGISTER(VF),
                                 "Wrong LSB(Vy) calculation.");
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x90; // 1001 0000
  f_8XY6(CHIP8_POINTER, 0x8016);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x48, REGISTER(V0),
                                 "Wrong Vx = Vy >> 1 instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, REGISTER(VF),
                                 "Wrong LSB(Vy) calculation.");
}
void test_Instruction_8XY7() {
  REGISTER(V0) = 0x03;
  REGISTER(V1) = 0x01;
  f_8XY7(CHIP8_POINTER, 0x8017);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xFE, REGISTER(V0),
                                 "Wrong Vx = Vy - Vx instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(CARRY, REGISTER(VF), "Wrong carry set.");
  REGISTER(V0) = 0x01;
  REGISTER(V1) = 0x02;
  f_8XY7(CHIP8_POINTER, 0x8017);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, REGISTER(V0),
                                 "Wrong Vx = Vy - Vx instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(NO_CARRY, REGISTER(VF), "Wrong carry set.");
}
void test_Instruction_8XYE() {
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x81; // 1000 0001
  f_8XYE(CHIP8_POINTER, 0x801E);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x02, REGISTER(V0),
                                 "Wrong Vx = Vy << 1 instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x01, REGISTER(VF),
                                 "Wrong MSB(Vy) calculation.");
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x05; // 0000 0101
  f_8XYE(CHIP8_POINTER, 0x801E);
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x0A, REGISTER(V0),
                                 "Wrong Vx = Vy << 1 instruction.");
  TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, REGISTER(VF),
                                 "Wrong MSB(Vy) calculation.");
}
void test_Instruction_9XY0() {
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x00;
  f_9XY0(CHIP8_POINTER, 0x9010);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START, PC,
                                  "Wrong PC position after instruction call.");
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x00;
  REGISTER(V1) = 0x01;
  f_9XY0(CHIP8_POINTER, 0x9010);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(MEMORY_PROGRAM_START + 2, PC,
                                  "Wrong PC position after instruction call.");
}

void test_Instruction_ANNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_BNNN() {
  PC = MEMORY_PROGRAM_START;
  REGISTER(V0) = 0x01;
  f_BNNN(CHIP8_POINTER, 0xB300);
  TEST_ASSERT_EQUAL_HEX16_MESSAGE(
      0x301, PC, "Wrong address calculation in PC = Vx + NNN.");
}
void test_Instruction_CXNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_DXYN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_EX9E() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_EXA1() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX07() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX0A() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX15() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX18() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX1E() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX29() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX33() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX55() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_FX65() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_Should_Initialize_Chip8);
  RUN_TEST(test_Step_Should_Increase_PC_By_Two);
  RUN_TEST(test_Should_Group_Opcodes);
  RUN_TEST(test_Should_Parse_Opcodes);
  RUN_TEST(test_Should_Fetch_Opcode_From_PC);
  RUN_TEST(test_Should_Determine_Most_Significant_Bit);
  RUN_TEST(test_Should_Determine_Least_Significant_Bit);
  /* Run a test for each Chip8 instruction */
#define OP(x) RUN_TEST(test_Instruction_##x);
  CHIP8_INSTR_SET
#undef OP
  return UNITY_END();
}
