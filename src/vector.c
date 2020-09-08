#include "vector.h"
#include <math.h>

const vector ZERO_VECTOR = (vector){ .x = 0, .y = 0 };
const float_vector ZERO_FLOAT_VECTOR = (float_vector){ .x = 0, .y = 0 };

vector new_vector(int x, int y){

    return (vector){ .x = x, .y = y };
}

vector as_int_vector(float_vector a){

    return (vector){ .x = (int)a.x, .y = (int)a.y };
}

vector vector_sum(vector a, vector b){

    return (vector){ .x = a.x + b.x, .y = a.y + b.y };
}

vector vector_difference(vector a, vector b){

    return (vector){ .x = a.x - b.x, .y = a.y - b.y };
}

vector vector_mult_int(vector a, int b){

    return (vector){ .x = a.x * b, .y = a.y * b };
}

vector vector_mult_float(vector a, float b){

    return (vector){ .x = (int)(a.x * b), .y = (int)(a.y * b) };
}

vector vector_scale(vector old_vector, float new_magnitude){

    float old_magnitude = sqrt(pow(old_vector.x, 2) + pow(old_vector.y, 2));
    if(old_magnitude == 0){

        return ZERO_VECTOR;
    }

    float scale = new_magnitude / old_magnitude;

    return as_int_vector(new_float_vector(old_vector.x * scale, old_vector.y * scale));
}

float vector_distance(vector a, vector b){

    return (float)sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

float_vector new_float_vector(float x, float y){

    return (float_vector){ .x = x, .y = y };
}

float_vector as_float_vector(vector a){

    return (float_vector){ .x = (float)a.x, .y = (float)a.y };
}

float_vector float_vector_sum(float_vector a, float_vector b){

    return (float_vector){ .x = a.x + b.x, .y = a.y + b.y };
}

float_vector float_vector_difference(float_vector a, float_vector b){

    return (float_vector){ .x = a.x - b.x, .y = a.y - b.y };
}

float_vector float_vector_mult_int(float_vector a, int b){

    return (float_vector){ .x = a.x * b, .y = a.y * b };
}

float_vector float_vector_mult_float(float_vector a, float b){

    return (float_vector){ .x = a.x * b, .y = a.y * b };
}

float_vector float_vector_scale(float_vector old_vector, float new_magnitude){

    float old_magnitude = sqrt(pow(old_vector.x, 2) + pow(old_vector.y, 2));
    if(old_magnitude == 0){

        return ZERO_FLOAT_VECTOR;
    }

    float scale = new_magnitude / old_magnitude;

    return new_float_vector(old_vector.x * scale, old_vector.y * scale);
}

rectangle rect_from_vect(vector origin, int width, int height){

    return (rectangle){ .x = origin.x, .y = origin.y, .width = width, .height = height };
}

bool is_rect_collision(rectangle a, rectangle b){

    return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
            a.y + a.height <= b.y || b.y + b.height <= a.y);
}

bool is_point_in_rect(vector point, rectangle rect){

    return !(point.x <= rect.x || point.x >= rect.x + rect.width ||
            point.y <= rect.y || point.y >= rect.y + rect.height);
}
