#define HEIGHT 165
#define WIDTH 639
#define CELL_SIZE 4
#define WINDOW_W (WIDTH * CELL_SIZE)
#define WINDOW_H (HEIGHT * CELL_SIZE)

#include <string.h>
#include <inttypes.h>
#include <SDL2/SDL.h>

#define move(y, x) printf("\033[%d;%dH", (y)+1, (x)+1)
#define rgb(clr) printf("\033[38;2;%d;%d;%dm", (clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff)\

uint32_t grid[HEIGHT][WIDTH] = {0};

int seed;

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} e_dir;

int aabs(int a, int b)
{
    return ((a % b + b) % b);
}

void set_rgb(int y, int x)
{
    srand(seed + (grid[y][x] >> 29));
    int clrbyte = (1 << ((25 + rand() % 3)));
    grid[y][x] += 1 << 29;
    if ((grid[y][x] >> 29) == 0b111)
        grid[y][x] ^= 0b111 << 29;
    grid[y][x] = (grid[y][x] & 0b11100001111111111111111111111111) + clrbyte;
    switch ((clrbyte >> 25) & 0b111)
    {
    case 0b100:
        if (((grid[y][x] >> 16) & 0xff) == 0xff)
            grid[y][x] |= 1 << 28;
        break;

    case 0b010:
        if (((grid[y][x] >> 8) & 0xff) == 0xff)
            grid[y][x] |= 1 << 28;
        break;

    case 0b001:
        if ((grid[y][x] & 0xff) == 0xff)
            grid[y][x] |= 1 << 28;
        break;

    default:
        break;
    }
}

void add_rgb(int y, int x)
{
    int dir = (grid[y][x] >> 28) & 1 ? -1 : 1;
    if (!((grid[y][x] >> 25) & 0b111))
        set_rgb(y, x);

    switch (grid[y][x] >> 25 & 0b111)
    {
    case 0b100:
        grid[y][x] += 0x010000 * dir;
        if ((dir == -1) && (((grid[y][x] >> 16) & 0xff) == 0))
            set_rgb(y, x);
        else if (((grid[y][x] >> 16) & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    case 0b010:
        grid[y][x] += 0x0100 * dir;
        if ((dir == -1) && (((grid[y][x] >> 8) & 0xff) == 0))
            set_rgb(y, x);
        else if (((grid[y][x] >> 8) & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    case 0b001:
        grid[y][x] += 0x01 * dir;
        if ((dir == -1) && ((grid[y][x] & 0xff) == 0))
            set_rgb(y, x);
        else if ((grid[y][x] & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    default:
        break;
    }
}

void random_grid()
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            grid[y][x] = (rand() & 1) << 24;
}

int main(int argc, char **argv){    (void)argc; (void)argv;    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
    SDL_Window *win = SDL_CreateWindow("SDL2 Automaton",                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,                                       WINDOW_W, WINDOW_H, 0);    if (!win) { SDL_Quit(); return 2; }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);    if (!ren) { SDL_DestroyWindow(win); SDL_Quit(); return 3; }
    srand((unsigned)time(NULL));    random_grid();
    int y = HEIGHT / 2;    int x = WIDTH / 2;    e_dir dir = UP;
    int running = 1;    SDL_Event ev;
    const Uint32 frame_delay_ms = 0; // set to 0 for max speed, or e.g. 16 for ~60fps
    // initial clear    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);    SDL_RenderClear(ren);    SDL_RenderPresent(ren);
    while (running)    {        // handle events        while (SDL_PollEvent(&ev))        {            if (ev.type == SDL_QUIT) running = 0;            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = 0;        }
        // current cell        uint32_t cell = grid[y][x];        uint8_t r = (cell >> 16) & 0xFF;        uint8_t g = (cell >> 8) & 0xFF;        uint8_t b = cell & 0xFF;        uint32_t flip = (cell >> 24) & 1;
        // draw cell (filled rectangle)        SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };        SDL_SetRenderDrawColor(ren, r, g, b, 255);        SDL_RenderFillRect(ren, &rect);
        // flip the high bit        grid[y][x] = grid[y][x] ^ (1u << 24);
        // turn based on previous flip (use flipped value as in your original logic):        // In original code you read cell, add rgb, then toggle, then used the toggled bit to choose turn.        // The code here uses flip (the old bit) to decide direction inverse of original order,        // so mirror the original: after toggling, look at new bit:        uint32_t newflip = (grid[y][x] >> 24) & 1;        dir = (e_dir)aabs(dir + (newflip ? 1 : -1), 4);
        // step        switch (dir)        {            case UP:    y = aabs(y - 1, HEIGHT); break;            case RIGHT: x = aabs(x + 1, WIDTH);  break;            case DOWN:  y = aabs(y + 1, HEIGHT); break;            case LEFT:  x = aabs(x - 1, WIDTH);  break;        }
        // Present incremental update (fast). You may prefer to present only occasionally.        SDL_RenderPresent(ren);
        if (frame_delay_ms) SDL_Delay(frame_delay_ms);    }
    SDL_DestroyRenderer(ren);    SDL_DestroyWindow(win);    SDL_Quit();    return 0;}
