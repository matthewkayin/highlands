#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct{
    int x;
    int y;
} vector;

typedef struct{
    float x;
    float y;
} float_vector;

typedef struct{
    int x;
    int y;
    int width;
    int height;
} rectangle;

extern const vector ZERO_VECTOR;
extern const float_vector ZERO_FLOAT_VECTOR;

// Int vector functions
vector new_vector(int x, int y);
vector as_int_vector(float_vector a);
vector vector_sum(vector a, vector b);
vector vector_difference(vector a, vector b);
vector vector_mult_int(vector a, int b);
vector vector_mult_float(vector a, float b);
vector vector_scale(vector old_vector, float new_magnitude);
float vector_distance(vector a, vector b);

// Float vector functions
float_vector new_float_vector(float x, float y);
float_vector as_float_vector(vector a);
float_vector float_vector_sum(float_vector a, float_vector b);
float_vector float_vector_difference(float_vector a, float_vector b);
float_vector float_vector_mult_int(float_vector a, int b);
float_vector float_vector_mult_float(float_vector a, float b);
float_vector float_vector_scale(float_vector old_vector, float new_magnitude);

// Rectangle functions
rectangle rect_from_vect(vector origin, int width, int height);
bool is_rect_collision(rectangle a, rectangle b);
bool is_point_in_rect(vector point, rectangle rect);

#endif
