#include <SDL.h>
#include <iostream>
#include "kingdoms.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "Hello Kingdoms!\n";
    funct();

    SDL_Quit();
    return 0;
}