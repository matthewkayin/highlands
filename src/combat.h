#ifndef COMBAT_H
#define COMBAT_H

#include "vector.h"
#include <stdbool.h>

typedef struct unit_waypoint{
    vector position;
    struct unit_waypoint* next;
} unit_waypoint;

typedef struct{
    vector position;
    unit_waypoint* waypoint;
} unit;

void init_combat();
void deinit_combat();

int get_map_tile_width();
int get_map_tile_height();
int_vector get_camera_position();
int get_player_units_size();
bool is_unit_on_screen(int index);
int_vector get_player_unit_position(int index);

void add_unit(int start_x, int start_y);
void free_unit(int index);
void unit_waypoint_add(int index, int target_x, int target_y);
vector unit_waypoint_peek(int index);
vector unit_waypoint_pop(int index);

void update_combat(int delta);
void update_camera(int delta);
void update_player_units(int delta);

void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y);
void input_handle_right_click(int mouse_x, int mouse_y);

#endif
