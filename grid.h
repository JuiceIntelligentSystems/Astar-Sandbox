#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <SDL2/SDL.h>

#define WINDOW_SIZE 800
#define GRID_SIZE 20
#define BLOCK_SIZE WINDOW_SIZE / GRID_SIZE

struct pos
{
    int x, y;
};

class Grid
{
private:
    SDL_Rect outline_block;
    SDL_Rect wall_block;
    SDL_Rect start_block;
    SDL_Rect end_block;
    SDL_Rect path_block;

    pos wall_block_pos[GRID_SIZE * GRID_SIZE];
    pos path_block_pos[GRID_SIZE * GRID_SIZE];

public:
    int mouse_x, mouse_y;
    int amt_placed = 0;
    int amt_path_placed = 0;

    bool start = false, wall = true, end = false, erase = false;
    bool place_blocks = false;
    bool aStar = false;

    int wallGrid[GRID_SIZE][GRID_SIZE] = {0};
    pos end_block_pos_relative_to_grid;
    pos start_block_pos_relative_to_grid;
    pos wall_block_pos_relative_to_grid[GRID_SIZE * GRID_SIZE];

    void init();
    void drawGrid(SDL_Renderer *);
    void drawOutlineBlock(SDL_Renderer *);
    void draw(SDL_Renderer *);
    void loadMatrix();
    void loadPath(int path[GRID_SIZE][GRID_SIZE]);
    void clearPath();
    bool placeBlock();
};

void Grid::init()
{
    outline_block.w = BLOCK_SIZE;
    outline_block.h = BLOCK_SIZE;
    outline_block.x = 0;
    outline_block.y = 0;

    start_block.x = -55;
    start_block.y = -55;

    end_block.x = -55;
    end_block.y = -55;
}

void Grid::drawGrid(SDL_Renderer *rend)
{
    SDL_SetRenderDrawColor(rend, 128, 128, 128, 255);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        SDL_RenderDrawLine(rend, 0, i * BLOCK_SIZE, 800, i * BLOCK_SIZE); // Draw Grid Rows
        SDL_RenderDrawLine(rend, i * BLOCK_SIZE, 0, i * BLOCK_SIZE, 800); // Draw Grid Columns
    }
}

void Grid::drawOutlineBlock(SDL_Renderer *rend)
{
    SDL_GetMouseState(&mouse_x, &mouse_y);

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    if (wall)
    {
        SDL_SetRenderDrawColor(rend, 128, 128, 128, 128);
    }
    if (start)
    {
        SDL_SetRenderDrawColor(rend, 255, 0, 0, 128);
    }
    if (end)
    {
        SDL_SetRenderDrawColor(rend, 0, 0, 255, 128);
    }
    if (erase)
    {
        SDL_SetRenderDrawColor(rend, 255, 128, 0, 128);
    }

    outline_block.x = round(mouse_x / (GRID_SIZE * 2)) * (GRID_SIZE * 2);
    outline_block.y = round(mouse_y / (GRID_SIZE * 2)) * (GRID_SIZE * 2);

    SDL_RenderFillRect(rend, &outline_block);
}

void Grid::draw(SDL_Renderer *rend)
{
    if (amt_placed != 0)
    {
        for (int i = 0; i < amt_placed; i++)
        {
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 200);

            wall_block.w = BLOCK_SIZE;
            wall_block.h = BLOCK_SIZE;
            wall_block.x = wall_block_pos[i].x;
            wall_block.y = wall_block_pos[i].y;

            SDL_RenderFillRect(rend, &wall_block);
        }
    }
    if (amt_path_placed != 0)
    {
        for (int i = 0; i < amt_path_placed; i++)
        {
            SDL_SetRenderDrawColor(rend, 173, 216, 245, 175);

            path_block.w = BLOCK_SIZE;
            path_block.h = BLOCK_SIZE;
            path_block.x = path_block_pos[i].x;
            path_block.y = path_block_pos[i].y;

            SDL_RenderFillRect(rend, &path_block);
        }
    }
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 200);
    start_block.w = BLOCK_SIZE;
    start_block.h = BLOCK_SIZE;
    SDL_RenderFillRect(rend, &start_block);

    SDL_SetRenderDrawColor(rend, 0, 0, 255, 200);
    end_block.w = BLOCK_SIZE;
    end_block.h = BLOCK_SIZE;
    SDL_RenderFillRect(rend, &end_block);
}

