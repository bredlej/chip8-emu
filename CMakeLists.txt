# CMakeList.txt : CMake project for CHIP-8 Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("CHIP-8 Emulator" C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wno-error")
set(SDL2_DIR "external/SDL2-2.0.12")

enable_testing()
add_subdirectory(Chip8)
add_subdirectory(test)
add_subdirectory(external)

add_executable (chip8-emu "main.c")
include(CTest)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_include_directories(chip8-emu PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/Chip8"
	"${PROJECT_SOURCE_DIR}/include"
	)
target_link_libraries(chip8-emu PUBLIC Chip8 ${CMAKE_CURRENT_LIST_DIR} ${SDL2_LIBRARIES})

add_custom_command(
        TARGET chip8-emu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/roms
                ${CMAKE_CURRENT_BINARY_DIR}/roms)
