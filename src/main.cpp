#include <stdio.h>
#include <math.h>
#include <iostream>

#include "Syntheloper.h"

#include "SDL2/SDL.h"
#undef main

int CreateSDLWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        680, 480,
        0);

    if (!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);

    if (!window_surface)
    {
        std::cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);

    return 0;
}

bool IsRunning = true;

void HandleMonoKeyPressEvent(int key) {

}

// Process Input
void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: {
            if (!event.key.repeat) {
                HandleMonoKeyPressEvent(event.key.keysym.sym);
            }
        }
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        IsRunning = false;
    }
};

void AppLoop() {
    while (IsRunning) {
        ProcessInput();
    }
}


int main() {
    CreateSDLWindow();
    std::cout << "sdl window creation complete" << std::endl;

    Syntheloper SynthSystem;
    SynthSystem.customSetUp();
    SynthSystem.openStream();

    AppLoop();


    SynthSystem.Terminate();
}