bool Grid::placeBlock()
{
    if (!erase)
    {
        for (int i = 0; i < amt_placed; i++)
        {
            if ((outline_block.x == wall_block_pos[i].x && outline_block.y == wall_block_pos[i].y) ||
                (outline_block.x == start_block.x && outline_block.y == start_block.y) ||
                (outline_block.x == end_block.x && outline_block.y == end_block.y))
            {
                return false;
            }
        }
    }
    if (erase)
    {
        if (outline_block.x == start_block.x && outline_block.y == start_block.y)
        {
            start_block.x = -55;
            start_block.y = -55;
        }
        if (outline_block.x == end_block.x && outline_block.y == end_block.y)
        {
            end_block.x = -55;
            end_block.y = -55;
        }

        for (int i = 0; i < amt_placed; i++)
        {
            if (outline_block.x == wall_block_pos[i].x && outline_block.y == wall_block_pos[i].y)
            {
                int j;
                for (j = 0; j < amt_placed; j++)
                {
                    if (outline_block.x == wall_block_pos[j].x && outline_block.y == wall_block_pos[j].y)
                    {
                        break;
                    }
                }
                if (j < amt_placed)
                {
                    for (int k = j; k < amt_placed; k++)
                    {
                        wall_block_pos[k] = wall_block_pos[k + 1];
                    }
                }
                amt_placed--;
            }
        }
    }

    if (wall)
    {
        wall_block_pos[amt_placed].x = outline_block.x;
        wall_block_pos[amt_placed].y = outline_block.y;

        amt_placed++;
    }
    if (start)
    {
        start_block.x = outline_block.x;
        start_block.y = outline_block.y;
    }
    if (end)
    {
        end_block.x = outline_block.x;
        end_block.y = outline_block.y;
    }

    return true;
}

void Grid::loadMatrix()
{
    // Load Start Block Coordinates relative to Grid Matrix
    start_block_pos_relative_to_grid.x = start_block.x / (GRID_SIZE * 2);
    start_block_pos_relative_to_grid.y = start_block.y / (GRID_SIZE * 2);

    // Load End Block Coordinates relative to Grid Matrix
    end_block_pos_relative_to_grid.x = end_block.x / (GRID_SIZE * 2);
    end_block_pos_relative_to_grid.y = end_block.y / (GRID_SIZE * 2);

    // Load Wall Matrix
    // NOTE: LOAD WHEN DRAWING IS FINISHED
    for (int i = 0; i < amt_placed; i++)
    {
        wall_block_pos_relative_to_grid[i].x = wall_block_pos[i].x / (GRID_SIZE * 2);
        wall_block_pos_relative_to_grid[i].y = wall_block_pos[i].y / (GRID_SIZE * 2);

        wallGrid[wall_block_pos_relative_to_grid[i].y][wall_block_pos_relative_to_grid[i].x] = 1;
    }

    /*
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            std::cout << wallGrid[i][j];
        }
        std::cout << std::endl;
    }
    */
}

void Grid::loadPath(int path[GRID_SIZE][GRID_SIZE])
{
    int i, j;
    for (i = 0; i < GRID_SIZE; i++)
    {
        for (j = 0; j < GRID_SIZE; j++)
        {
            if (path[i][j] == 1)
            {
                path_block_pos[amt_path_placed].x = j * (GRID_SIZE * 2);
                path_block_pos[amt_path_placed].y = i * (GRID_SIZE * 2);

                amt_path_placed++;
            }
        }
    }
}

void Grid::clearPath()
{
    amt_path_placed = 0;
}

#endif