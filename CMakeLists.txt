# CMakeList.txt : CMake project for CHIP-8 Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("CHIP-8 Emulator" C)
set(CMAKE_C_STANDARD 11)
add_subdirectory(Chip8)
# Add source to this project's executable.
add_executable (chip8-emu "main.c")
target_include_directories(chip8-emu PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/Chip8"
	)

target_link_libraries(chip8-emu PUBLIC Chip8)
# TODO: Add tests and install targets if needed.
