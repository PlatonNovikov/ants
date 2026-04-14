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

__uint64_t grid[HEIGHT][WIDTH] = {0};

int b_color = 0x0;

char brg[93] = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
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


int main()
{
    int y = HEIGHT / 2;
    int x = WIDTH / 2;
    e_dir dir = UP;
    int die = 2;

    //for (int i = 0; i < 110 * 100; i++)
    while(1)
    {
        grid[y][x] = !grid[y][x];
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
        if (grid[y][x])
            rgb(b_color);
        else
            rgb(0);
		printf("%c", brg[(int)(rgb_bgtnss(b_color) * (brglen - 1))]);
		b_color = (b_color + dir) % 0xFFFFFF;
    }
}
