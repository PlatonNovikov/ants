int HEIGHT;
int WIDTH;
int H;
int W;
int CELL_SIZE = 1;

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#define rgb(clr) (Color){(clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff, 255}

uint32_t *grid;
#define GRID(y,x) grid[(y)*W + (x)]

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
	srand(seed + (GRID(y, x) >> 29));
	int clrbyte = (1 << ((25 + rand() % 3)));
	GRID(y, x) += 1 << 29;
	if ((GRID(y, x) >> 29) == 0b111)
		GRID(y, x) ^= 0b111 << 29;
	GRID(y, x) = (GRID(y, x) & 0b11100001111111111111111111111111ULL) + clrbyte;
	switch ((clrbyte >> 25) & 0b111)
	{
	case 0b100:
		if (((GRID(y, x) >> 16) & 0xff) == 0xff)
			GRID(y, x) |= 1 << 28;
		break;

	case 0b010:
		if (((GRID(y, x) >> 8) & 0xff) == 0xff)
			GRID(y, x) |= 1 << 28;
		break;

	case 0b001:
		if ((GRID(y, x) & 0xff) == 0xff)
			GRID(y, x) |= 1 << 28;
		break;

	default:
		break;
	}
}

void add_rgb(int y, int x)
{
	int dir = (GRID(y, x) >> 28) & 1 ? -1 : 1;
	if (!((GRID(y, x) >> 25) & 0b111))
		set_rgb(y, x);

	switch (GRID(y, x) >> 25 & 0b111)
	{
	case 0b100:
		GRID(y, x) += 0x010000 * dir;
		if ((dir == -1) && (((GRID(y, x) >> 16) & 0xff) == 0))
			set_rgb(y, x);
		else if (((GRID(y, x) >> 16) & 0xff) == 0xff)
			set_rgb(y, x);
		return ;
		break;

	case 0b010:
		GRID(y, x) += 0x0100 * dir;
		if ((dir == -1) && (((GRID(y, x) >> 8) & 0xff) == 0))
			set_rgb(y, x);
		else if (((GRID(y, x) >> 8) & 0xff) == 0xff)
			set_rgb(y, x);
		return ;
		break;

	case 0b001:
		GRID(y, x) += 0x01 * dir;
		if ((dir == -1) && ((GRID(y, x) & 0xff) == 0))
			set_rgb(y, x);
		else if ((GRID(y, x) & 0xff) == 0xff)
			set_rgb(y, x);
		return ;
		break;

	default:
		break;
	}
}

void random_grid()
{
	for (int x = 0; x < WIDTH / CELL_SIZE; x++)
		for (int y = 0; y < HEIGHT / CELL_SIZE; y++)
			GRID(y, x) = (rand() & 1) << 24;
}

void usage()
{
	printf("ant_raylib -help -no-fullscreen -seed [seed] -size [width] [height]\n");
}

void invalid_args()
{
	printf("Invalid args\n");
	usage();
	exit(EXIT_FAILURE);
}

void start_args(int argc, char *argv[])
{
	bool arg_size = false;
	bool fullscreen = true;
	bool arg_seed = false;
	bool arg_cell = false;

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp("-size", argv[i]))
		{
			if (i+2 >= argc)
				invalid_args();
			arg_size = true;
			WIDTH = atoi(argv[i+1]);
			HEIGHT = atoi(argv[i+2]);
			i += 2;
			continue;
		}

		if (!strcmp("-help", argv[i]))
		{
			usage();
			exit(EXIT_SUCCESS);
		}

		if (!strcmp("-no-fullscreen", argv[i]))
		{
			fullscreen = false;
			continue;
		}

		if (!strcmp("-seed", argv[i]))
		{
			if (i+1 >= argc)
				invalid_args();
			seed = atoi(argv[i+1]);
			arg_seed = true;
			srand(seed);
			i++;
			continue;
		}

		// if (!strcmp("-cell", argv[i]))
		// {
		// 	if (i+1 >= argc)
		// 		invalid_args();
		// 	CELL_SIZE = atoi(argv[i+1]);
		// 	arg_cell = true;
		// 	i++;
		// 	continue;
		// }

		invalid_args();
	}

	if (!arg_size)
	{
		InitWindow(1, 1, "ant :)");
		int monitor = GetCurrentMonitor();
		WIDTH = GetMonitorWidth(monitor);
		HEIGHT = GetMonitorHeight(monitor);
		CloseWindow();
	}
	if (!arg_seed)
	{
		seed = time(NULL);
		printf("seed: %d\n", seed);
	}
	// if (!arg_cell)
	// {
		CELL_SIZE = 1;
	// }

	InitWindow(WIDTH, HEIGHT, "ant :)");
	if (fullscreen)
		ToggleFullscreen();
}

int main(int argc, char *argv[])
{
	SetTraceLogLevel(LOG_ERROR);
	start_args(argc, argv);

	W = WIDTH / CELL_SIZE;
	H = HEIGHT / CELL_SIZE;
	grid = calloc(H * W, sizeof(uint32_t));
	int y = W / 2;
	int x = H / 2;
	e_dir dir = UP;

	Image img = GenImageColor(WIDTH, HEIGHT, BLACK);
	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);

	Color *pixels = calloc(W * H, sizeof(Color));

	random_grid();
	while(!WindowShouldClose())
	{
		for (int a = 0; a < 500000; a++)
		{
			int dx = x / CELL_SIZE;
			int dy = y / CELL_SIZE;
			int cell = GRID(dy, dx);
			add_rgb(dy, dx);
			GRID(dy, dx) = GRID(dy, dx) ^ (1 << 24);
			dir = aabs(dir + (((GRID(dy, dx) >> 24)&1)?1:-1), 4);

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
			pixels[dy * W + dx] = rgb(GRID(dy, dx));
		}

		UpdateTexture(tex, pixels);
		BeginDrawing();
		ClearBackground(BLACK);
		DrawTexture(tex, 0, 0, WHITE);
		EndDrawing();
	}
	CloseWindow();
}
