#ifndef UNIT_H
#define UNIT_H

#include "vector.h"

typedef struct{
    vector position;
    vector waypoint;
    int move_order;
    int move_distance;
    int end_move_timer;
} unit;

// Init and deinit
void init_units();
void free_units();
void create_unit(vector initial_position);

// Getters
int get_units_size();
int_vector get_unit_position(int index);
rectangle get_unit_rect(int index);
rectangle get_unit_collider(int index);
bool is_unit_selected(int index);

// Selection
void clear_selection();
void select_point(vector point);
void select_rect(rectangle rect);

// Movement
void update_units(int delta);
void update_unit(int index, int delta);
vector get_velocity(int index, float weight);
vector get_alignment(int index, float weight);
vector get_cohesion(int index, float weight);
vector get_separation(int index, float weight);

// Commands
void command_move(vector point, int map_width, int map_height);

#endif
