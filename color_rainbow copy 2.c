#define HEIGHT 165
#define WIDTH 639

#include <string.h>
#include <inttypes.h>

#define clear printf("\033[H\033[J")

#define move(y, x) printf("\033[%d;%dH", (y)+1, (x)+1)
#define rgb(clr) printf("\033[38;2;%d;%d;%dm", (clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff)
#define rgb_bgtnss(clr) ( \
    0.299 * ((clr >> 16) & 0xff) + \
    0.587 * ((clr >> 8) & 0xff) + \
    0.114 * (clr & 0xff) \
) / 255.0

//bytes:
//1-24 - rgb color
//25 - on/off
//26-28 - current color
//29 - direction (0 - up, 1 - down)
//30 - 32 - rand calls
int grid[HEIGHT][WIDTH] = {0};

int seed;

char brg[93] = "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
int brglen = 93;

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
    int i, j;

    for (i = -1; i != 2; i++)
    {
        for (j = -1; j != 2; j++)
        {
            int cell = grid[aabs(y - i, HEIGHT)][aabs(x - j, WIDTH)];
            if (cell >> 25 & 0b111)
            {
                switch (cell >> 25 & 0b111)
                {
                case 0b100:
                    if ((grid[y][x] >> 16) & 0xff)
                        continue;
                    break;

                case 0b001:
                    if (grid[y][x] & 0xff)
                        continue;
                    break;

                case 0b010:
                    if ((grid[y][x] >> 8) & 0xff)
                        continue;
                    break;

                default:
                    continue;
                    break;
                }
                grid[y][x] = (grid[y][x] & 0b10001111111111111111111111111) + ((cell >> 25 & 0b111) << 25);
                return ;
            }
        }
    }

    srand(seed + (grid[y][x] >> 29));
    int clrbyte = (1 << ((25 + rand() % 3)));
    grid[y][x] += 1 << 29;
    if ((grid[y][x] >> 29) == 0b111)
        grid[y][x] ^= 0b111 << 29;
    grid[y][x] = (grid[y][x] & 0b10001111111111111111111111111) + clrbyte;
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
        if (((grid[y][x] >> 16) & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    case 0b010:
        grid[y][x] += 0x0100 * dir;
        if (((grid[y][x] >> 8) & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    case 0b001:
        grid[y][x] += 0x01 * dir;
        if ((grid[y][x] & 0xff) == 0xff)
            set_rgb(y, x);
        return ;
        break;

    default:
        break;
    }
}

int main()
{
    seed = time(NULL);

    int y = HEIGHT / 2;
    int x = WIDTH / 2;
    e_dir dir = UP;

    //for (int i = 0; i < 110 * 100; i++)
    while(1)
    {
        int cell = grid[y][x];
        add_rgb(y, x);
        grid[y][x] = grid[y][x] ^ (1 << 24);
        dir = aabs(dir + (((grid[y][x] >> 24)&1)?1:-1), 4);

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
        rgb(grid[y][x]);
		printf("#");
		// b_color = (b_color + 1) % 0xFFFFFF;
    }
}
