grid[100][200] = {0};

dir, x, y, i;

mod(a, b)
{
    return ((a % b + b) % b);
}

void draw_grid()
{
    y = y << 16;
    x = x << 16;
    for (; (y & 255) < 100; y++)
    {
        for (; (x & 255) < 200; x++)
            putchar(" #"[grid[(y & 255)][(x & 255)]]);
        printf("\n");
    }
    y = y >> 16;
    x = x >> 16;
}

int main()
{
    y = 49;
    x = 99;
    dir = 0;

    for (i = 0; i < 110 * 100; i++)
    {
        draw_grid();

        grid[y][x] = !grid[y][x];
        dir = mod(dir - grid[y][x] * 2 - 1, 4);

        y = mod(y + (dir == 2) - (dir == 0), 100);
        x = mod(x + (dir == 1) - (dir == 3), 200);

        usleep(20000);
        printf("\033[H\033[J");
    }
}
