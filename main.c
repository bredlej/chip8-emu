#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <SDL.h>

int main(int argc, char **argv) {
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "SDL2Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 90, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000);

    SDL_DestroyWindow(window);
    SDL_Quit();

  CHIP8 *CHIP8_POINTER = init_chip8();

  FILE *rom_file_p = fopen("roms/PONG", "rb");
  if (!rom_file_p) {
    printf("Can't open file.\n");
  } else {
    uint8_t buffer;
    uint16_t memory_address = MEMORY_PROGRAM_START;
    while (fread(&buffer, sizeof buffer, 1, rom_file_p)) {
      MEMORY(memory_address++) = buffer;
    };
    fclose(rom_file_p);
  }

  if (dump_memory(CHIP8_POINTER))
    printf("Error dumping memory.\n");
  run(CHIP8_POINTER);
  dump_registers(CHIP8_POINTER);
  free_memory(CHIP8_POINTER);
  return 0;
}
