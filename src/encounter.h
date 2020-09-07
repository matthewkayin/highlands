#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "global.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    vector camera_position;
    vector camera_speed;
    vector camera_direction;
} Encounter;

vector get_camera_position();

void init_encounter();
void update_encounter(double delta);

void update_camera(double delta);

void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y);

int min(int a, int b);
int max(int a, int b);

#endif
