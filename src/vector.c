#include "vector.h"

const vector ZERO_VECTOR = (vector){ .x = 0, .y = 0 };

vector new_vector(int x, int y){

    return (vector){ .x = x, .y = y };
}

vector vector_sum(vector a, vector b){

    return (vector){ .x = a.x + b.x, .y = a.y + b.y };
}

vector vector_mult_int(vector a, int b){

    return (vector){ .x = a.x * b, .y = a.y * b };
}

vector vector_mult_double(vector a, double b){

    return (vector){ .x = (int)(a.x * b), .y = (int)(a.y * b) };
}
