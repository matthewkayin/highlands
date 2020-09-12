#include "vector.h"
#include <math.h>
#include <stdio.h>

const vector ZERO_VECTOR = (vector){ .x = 0, .y = 0 };
const int_vector ZERO_INT_VECTOR = (int_vector){ .x = 0, .y = 0 };

vector new_vector(double x, double y){

    return (vector){ .x = x, .y = y };
}

vector vector_sum(vector a, vector b){

    return (vector){ .x = a.x + b.x, .y = a.y + b.y };
}

vector vector_difference(vector a, vector b){

    return (vector){ .x = a.x - b.x, .y = a.y - b.y };
}

vector vector_mult_scaler(vector a, double b){

    return (vector){ .x = a.x * b, .y = a.y * b };
}

vector vector_normalize(vector old_vector){

    double old_magnitude = sqrt(pow(old_vector.x, 2) + pow(old_vector.y, 2));
    if(old_magnitude == 0){

        return ZERO_VECTOR;
    }

    return new_vector(old_vector.x / old_magnitude, old_vector.y / old_magnitude);
}

vector vector_scale(vector old_vector, double new_magnitude){

    double old_magnitude = sqrt(pow(old_vector.x, 2) + pow(old_vector.y, 2));
    if(old_magnitude == 0){

        return ZERO_VECTOR;
    }

    double scale = new_magnitude / old_magnitude;

    return new_vector(old_vector.x * scale, old_vector.y * scale);
}

double vector_distance(vector a, vector b){

    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

bool vectors_equal(vector a, vector b){

    return a.x == b.x && a.y == b.y;
}

int_vector as_int_vector(vector a){

    return (int_vector){ .x = (int)a.x, .y = (int)a.y };
}

int_vector new_int_vector(int x, int y){

    return (int_vector){ .x = x, .y = y };
}

int_vector int_vector_sum(int_vector a, int_vector b){

    return (int_vector){ .x = a.x + b.x, .y = a.y + b.y };
}

int_vector int_vector_mult_scaler(int_vector a, int b){

    return (int_vector){ .x = a.x * b, .y = a.y * b };
}

vector as_double_vector(int_vector a){

    return (vector){ .x = (double)a.x, .y = (double)a.y };
}

rectangle rect_from_vect(vector origin, int width, int height){

    return (rectangle){ .x = (int)origin.x, .y = (int)origin.y, .width = width, .height = height };
}

bool is_rect_collision(rectangle a, rectangle b){

    return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
            a.y + a.height <= b.y || b.y + b.height <= a.y);
}

bool is_point_in_rect(vector point, rectangle rect){

    return !(point.x <= rect.x || point.x >= rect.x + rect.width ||
            point.y <= rect.y || point.y >= rect.y + rect.height);
}

double min(double a, double b){

    if(a <= b){

        return a;

    }else{

        return b;
    }
}

double max(double a, double b){

    if(a >= b){

        return a;

    }else{

        return b;
    }
}
