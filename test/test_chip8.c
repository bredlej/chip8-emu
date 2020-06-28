#include <unity.h>
#include <chip8.h>

CHIP8* CHIP8_POINTER;

void setUp(void) {
    CHIP8_POINTER = init_chip8();
}
void tearDown(void) { 
    free_memory(CHIP8_POINTER);
}

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
    STEP
    TEST_ASSERT_EQUAL_HEX8(MEMORY_PROGRAM_START + 2, PC);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_Should_Initialize_Chip8);
    RUN_TEST(test_Step_Should_Increase_PC_By_Two);

    return UNITY_END();
}