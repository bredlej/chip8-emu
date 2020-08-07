add_library(Chip8 "chip8.c" "chip8.h" "pcg_basic.c" "pcg_basic.h")
target_include_directories(Chip8
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )
install(TARGETS Chip8 DESTINATION lib)
install(FILES chip8.h DESTINATION include)