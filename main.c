#include "SDL_FontCache.h"
#include "chip8.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdint.h>
#include <stdio.h>

void render_framebuffer(CHIP8 *chip8_p, SDL_Renderer *renderer, FC_Font *font,
                        uint16_t x, uint16_t y, uint8_t is_debug) {
  for (int i = 0; i < BITMAP_SIZE; i++) {
    for (uint8_t bit_offset = 0; bit_offset < 8; bit_offset++) {
      SDL_Rect r;
      r.x = x + ((i % 8) * 80) + (bit_offset * 10);
      r.y = y + ((i >> 3) * 10);
      r.w = 10;
      r.h = 10;
      if (((0x80 >> bit_offset) & FRAMEBUFFER(i)) > 0) {
        SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
        SDL_RenderFillRect(renderer, &r);
      } else {
        if (is_debug) {
          if ((0x80 >> bit_offset & 1) == 1 && (i * 8) % 8 == 0) {
            SDL_SetRenderDrawColor(renderer, 25, 75, 25, 255);
          } else {
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
          }
          if (!(0x1 >> bit_offset & 1) == 1) {
            SDL_RenderDrawRect(renderer, &r);
          } else {
            FC_Draw(font, renderer, r.x, r.y - 2, "%02X", i);
          }
        }
      }
    }
  }
}
void render_sprite_preview(CHIP8 *chip8_p, SDL_Renderer *renderer,
                           FC_Font *font, uint16_t x, uint16_t y) {
  FC_Draw(font, renderer, x + 5, y + 5, "Sprite");
  for (uint16_t sprite_address = chip8_p->I; sprite_address < chip8_p->I + 16;
       sprite_address++) {
    uint16_t sprite = chip8_p->memory[sprite_address];
    for (uint8_t bit_offset = 0; bit_offset < 8; bit_offset++) {
      SDL_Rect r;
      r.x = x + (bit_offset * 10);
      r.y = y + 35;
      r.w = 10;
      r.h = 10;
      if (((0x80 >> bit_offset) & sprite) > 0) {
        SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
        SDL_RenderFillRect(renderer, &r);
      } else {
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderDrawRect(renderer, &r);
      }
    }
    y = y + 10;
  }
}
void render_instructions(CHIP8 *chip8_p, SDL_Renderer *renderer, FC_Font *font,
                         FC_Font *font_grey, uint16_t x, uint16_t y) {
  char buffer[50];
  for (int i = 4; i > 0; i--) {
    decode_opcode_at(chip8_p, PC - (i * 2), buffer);
    FC_Draw(font_grey, renderer, x, y, "[$%04X]: %04X --> %s ", PC - (i * 2),
            fetch_opcode_at(CHIP8_POINTER, PC - (i * 2)), buffer);
    y = y + 30;
  }
  decode_opcode_at(chip8_p, PC, buffer);
  FC_Draw(font, renderer, x, y, "[$%04X]: %04X --> %s ", PC,
          fetch_opcode_at(CHIP8_POINTER, PC), buffer);
  y = y + 30;
  for (int i = 1; i < 5; i++) {
    decode_opcode_at(chip8_p, PC + (i * 2), buffer);
    FC_Draw(font_grey, renderer, x, y, "[$%04X]: %04X --> %s ", PC + (i * 2),
            fetch_opcode_at(CHIP8_POINTER, PC + (i * 2)), buffer);
    y = y + 30;
  }
}

void render_register_8(CHIP8 *chip8_p, SDL_Renderer *renderer, FC_Font *font,
                       FC_Font *font_grey, uint16_t x, uint16_t y, uint8_t vx,
                       uint8_t is_text_above, char *label, char *value) {
  SDL_Rect r;
  r.x = x;
  r.y = y + (is_text_above ? 30 : 0);
  r.w = 30;
  r.h = 30;
  SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
  SDL_RenderDrawRect(renderer, &r);
  FC_Draw(font_grey, renderer, x + 10, is_text_above ? y + 5 : y + 32, label);
  FC_Draw(font, renderer, x + 5, is_text_above ? y + 32 : y + 5, value);
}

void render_register_16(CHIP8 *chip8_p, SDL_Renderer *renderer, FC_Font *font,
                        FC_Font *font_grey, uint16_t x, uint16_t y, uint8_t vx,
                        uint8_t is_text_above, char *label, char *value) {
  SDL_Rect r;
  r.x = x;
  r.y = y + (is_text_above ? 30 : 0);
  r.w = 60;
  r.h = 30;
  SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
  SDL_RenderDrawRect(renderer, &r);
  FC_Draw(font_grey, renderer, x + 10, is_text_above ? y + 5 : y + 32, label);
  FC_Draw(font, renderer, x + 5, is_text_above ? y + 32 : y + 5, value);
}

