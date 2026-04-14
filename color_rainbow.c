#define HEIGHT 165
#define WIDTH 639

#include <string.h>
#include <inttypes.h>

#define clear printf("\033[H\033[J")

#define move(y, x) printf("\033[%d;%dH", (y)+1, (x)+1)
#define rgb(clr) printf("\033[38;2;%d;%d;%dm", (clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff)

__uint64_t grid[HEIGHT][WIDTH] = {0};

int b_color = 0x0;

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


//last bytes:
//33 - on/off
//32, 31 - channel (red green or blue)
//30 - sign
void make_rgb(int y, int x)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
        {
            if (!i && !j)
                continue;
            if (grid[aabs(y+i, HEIGHT)][aabs(x+j, WIDTH)] >> 32)
            {
                grid[y][x] = grid[aabs(y+i, HEIGHT)][aabs(x+j, WIDTH)] & (0b11 << 29);
                return ;
            }
        }

    int ch = rand() % 3;
    grid[y][x] = ((__uint64_t)ch << 30);

    if (rand() % 2)
        grid[y][x] |= (1ULL << 29);
}

void addrgb(int y, int x)
{
    if (!(grid[y][x] >> 32))
        make_rgb(y, x);

    int ch = (grid[y][x] >> 30) & 3;
    int shift = ch * 8;

    int dir = (grid[y][x] & (1ULL << 29)) ? -1 : 1;

    int val = (grid[y][x] >> shift) & 0xFF;

    val += dir;

    if (val <= 0 || val >= 255)
    {
        grid[y][x] ^= (1ULL << 29);
        val = (val <= 0) ? 0 : 255;
    }

    grid[y][x] &= ~(0xFFULL << shift);

    grid[y][x] |= ((__uint64_t)val << shift);

    grid[y][x] ^= 1;
}

int main()
{
    int y = HEIGHT / 2;
    int x = WIDTH / 2;
    e_dir dir = UP;

    //for (int i = 0; i < 110 * 100; i++)
    while(1)
    {
        addrgb(y, x);
        dir = aabs(dir + ((grid[y][x]&1)?1:-1), 4);

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
        rgb(grid[y][x] / 2);
        usleep(1000);
		printf("#");
		// b_color = (b_color + 1) % 0xFFFFFF;
    }
}
