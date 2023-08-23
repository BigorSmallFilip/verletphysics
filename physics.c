#include "physics.h"

#include "system.h"
#include "scene.h"
#include "editor.h"
#include <math.h>



static void reset_partitions()
{
	memset(g_scene_partitions, 0, g_scene_partition_count * sizeof(Partition));
}

static void update_ball_movement(
	Ball* ball,
	const float dt
)
{
	ball->velx += ball->newx * g_physics_substeps / dt;
	ball->vely += ball->newy * g_physics_substeps / dt;
	ball->vely += (gravity / g_physics_substeps) * dt;

	float max_speed = (ball->radius * g_physics_substeps) / dt;
	if (ball->velx > max_speed)       ball->velx = max_speed;
	else if (ball->velx < -max_speed) ball->velx = -max_speed;
	if (ball->vely > max_speed)       ball->vely = max_speed;
	else if (ball->vely < -max_speed) ball->vely = -max_speed;

	ball->newx = 0;
	ball->newy = 0;
	ball->x += (ball->velx / g_physics_substeps) * dt;
	ball->y += (ball->vely / g_physics_substeps) * dt;
	ball->collisioncount = 1; // Reset collision count to 1. Since it will divide by this, don't set to 0!

	if (solid_edges)
	{
		// Collide with the edges of the screen
		if (ball->x - ball->radius < 0)
		{
			ball->x = ball->radius;
			ball->velx = fabsf(ball->velx) * wall_bounce;
		}
		if (ball->x + ball->radius > screen_width)
		{
			ball->x = screen_width - ball->radius;
			ball->velx = fabsf(ball->velx) * -wall_bounce;
		}
		if (ball->y - ball->radius < 0)
		{
			ball->y = ball->radius;
			ball->vely = fabsf(ball->vely) * wall_bounce;
		}
		if (ball->y + ball->radius > screen_height)
		{
			ball->y = screen_height - ball->radius;
			ball->vely = fabsf(ball->vely) * -wall_bounce;
		}
	} else
	{
		// Remove balls that move out of bounds
		if (ball->x + ball->radius < 0 || ball->x - ball->radius > screen_width ||
			ball->y + ball->radius < 0 || ball->y - ball->radius > screen_height)
		{
			//sceneBalls.splice(i, 1);
			//bl--;
		}
	}

	Partition* partition = GetPartitionAtPosition(ball->x, ball->y);
	if (partition && partition->count < MAX_BALLS_IN_PARTITION)
	{
		partition->balls[partition->count++] = ball;
	}
}

static void update_balls_movement(const float dt)
{
	for (int i = 0; i < g_scene_balls.count; i++)
	{
		Ball* ball = g_scene_balls.items[i];
		update_ball_movement(ball, g_physics_substeps, dt);
	}
}



static void collide_balls(
	Ball* ball_a,
	Ball* ball_b
)
{
	float dx = ball_a->x - ball_b->x;
	float dy = ball_a->y - ball_b->y;
	float distsq = dx * dx + dy * dy;
	float radius = ball_a->radius + ball_b->radius;
	if (distsq < radius * radius)
	{
		float dist = sqrtf(distsq);
		dx /= dist;
		dy /= dist;
		dist = radius - dist;
		float pa = dist * (ball_b->radius / radius);
		float pb = dist * (ball_a->radius / radius);
		ball_a->newx += dx * pa;
		ball_a->newy += dy * pa;
		ball_a->collisioncount++;
		ball_b->newx -= dx * pb;
		ball_b->newy -= dy * pb;
		ball_b->collisioncount++;
	}
}

static void update_collision_naive()
{
	for (int a = 0; a < g_scene_balls.count; a++)
	{
		Ball* ball_a = g_scene_balls.items[a];
		for (int b = a + 1; b < g_scene_balls.count; b++)
		{
			Ball* ball_b = g_scene_balls.items[b];
			collide_balls(ball_a, ball_b);
		}
	}
}



static void collide_balls_separate_partitions(
	Partition* partition_a,
	Partition* partition_b
)
{
	if (partition_b->count == 0) return;
	for (int a = 0; a < partition_a->count; a++)
	{
		Ball* ball_a = partition_a->balls[a];
		for (int b = 0; b < partition_b->count; b++)
		{
			Ball* ball_b = partition_b->balls[b];
			collide_balls(ball_a, ball_b);
		}
	}
}

static void collide_balls_within_partition(
	Partition* partition
)
{
	for (int a = 0; a < partition->count; a++)
	{
		Ball* ball_a = partition->balls[a];
		for (int b = a + 1; b < partition->count; b++)
		{
			Ball* ball_b = partition->balls[b];
			collide_balls(ball_a, ball_b);
		}
	}
}