void render_registers(CHIP8 *chip8_p, SDL_Renderer *renderer, FC_Font *font,
                      FC_Font *font_grey, uint16_t x, uint16_t y) {
  uint16_t offset_x = x;
  uint16_t offset_y = y;
  uint8_t first_skipped = 0;
  char register_label[10];
  char value[10];

  for (int i = 0; i < 8; i++) {
    sprintf(value, "%02X", REGISTER(i));
    sprintf(register_label, "%01X", i);
    render_register_8(chip8_p, renderer, font, font_grey, offset_x, offset_y, i,
                      1, register_label, value);
    offset_x += 35;
  }
  offset_x = x;
  offset_y += 60;
  for (int i = 0; i < 8; i++) {
    sprintf(value, "%02X", REGISTER(i + 8));
    sprintf(register_label, "%01X", i + 8);
    render_register_8(chip8_p, renderer, font, font_grey, offset_x, offset_y,
                      i + 8, 0, register_label, value);
    offset_x += 35;
  }
  offset_x += 10;
  sprintf(value, "%02X", chip8_p->delay_timer);
  sprintf(register_label, "DT");
  render_register_8(chip8_p, renderer, font, font_grey, offset_x, y, 8, 1,
                    register_label, value);
  sprintf(value, "%04X", chip8_p->I);
  sprintf(register_label, "ADDR");
  render_register_16(chip8_p, renderer, font, font_grey, offset_x, offset_y, 8,
                     0, register_label, value);
}

int main(int argc, char **argv) {
  CHIP8 *CHIP8_POINTER = init_chip8();

  FILE *rom_file_p = fopen("roms/BLINKY", "rb");
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

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("Bredlej's Chip8 emulator", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 1200, 600, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

  FC_Font *font = FC_CreateFont();
  FC_LoadFont(font, renderer, "fonts/basis33.ttf", 24,
              FC_MakeColor(230, 230, 230, 255), TTF_STYLE_NORMAL);
  FC_Font *font_debug = FC_CreateFont();
  FC_LoadFont(font_debug, renderer, "fonts/FreeSans.ttf", 10,
              FC_MakeColor(230, 230, 230, 255), TTF_STYLE_NORMAL);
  FC_Font *font_grey = FC_CreateFont();
  FC_LoadFont(font_grey, renderer, "fonts/basis33.ttf", 24,
              FC_MakeColor(130, 130, 130, 255), TTF_STYLE_NORMAL);

  FC_Font *font_small = FC_CreateFont();
  FC_LoadFont(font_small, renderer, "fonts/basis33.ttf", 12,
              FC_MakeColor(230, 230, 230, 255), TTF_STYLE_NORMAL);

  uint8_t quit = 0;
  SDL_Event event;
  PC = MEMORY_PROGRAM_START;
  DELAY_TIMER = 0x01;

  uint8_t is_debug = 0;

  while (!quit) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      quit = 1;
      break;
    }
    switch (event.key.keysym.sym) {
    case SDLK_LEFT: {
      break;
    }
    case SDLK_RIGHT:
      break;
    case SDLK_UP:
      break;
    case SDLK_DOWN:
      if (event.key.state == SDL_PRESSED) {
        //
        if (is_debug) {
          run(CHIP8_POINTER);
          SDL_Delay(100);
        }
      }
      break;
    case SDLK_SPACE: {
      if (event.key.state == SDL_PRESSED) {
        //
        is_debug = is_debug ? 0 : 1;
      }

      break;
    }
    }
    if (!is_debug) {
      run(CHIP8_POINTER);
    }
    else {
        FC_Draw(font, renderer, 280, 400, "Debug mode");
    }
    render_registers(CHIP8_POINTER, renderer, font, font_grey, 690, 320);
    render_sprite_preview(CHIP8_POINTER, renderer, font_grey, 1050, 320);
    render_instructions(CHIP8_POINTER, renderer, font, font_grey, 690, 50);
    render_framebuffer(CHIP8_POINTER, renderer, font_debug, 20, 50, is_debug);
    SDL_RenderPresent(renderer);
  }
  dump_registers(CHIP8_POINTER);
  free_memory(CHIP8_POINTER);

  FC_FreeFont(font);
  FC_FreeFont(font_small);
  SDL_Delay(3000);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
