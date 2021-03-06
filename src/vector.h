#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct{
    float x;
    float y;
} vector;

typedef struct{
    int x;
    int y;
} int_vector;

typedef struct{
    int x;
    int y;
    int width;
    int height;
} rectangle;

extern const vector ZERO_VECTOR;
extern const vector NULL_VECTOR;
extern const int_vector ZERO_INT_VECTOR;

// Vector functions
vector new_vector(float x, float y);
vector vector_sum(vector a, vector b);
vector vector_difference(vector a, vector b);
vector vector_mult_scaler(vector a, float b);
vector vector_normalize(vector old_vector);
vector vector_scale(vector old_vector, float new_magnitude);
float vector_distance(vector a, vector b);
bool vectors_equal(vector a, vector b);
int_vector as_int_vector(vector a);

// Int vector functions
int_vector new_int_vector(int x, int y);
int_vector int_vector_sum(int_vector a, int_vector b);
int_vector int_vector_mult_scaler(int_vector a, int b);
vector as_float_vector(int_vector a);

// Rectangle functions
rectangle rect_from_vect(vector origin, int width, int height);
bool is_rect_collision(rectangle a, rectangle b);
bool is_point_in_rect(vector point, rectangle rect);

// Misc math functions
float min(float a, float b);
float max(float a, float b);

#endif
