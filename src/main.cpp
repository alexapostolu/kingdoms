#include <SDL.h>
#include <SDL_ttf.h>

#include "tile.hpp"

#include <stdio.h>
#include <iostream>

#define SCREEN_WIDTH 1170
#define SCREEN_HEIGHT 525

int main(int argv, char** args)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Nighthawk - Kingdoms", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window)
    {
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderClear(renderer);

        if (TTF_Init() == -1)
        {
            std::cout << "TTF_Init() Failed: " << TTF_GetError();
            return 0;
        }

        TTF_Font* Sans = TTF_OpenFont("C:\\dev\\repos\\nighthawk-kingdoms\\src\\brygada.ttf", 50);
 
        SDL_Color Black = { 0, 0, 0 };

        SDL_Surface* surfaceMessage =
            TTF_RenderText_Solid(Sans, "Gold: 4267", Black);

        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        SDL_Rect Message_rect; //create a rect
        Message_rect.x = 0;  //controls the rect's x coordinate 
        Message_rect.y = 0; // controls the rect's y coordinte
        Message_rect.w = 500; // controls the width of the rect
        Message_rect.h = 100; // controls the height of the rect

        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);



        SDL_RenderPresent(renderer);
    }

    return 0;
}