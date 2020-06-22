# CMakeList.txt : CMake project for CHIP-8 Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("CHIP-8 Emulator" C)

# Add source to this project's executable.
add_executable (chip8-emu "system.c" "system.h")

# TODO: Add tests and install targets if needed.
