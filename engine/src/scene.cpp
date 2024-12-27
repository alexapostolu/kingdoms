#include "scene.hpp"

#include "SDL.h"

Scene* Scene::current_scene;
Scene* Scene::scenes[5];

Scene::Scene(SDL_Window* _window, SDL_Renderer* _renderer)
	: window(_window), renderer(_renderer)
	, scale(1.0f)
{

}
