#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "vector.h"
#include <stdbool.h>

typedef struct{
    vector position;
    float_vector speed;
    vector target_position;
} unit;

typedef struct{
    vector camera_position;
    vector camera_speed;
    vector camera_direction;
} Encounter;

void init_encounter();
void deinit_encounter();

vector get_camera_position();
int get_player_units_size();
bool is_unit_on_screen(int index);
vector get_player_unit_position(int index);

void add_unit(int start_x, int start_y);
void direct_unit(int index, int target_x, int target_y);

void update_encounter(float delta);
void update_camera(float delta);
void update_player_units(float delta);

void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y);
void input_handle_right_click(int mouse_x, int mouse_y);

int min(int a, int b);
int max(int a, int b);

#endif
