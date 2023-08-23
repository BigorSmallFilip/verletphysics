#include "utility.h"



int Random()
{
	return rand();
}

double RandomRange(const double min, const double max)
{
	double n = (double)Random() / (double)RAND_MAX;
	n *= (max - min);
	n += min;
	return n;
}



int Clampi(const int number, const int min, const int max)
{
	if (number < min) return min;
	if (number > max) return max;
	return number;
}

float Clampf(const float number, const float min, const float max)
{
	if (number < min) return min;
	if (number > max) return max;
	return number;
}

float VectorMagnitude(const float x, const float y)
{
	return sqrtf(x * x + y * y);
}

void ConstrainVectorMagnitude(float* x, float* y, const float min, const float max)
{
	float l = VectorMagnitude(*x, *y);
	if (l == 0) return;
	*x /= l;
	*y /= l;
	l = Clampf(l, min, max);
	*x *= l;
	*y *= l;
}



Bool InitVector(Vector* vector, const int capacity)
{
	assert(vector);
	assert(capacity >= 0);
	assert(vector->items == NULL);
	assert(vector->count == 0);
	vector->count = 0;
	vector->capacity = capacity;
	vector->items = malloc(capacity * sizeof(void*));
	if (!vector->items) return FALSE;
	return TRUE;
}

Bool ResizeVector(Vector* vector, const int capacity)
{
	assert(vector);
	assert(capacity >= 0);
	vector->items = realloc(vector->items, capacity * sizeof(void*));
	if (!vector->items) return FALSE;
	vector->capacity = capacity;
	return TRUE;
}

Bool PushBackVector(Vector* vector, const void* item)
{
	assert(vector);
	assert(item);
	if (vector->count + 1 > vector->capacity)
		ResizeVector(vector, vector->capacity * 2);
	vector->items[vector->count] = item;
	vector->count++;
}
