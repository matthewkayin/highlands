#ifndef COMBAT_H
#define COMBAT_H

#include "vector.h"
#include "unit.h"
#include <stdbool.h>

// Init and deinit state
void init_combat();
void deinit_combat();

// Getters
int get_map_tile_width();
int get_map_tile_height();
int_vector get_camera_position();

// Update functions
void update_combat(int delta);
void update_camera(int delta);

// Input functions
void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y);
void input_handle_left_click(int mouse_x, int mouse_y);
void input_handle_right_click(int mouse_x, int mouse_y);
void input_handle_select_rect(rectangle rect);

#endif
