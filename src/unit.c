#include "unit.h"
#include <stdlib.h>
#include <stdio.h>

static const int UNIT_WIDTH = 14;
static const int UNIT_HEIGHT = 22;
static const int UNIT_SPEED = 3;
static const int UNIT_MOVE_TIMEOUT = 1 * 60; // 5 seconds

static unit* units;
int unit_capacity;
int unit_size;

int* player_selection;
int player_selection_size;
int current_move_order = -1;

// INIT AND DEINIT

void init_units(){

    unit_capacity = 16;
    unit_size = 0;
    units = (unit*)malloc(sizeof(unit) * unit_capacity);

    player_selection = NULL;
    player_selection_size = 0;
}

void free_units(){

    clear_selection();
    free(units);
}

void create_unit(vector initial_position){

    if(unit_size == unit_capacity){

        unit_capacity *= 2;
        units = (unit*)realloc(units, sizeof(unit) * unit_capacity);
    }

    units[unit_size] = (unit){
        .position = initial_position,
        .waypoint = NULL_VECTOR,
        .move_order = -1
    };
    unit_size++;
}

// GETTERS

int get_units_size(){

    return unit_size;
}

int_vector get_unit_position(int index){

    if(index >= unit_size){

        printf("Unit index of %i is out of bounds\n", index);
        return ZERO_INT_VECTOR;
    }

    return as_int_vector(units[index].position);
}

rectangle get_unit_rect(int index){

    return rect_from_vect(units[index].position, UNIT_WIDTH, UNIT_HEIGHT);
}

rectangle get_unit_collider(int index){

    int off_width = 6;
    int off_height = 8;
    return rect_from_vect(vector_sum(units[index].position, new_vector(off_width, off_height)), UNIT_WIDTH - (off_width * 2), UNIT_HEIGHT - (off_height * 2));
}

bool is_unit_selected(int index){

    for(int i = 0; i < player_selection_size; i++){

        if(player_selection[i] == index){

            return true;
        }
    }

    return false;
}

// SELECTION

void clear_selection(){

    free(player_selection);
    player_selection = NULL;
    player_selection_size = 0;
}

void select_point(vector point){

    clear_selection();
    for(int i = 0; i < unit_size; i++){

        if(is_point_in_rect(point, get_unit_rect(i))){

            player_selection = (int*)malloc(sizeof(int));
            player_selection[0] = i;
            player_selection_size = 1;
            return;
        }
    }
}

void select_rect(rectangle rect){

    clear_selection();
    int new_selection_size = 0;
    for(int i = 0; i < unit_size; i++){

        if(is_rect_collision(rect, get_unit_rect(i))){

            new_selection_size++;
        }
    }

    if(new_selection_size == 0){

        return;
    }

    player_selection = (int*)malloc(sizeof(int) * new_selection_size);
    player_selection_size = 0;

    for(int i = 0; i < unit_size; i++){

        if(is_rect_collision(rect, get_unit_rect(i))){

            player_selection[player_selection_size] = i;
            player_selection_size++;
        }
    }
}

// MOVEMENT

void update_units(int delta){

    for(int i = 0; i < unit_size; i++){

        update_unit(i, delta);
    }
}

void update_unit(int index, int delta){

    if(vectors_equal(units[index].waypoint, NULL_VECTOR)){

        return;
    }

    if(vector_distance(units[index].position, units[index].waypoint) <= UNIT_SPEED * delta){

        units[index].position = units[index].waypoint;
        units[index].waypoint = NULL_VECTOR;
    }

    // float separation_weight_distance_factor = 0.8 * (vector_distance(units[index].position, units[index].waypoint) / units[index].move_distance);

    vector velocity = get_velocity(index, 2);
    velocity = vector_sum(velocity, get_alignment(index, 0.6));
    velocity = vector_sum(velocity, get_cohesion(index, 0.6));
    velocity = vector_sum(velocity, get_separation(index, 0.8));
    vector move_delta = vector_scale(velocity, UNIT_SPEED * delta);

    // Check for collisions
    vector tentative_position = vector_sum(units[index].position, move_delta);
    rectangle tentative_position_rect = rect_from_vect(tentative_position, UNIT_WIDTH, UNIT_HEIGHT);
    bool is_collision = false;
    for(int i = 0; i < unit_size; i++){

        if(i == index){

            continue;
        }

        if(is_rect_collision(tentative_position_rect, get_unit_rect(i))){

            is_collision = true;
            break;
        }
    }

    if(is_collision){

        units[index].end_move_timer += delta;
        if(units[index].end_move_timer >= UNIT_MOVE_TIMEOUT){

            units[index].waypoint = NULL_VECTOR;
            units[index].end_move_timer = 0;
        }

    }else{

        units[index].position = vector_sum(units[index].position, move_delta);
        units[index].end_move_timer = 0;
    }
}

vector get_velocity(int index, float weight){

    return vector_scale(vector_difference(units[index].waypoint, units[index].position), weight);
}

vector get_alignment(int index, float weight){

    vector alignment = ZERO_VECTOR;
    int neighbour_count = 0;

    for(int i = 0; i < unit_size; i++){

        if(i == index || units[index].move_order != units[i].move_order){

            continue;
        }

        if(vector_distance(units[index].position, units[i].position) <= 300){

            alignment = vector_sum(alignment, get_velocity(i, 1));
            neighbour_count++;
        }
    }

    if(neighbour_count == 0){

        return alignment;
    }

    alignment.x = alignment.x / neighbour_count;
    alignment.y = alignment.y / neighbour_count;
    return vector_scale(alignment, weight);
}

vector get_cohesion(int index, float weight){

    vector cohesion = ZERO_VECTOR;
    int neighbour_count = 0;

    for(int i = 0; i < unit_size; i++){

        if(i == index || units[index].move_order != units[i].move_order){

            continue;
        }

        if(vector_distance(units[index].position, units[i].position) <= 300){

            cohesion = vector_sum(cohesion, units[i].position);
            neighbour_count++;
        }
    }

    if(neighbour_count == 0){

        return cohesion;
    }

    cohesion.x = cohesion.x / neighbour_count;
    cohesion.y = cohesion.y / neighbour_count;
    return vector_scale(cohesion, weight);
}

vector get_separation(int index, float weight){

    vector separation = ZERO_VECTOR;
    int neighbour_count = 0;

    for(int i = 0; i < unit_size; i++){

        if(i == index || units[index].move_order != units[i].move_order){

            continue;
        }

        if(vector_distance(units[index].position, units[i].position) <= 32){

            separation = vector_sum(separation, vector_difference(units[i].position, units[index].position));
            neighbour_count++;
        }
    }

    if(neighbour_count == 0){

        return separation;
    }

    separation.x = (separation.x / neighbour_count) * -1;
    separation.y = (separation.y / neighbour_count) * -1;
    return vector_scale(separation, weight);
}

// COMMANDS

void command_move(vector point, int map_width, int map_height){

    for(int i = 0; i < player_selection_size; i++){

        units[player_selection[i]].waypoint = vector_difference(point, new_vector(UNIT_WIDTH / 2, UNIT_HEIGHT / 2));
        units[player_selection[i]].move_distance = vector_distance(units[player_selection[i]].position, units[player_selection[i]].waypoint);
    }
}
