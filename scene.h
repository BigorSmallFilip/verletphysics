#pragma once

#include "utility.h"

typedef struct
{
	float x, y;
	float velx, vely;
	float newx, newy;
	int collisioncount;

	float radius;
	uint32_t color;
} Ball;

extern Vector g_scene_balls;

typedef struct
{
	float x1, y1;
	float x2, y2;
	float width;

	float dx, dy;
	float lengthsquared;
	float magic_number, inverse_lengthsquared;

	size_t num_partitions;
	struct Partition** partitions;
} Line;

extern Vector g_scene_lines;

void UpdateLine(
	Line* line
);

Bool AddLine(
	const float x1,
	const float y1,
	const float x2,
	const float y2,
	const float width
);

float DistToLineSq(
	const Line* line,
	const float x,
	const float y
);

void MoveLine(
	Line* line,
	const float x1,
	const float y1,
	const float x2,
	const float y2,
	const float width,
	const int physics_substeps
);



#define MAX_BALLS_IN_PARTITION 8

typedef struct
{
	int count;
	Ball* balls[MAX_BALLS_IN_PARTITION];
} Partition;

extern int g_scene_partition_width;
extern int g_scene_partition_height;
extern int g_scene_partition_countx;
extern int g_scene_partition_county;
extern int g_scene_partition_count;
extern Partition* g_scene_partitions;

Bool InitSpacePartitions(
	const int partition_width,
	const int partition_height
);

void DestroySpacePartitions();

Partition* GetPartitionAtPosition(
	const float x,
	const float y
);



Bool InitScene(
	const int numballs,
	const float minr,
	const float maxr,
	const float vel
);

void DestroyScene();
