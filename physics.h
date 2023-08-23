#pragma once

#include "utility.h"



extern float gravity;
extern Bool solid_edges;
extern float wall_bounce;

extern size_t g_physics_substeps;

void UpdatePhysics(
	float dt
);
