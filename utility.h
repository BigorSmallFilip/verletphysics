#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef bool Bool;
#define FALSE false
#define TRUE true

#define INLINE inline

int Random();
double RandomRange(const double min, const double max);

int Clampi(
	const int number,
	const int min,
	const int max
);

float Clampf(
	const float number,
	const float min,
	const float max
);

float VectorMagnitude(const float x, const float y);

void ConstrainVectorMagnitude(
	float* x,
	float* y,
	const float min,
	const float max
);

typedef struct
{
	void** items;
	int count;
	int capacity;
} Vector;

Bool InitVector(Vector* vector, const int capacity);
Bool ResizeVector(Vector* vector, const int capacity);
Bool PushBackVector(Vector* vector, const void* item);
