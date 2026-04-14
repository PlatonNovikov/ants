#define HEIGHT 165
#define WIDTH 639

#define clear printf("\033[H\033[J")

int grid[HEIGHT][WIDTH] = {0};

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} e_dir;

int abs(int a, int b)
{
    return ((a % b + b) % b);
}

void draw_grid()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
            printf("%c", " #"[grid[y][x]]);
        printf("\n");
    }
}

int main()
{
    int y = 49;
    int x = 99;
    e_dir dir = UP;

    while (1)
    {
        draw_grid();

        if (!grid[y][x])
        {
            grid[y][x] = 1;
            dir = abs(dir - 1, 4);
        }
        else
        {
            grid[y][x] = 0;
            dir = abs(dir + 1, 4);
        }
        switch (dir)
        {
        case UP:
            y = abs(y - 1, HEIGHT);
            break;

        case RIGHT:
            x = abs(x + 1, WIDTH);
            break;

        case DOWN:
            y = abs(y + 1, HEIGHT);
            break;

        case LEFT:
            x = abs(x - 1, WIDTH);
            break;

        default:
            break;
        }
        usleep(100);
        clear;
    }
}
