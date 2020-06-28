add_executable(test_chip8 test_chip8.c)
target_link_libraries(test_chip8 Chip8 Unity)
add_test(test_chip8_all test_chip8)