#include <iostream>
#include <SDL2/SDL.h>
#include "grid.h"
#include "Astar.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "Error Initializing SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    Grid grid;

    SDL_Window *win = SDL_CreateWindow("Path Finding Sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, 0);

    Uint32 renderflags = SDL_RENDERER_ACCELERATED;

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, renderflags);

    grid.init();

    bool close = false;
    while (!close)
    {
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!grid.aStar)
                {
                    grid.place_blocks = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (!grid.aStar)
                {
                    grid.place_blocks = false;
                }
                break;
            }
            if (event.type == SDL_KEYDOWN)
            {
                grid.clearPath();
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    close = true;
                    break;
                case SDLK_q:
                    if (!grid.aStar)
                    {
                        grid.start = true;
                        grid.wall = false;
                        grid.end = false;
                        grid.erase = false;
                    }
                    break;
                case SDLK_w:
                    if (!grid.aStar)
                    {
                        grid.start = false;
                        grid.wall = true;
                        grid.end = false;
                        grid.erase = false;
                    }
                    break;
                case SDLK_e:
                    if (!grid.aStar)
                    {
                        grid.start = false;
                        grid.wall = false;
                        grid.end = true;
                        grid.erase = false;
                    }
                    break;
                case SDLK_r:
                    if (!grid.aStar)
                    {
                        grid.start = false;
                        grid.wall = false;
                        grid.end = false;
                        grid.erase = true;
                    }
                    break;
                case SDLK_a:
                    grid.aStar = true;
                    break;
                }
            }
        }
        if (grid.place_blocks && !grid.aStar)
        {
            grid.placeBlock();
        }
        if (grid.aStar)
        {
            Astar astar;

            grid.loadMatrix();

            Pair src = std::make_pair(grid.start_block_pos_relative_to_grid.y, grid.start_block_pos_relative_to_grid.x);
            Pair dest = std::make_pair(grid.end_block_pos_relative_to_grid.y, grid.end_block_pos_relative_to_grid.x);

            astar.aStarSearch(grid.wallGrid, src, dest, false);
            grid.loadPath(astar.aStar_trace_path);

            grid.aStar = false;
        }

        grid.draw(rend);
        grid.drawOutlineBlock(rend);
        grid.drawGrid(rend);

        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}