#define HEIGHT 165
#define WIDTH 639

#include <string.h>

#define clear printf("\033[H\033[J")

#define move(y, x) printf("\033[%d;%dH", (y)+1, (x)+1)

int grid[HEIGHT][WIDTH] = {0};

const char *colors[] = {
    "\033[38;2;0;0;0m",
    "\033[38;2;0;0;255m",
    "\033[38;2;0;255;0m",
    "\033[38;2;0;255;255m",
    "\033[38;2;255;0;0m",
    "\033[38;2;255;0;255m",
    "\033[38;2;255;255;0m",
    "\033[38;2;255;255;255m"
};

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} e_dir;

int list_color_mov[] = {-1, -1, 1, 1};
const int col_num = 4;

int aabs(int a, int b)
{
    return ((a % b + b) % b);
}

int main()
{
    int y = HEIGHT / 2;
    int x = WIDTH / 2;
    e_dir dir = UP;

    //for (int i = 0; i < 110 * 100; i++)
    while(1)
    {

        grid[y][x] = aabs(grid[y][x] + 1, col_num);
        dir = aabs(dir + list_color_mov[grid[y][x]], 4);

        switch (dir)
        {
        case UP:
            y = aabs(y - 1, HEIGHT);
            break;

        case RIGHT:
            x = aabs(x + 1, WIDTH);
            break;

        case DOWN:
            y = aabs(y + 1, HEIGHT);
            break;

        case LEFT:
            x = aabs(x - 1, WIDTH);
            break;

        default:
            break;
        }
        move(y, x);
        printf("%s#", colors[grid[y][x]]);
        // usleep(1000);
    }
}
