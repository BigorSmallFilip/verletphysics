#include "render.h"

#include "scene.h"
#include <math.h>

#include <SDL_opengl.h>



static void render_space_partition_grid()
{
	SetColorI(0xffff0000);
	for (int x = 1; x < g_scene_partition_countx; x++)
	{
		DrawLine(x * g_scene_partition_width, 0, x * g_scene_partition_width, screen_height);
	}
	for (int y = 1; y < g_scene_partition_county; y++)
	{
		DrawLine(0, y * g_scene_partition_height, screen_width, y * g_scene_partition_height);
	}
}



#define NUM_CIRCLE_TRIANGLES 30

SDL_Vertex circle_vertecies[NUM_CIRCLE_TRIANGLES * 3] = { 0 };

static void init_circle_vertecies()
{
	for (int i = 0; i < NUM_CIRCLE_TRIANGLES; i++)
	{
		circle_vertecies[(i * 3)].position.x = 0;
		circle_vertecies[(i * 3)].position.y = 0;
		circle_vertecies[(i * 3)].color.r = 255;
		circle_vertecies[(i * 3)].color.g = 255;
		circle_vertecies[(i * 3)].color.b = 255;
		circle_vertecies[(i * 3)].color.a = 255;
		const float angle1 = ((float)i / (float)NUM_CIRCLE_TRIANGLES) * M_PI * 2;
		circle_vertecies[(i * 3) + 1].position.x = cosf(angle1) * 100;
		circle_vertecies[(i * 3) + 1].position.y = sinf(angle1) * 100;
		circle_vertecies[(i * 3) + 1].color.r = 255;
		circle_vertecies[(i * 3) + 1].color.g = 255;
		circle_vertecies[(i * 3) + 1].color.b = 255;
		circle_vertecies[(i * 3) + 1].color.a = 255;
		const float angle2 = ((float)(i + 1) / (float)NUM_CIRCLE_TRIANGLES) * M_PI * 2;
		circle_vertecies[(i * 3) + 2].position.x = cosf(angle2) * 100;
		circle_vertecies[(i * 3) + 2].position.y = sinf(angle2) * 100;
		circle_vertecies[(i * 3) + 2].color.r = 255;
		circle_vertecies[(i * 3) + 2].color.g = 255;
		circle_vertecies[(i * 3) + 2].color.b = 255;
		circle_vertecies[(i * 3) + 2].color.a = 255;

	}
}

static void render_ball(
	const float x,
	const float y,
	const float radius,
	const uint32_t color
)
{
	/*SDL_RenderGeometry(
		main_renderer,
		NULL,
		circle_vertecies,
		NUM_CIRCLE_TRIANGLES * 3,
		NULL,
		0
	);*/
	
	SDL_SetRenderDrawColor(main_renderer, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, (color >> 24) & 0xff);
	SDL_FRect rect = { x - radius, y - radius, radius * 2, radius * 2 };
	SDL_RenderFillRectF(main_renderer, &rect);
}

static void render_lines()
{
	SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
	for (int i = 0; i < g_scene_lines.count; i++)
	{
		Line* line = g_scene_lines.items[i];
		SDL_RenderDrawLineF(
			main_renderer,
			line->x1,
			line->y1,
			line->x2,
			line->y2
		);
	}
}



Bool InitRender()
{
	init_circle_vertecies();
	return TRUE;
}

void RenderScene()
{
	//render_space_partition_grid();

	for (int i = 0; i < g_scene_balls.count; i++)
	{
		Ball* ball = g_scene_balls.items[i];
		render_ball(ball->x, ball->y, ball->radius, ball->color);
	}

	render_lines();
}
