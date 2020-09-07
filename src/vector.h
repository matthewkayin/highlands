#ifndef VECTOR_H
#define VECTOR_H

typedef struct{
    int x;
    int y;
} vector;

extern const vector ZERO_VECTOR;

vector new_vector(int x, int y);
vector vector_sum(vector a, vector b);
vector vector_mult_int(vector a, int b);
vector vector_mult_double(vector a, double b);

#endif
