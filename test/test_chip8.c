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
  STEP TEST_ASSERT_EQUAL_HEX8(MEMORY_PROGRAM_START + 2, PC);
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
  STEP TEST_ASSERT_EQUAL_HEX16_MESSAGE(0x6415, fetch_opcode(CHIP8_POINTER),
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
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_4XNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_5XY0() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
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
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY1() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY2() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY3() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY4() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY5() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY6() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XY7() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_8XYE() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_9XY0() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_ANNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
}
void test_Instruction_BNNN() {
  TEST_ASSERT_TRUE_MESSAGE(0 == 1, "Not implemented");
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
