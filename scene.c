#include "scene.h"

#include "system.h"
#include "editor.h"



Vector g_scene_balls = { 0 };



Vector g_scene_lines = { 0 };

void UpdateLine(
	Line* line
)
{
	line->dx = line->x2 - line->x1;
	line->dy = line->y2 - line->y1;
	line->magic_number = ((line->x2 * line->x2) + (line->y2 * line->y2)) - ((line->x2 * line->x1) + (line->y2 * line->y1));
	line->lengthsquared = line->dx * line->dx + line->dy * line->dy;
	if (line->lengthsquared == 0) line->lengthsquared = 0.001f;
	line->inverse_lengthsquared = 1 / line->lengthsquared;

	// Looks messy but is fast since min and max do this anyway
	// Gets a bounding rect of partitions
	int minpx = 0;
	int maxpx = 0;
	int minpy = 0;
	int maxpy = 0;
	if (line->x1 < line->x2)
	{
		minpx = line->x1;
		maxpx = line->x2;
	} else
	{
		minpx = line->x2;
		maxpx = line->x1;
	}
	if (line->y1 < line->y2)
	{
		minpy = line->y1;
		maxpy = line->y2;
	} else
	{
		minpy = line->y2;
		maxpy = line->y1;
	}
	minpx = Clampi(floorf((minpx - line->width) / g_scene_partition_width) - 1, 0, g_scene_partition_countx);
	maxpx = Clampi(ceilf((maxpx + line->width) / g_scene_partition_width) + 1, 0, g_scene_partition_countx);
	minpy = Clampi(floorf((minpy - line->width) / g_scene_partition_height) - 1, 0, g_scene_partition_county);
	maxpy = Clampi(ceilf((maxpy + line->width) / g_scene_partition_height) + 1, 0, g_scene_partition_county);
	int partition_countx = maxpx - minpx;
	int partition_county = maxpy - minpy;
	size_t max_partition_count = (size_t)(partition_countx * partition_county);
	free(line->partitions);

	line->num_partitions = 0;
	line->partitions = malloc(max_partition_count * sizeof(Partition*));
	float widthSq = line->width + max(g_scene_partition_width, g_scene_partition_height) * 1.25f; // 1.25 seems to work for some reason
	for (int y = minpy; y < maxpy; y++)
	{
		for (int x = minpx; x < maxpx; x++)
		{
			// Go through the bounding rect and find all partitions that are close enough to the line
			if (DistToLineSq(line,
				x * g_scene_partition_width + g_scene_partition_width / 2,
				y * g_scene_partition_height + g_scene_partition_height / 2) < widthSq * widthSq)
			{
				line->partitions[line->num_partitions] = &g_scene_partitions[x + y * g_scene_partition_countx];
				line->num_partitions++;
			}
		}
	}
	line->partitions = realloc(line->partitions, line->num_partitions * sizeof(Partition*));
}

Bool AddLine(
	const float x1,
	const float y1,
	const float x2,
	const float y2,
	const float width
)
{
	Line* line = malloc(sizeof(Line));
	if (!line) return FALSE;
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->width = width;
	line->partitions = NULL;
	UpdateLine(line);
	PushBackVector(&g_scene_lines, line);
	return TRUE;
}

float DistToLineSq(
	const Line* line,
	const float x,
	const float y
)
{
	float a = line->inverse_lengthsquared * (line->magic_number - x * line->dx - y * line->dy);
	if (a > 1) a = 1.0;
	if (a < 0) a = 0.0;
	float dx = -a * line->dx + line->x2 - x;
	float dy = -a * line->dy + line->y2 - y;
	return dx * dx + dy * dy;
}

void MoveLine(
	Line* line,
	const float x1,
	const float y1,
	const float x2,
	const float y2,
	const float width,
	const int substeps
)
{
	float difx1 = x1 - line->x1;
	float dify1 = y1 - line->y1;
	ConstrainVectorMagnitude(&difx1, &dify1, 0, substeps * line->width);
	float difx2 = x2 - line->x2;
	float dify2 = y2 - line->y2;
	ConstrainVectorMagnitude(&difx2, &dify2, 0, substeps * line->width);

	g_move_linex1 = line->x1 + difx1;
	g_move_liney1 = line->y1 + dify1;
	g_move_linex2 = line->x2 + difx2;
	g_move_liney2 = line->y2 + dify2;
}



int g_scene_partition_width = 0;
int g_scene_partition_height = 0;
int g_scene_partition_countx = 0;
int g_scene_partition_county = 0;
int g_scene_partition_count = 0;
Partition* g_scene_partitions = NULL;

Bool InitSpacePartitions(const int partition_width, const int partition_height)
{
	DestroySpacePartitions();
	g_scene_partition_width = partition_width;
	g_scene_partition_height = partition_height;
	g_scene_partition_countx = (int)ceilf((float)screen_width / (float)partition_width);
	g_scene_partition_county = (int)ceilf((float)screen_height / (float)partition_height);
	g_scene_partition_count = g_scene_partition_countx * g_scene_partition_county;

	g_scene_partitions = calloc(g_scene_partition_count, sizeof(Partition));
	if (!g_scene_partitions) return FALSE;
}

void DestroySpacePartitions()
{

}

Partition* GetPartitionAtPosition(const float x, const float y)
{
	int px = (int)floorf(x / (float)g_scene_partition_width);
	if (px < 0 || px >= g_scene_partition_countx) return NULL;
	int py = (int)floorf(y / (float)g_scene_partition_height);
	if (py < 0 || py >= g_scene_partition_county) return NULL;
	return &g_scene_partitions[px + (py * g_scene_partition_countx)];
}



Bool InitScene(
	const int numballs,
	const float minr,
	const float maxr,
	const float vel
)
{
	assert(g_scene_balls.capacity == 0);

	InitVector(&g_scene_balls, numballs);
	InitVector(&g_scene_lines, 4);

	for (int i = 0; i < numballs; i++)
	{
		Ball* ball = malloc(sizeof(Ball));
		if (!ball) goto on_error;
		ball->x = RandomRange(0, screen_width);
		ball->y = RandomRange(0, screen_height);
		ball->velx = RandomRange(-vel, vel);
		ball->vely = RandomRange(-vel, vel);
		ball->newx = 0;
		ball->newy = 0;
		ball->radius = RandomRange(minr, maxr);
		ball->color = (uint32_t)((Random() & 255) | ((Random() & 255) << 8) | ((Random() & 255) << 16));
		PushBackVector(&g_scene_balls, ball);
	}

	InitSpacePartitions(ceilf(maxr) * 2, ceilf(maxr) * 2);

on_error:
	DestroyScene();
	return FALSE;
}

void DestroyScene()
{
}
