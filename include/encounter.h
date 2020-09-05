#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "global.h"

#define CAMERA_SPEED 100
#define MAP_WIDTH 2560
#define MAP_HEIGHT 1440

typedef struct{
    int camera_x;
    int camera_y;
} Encounter;

Encounter init_encounter();
Encounter update_encounter(Encounter previous_state, double delta, int mouse_x, int mouse_y);

#endif
