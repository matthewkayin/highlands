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
    int blocked_path_timer;
    unit_waypoint* waypoint;
} unit;

// Init and deinit state
void init_combat();
void deinit_combat();

// Getters
int get_map_tile_width();
int get_map_tile_height();
int_vector get_camera_position();
int get_player_units_size();
int get_player_selection_size();
bool is_unit_on_screen(int index);
bool is_unit_selected(int index);
int_vector get_player_unit_position(int index);
rectangle get_player_unit_rect(int index);

// Unit functions
void add_unit(int start_x, int start_y);
void free_unit(int index);
void unit_waypoint_add(int index, int target_x, int target_y);
vector unit_waypoint_peek(int index);
vector unit_waypoint_pop(int index);
void unit_waypoint_pop_all(int index);

// Selection functions
void player_selection_clear();
void player_selection_select_one(int target_x, int target_y);
void player_selection_select_many(rectangle selection_rect);

// Update functions
void update_combat(int delta);
void update_camera(int delta);
void update_player_units(int delta);

// Input functions
void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y);
void input_handle_left_click(int mouse_x, int mouse_y);
void input_handle_right_click(int mouse_x, int mouse_y);
void input_handle_select_rect(rectangle select_rect);

#endif