static void update_collision_space_partitioned()
{
	for (int y = 0; y < g_scene_partition_county; y++)
	{
		Bool on_bottom = (y == g_scene_partition_county - 1);
		for (int x = 0; x < g_scene_partition_countx; x++)
		{
			Partition* partition_a = &g_scene_partitions[x + (y * g_scene_partition_countx)];
			if (partition_a->count == 0) continue;
			Bool on_right = (x == g_scene_partition_countx - 1);

			collide_balls_within_partition(partition_a);
			if (!on_bottom)
			{
				if (x > 0) collide_balls_separate_partitions(
					partition_a,
					&g_scene_partitions[x - 1 + ((y + 1) * g_scene_partition_countx)]
				);
				collide_balls_separate_partitions(
					partition_a,
					&g_scene_partitions[x + ((y + 1) * g_scene_partition_countx)]
				);
				if (!on_right)
				{
					collide_balls_separate_partitions(
						partition_a,
						&g_scene_partitions[x + 1 + (y * g_scene_partition_countx)]
					);
					collide_balls_separate_partitions(
						partition_a,
						&g_scene_partitions[x + 1 + ((y + 1) * g_scene_partition_countx)]
					);
				}
			} else
			{
				if (!on_right)
					collide_balls_separate_partitions(
						partition_a,
						&g_scene_partitions[x + 1 + (y * g_scene_partition_countx)]
					);
			}
		}
	}
}



static void collide_line_ball(
	const Line* line,
	Ball* ball
)
{
	ball->radius += line->width;
	float d = 0;
	if (d < ball->radius)
	{
		float a = line->inverse_lengthsquared * (line->magic_number - ball->x * line->dx - ball->y * line->dy);
		if (a > 1) a = 1.0;
		else if (a < 0) a = 0.0;
		float dx = -a * line->dx + line->x2 - ball->x;
		float dy = -a * line->dy + line->y2 - ball->y;
		d = dx * dx + dy * dy;
		if (d < ball->radius * ball->radius)
		{
			d = sqrtf(d);
			dx /= d;
			dy /= d;
			dx *= ball->radius - d;
			dy *= ball->radius - d;
			ball->newx -= dx;
			ball->newy -= dy;
		}
	}
	ball->radius -= line->width;
}

static void collide_line_partition(
	const Line* line,
	const Partition* partition
)
{
	for (int i = 0; i < partition->count; i++)
	{
		collide_line_ball(line, partition->balls[i]);
	}
}

static void collide_line_balls(
	const Line* line
)
{
	for (int i = 0; i < line->num_partitions; i++)
	{
		collide_line_partition(line, line->partitions[i]);
	}
}

static void collide_lines_balls_space_partitioned()
{
	for (int i = 0; i < g_scene_lines.count; i++)
	{
		Line* line = g_scene_lines.items[i];
		collide_line_balls(line);
	}
}



static void substep_move_point(
	float* x,
	float* y,
	const float x2,
	const float y2,
	const int substep_index
)
{
	const float c1 = (((float)substep_index + 1.0f - (float)g_physics_substeps) / ((float)substep_index - (float)g_physics_substeps));
	const float c2 = 1.0f / ((float)g_physics_substeps - (float)substep_index);
	*x = (*x * c1) + (x2 * c2);
	*y = (*y * c1) + (y2 * c2);
}

static void substep_move_line(
	Line* line,
	const float x1,
	const float y1,
	const float x2,
	const float y2,
	const int substep_index
)
{
	float p1x = line->x1;
	float p1y = line->y1;
	substep_move_point(&p1x, &p1y, x1, y1, substep_index);
	float p2x = line->x2;
	float p2y = line->y2;
	substep_move_point(&p2x, &p2y, x2, y2, substep_index);
	line->x1 = p1x;
	line->y1 = p1y;
	line->x2 = p2x;
	line->y2 = p2y;
	UpdateLine(line);
}



static void update_balls_collision_response()
{
	for (int i = 0; i < g_scene_balls.count; i++)
	{
		Ball* ball = g_scene_balls.items[i];
		ball->x += ball->newx / ball->collisioncount;
		ball->y += ball->newy / ball->collisioncount;
	}
}



float gravity = 0.004f;
Bool solid_edges = TRUE;
float wall_bounce = 0.5;

size_t g_physics_substeps = 8;

void UpdatePhysics(float dt)
{
	if (dt == 0.0f) dt = 0.001f;
	for (int i = 0; i < g_physics_substeps; i++)
	{
		if (g_move_lineid >= 0)
		{
			Line* line = g_scene_lines.items[g_move_lineid];
			substep_move_line(line, g_move_linex1, g_move_liney1, g_move_linex2, g_move_liney2, i);
		}

		reset_partitions();
		update_balls_movement(dt / g_physics_substeps);
		update_collision_space_partitioned();
		collide_lines_balls_space_partitioned();
		update_balls_collision_response();
	}
}
