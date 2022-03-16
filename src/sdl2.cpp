#include "sdl2.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <iostream>

#define n_ptr else std::cout << "[error]\n" << '\n';

namespace sdl2
{

void SDL_Deleter::operator()(SDL_Window * ptr) { if (ptr) SDL_DestroyWindow(ptr); n_ptr }
void SDL_Deleter::operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); n_ptr }
void SDL_Deleter::operator()(SDL_Surface* ptr) { if (ptr) SDL_FreeSurface(ptr); n_ptr }
void SDL_Deleter::operator()(SDL_Texture* ptr) { if (ptr) SDL_DestroyTexture(ptr); n_ptr }

void SDL_Deleter::operator()(TTF_Font* ptr) { if (ptr) TTF_CloseFont(ptr); }

}