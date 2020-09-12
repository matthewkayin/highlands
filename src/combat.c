#include "combat.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

// Camera constants
static const int CAMERA_EDGE_SCROLL_GAP = 5;
static const int CAMERA_MAX_SPEED = 40;
static const int CAMERA_ACCELERATION_SPEED = 7;

// Unit constants
static const int UNIT_SPEED = 3;

// Map variables
static int map_tile_width;
static int map_tile_height;
static int map_width;
static int map_height;

// Camera variables
static int_vector camera_position;
static int_vector camera_speed;
static int_vector camera_direction;

// Unit variables
static unit** player_units;
static int player_units_capacity;
static int player_units_size;

void init_combat(){

    map_tile_width = 40;
    map_tile_height = 40;
    map_width = map_tile_width * MAP_TILE_SIZE;
    map_height = map_tile_height * MAP_TILE_SIZE;

    camera_position = ZERO_INT_VECTOR;
    camera_speed = ZERO_INT_VECTOR;
    camera_direction = ZERO_INT_VECTOR;

    player_units_capacity = 20;
    player_units_size = 0;
    player_units = (unit**)malloc(sizeof(unit*) * player_units_capacity);
    for(int index = 0; index < player_units_capacity; index++){

        player_units[index] = NULL;
    }

    add_unit(100, 100);
}

void deinit_combat(){

    for(int index = 0; index < player_units_size; index++){

        free_unit(index);
    }
    free(player_units);
}

int get_map_tile_width(){

    return map_tile_width;
}

int get_map_tile_height(){

    return map_tile_height;
}

int_vector get_camera_position(){

    return camera_position;
}

int get_player_units_size(){

    return player_units_size;
}

bool is_unit_on_screen(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for is_unit_on_screen() at index %i\n", index);
        return false;
    }

    vector unit_offset = vector_difference(player_units[index]->position, as_float_vector(camera_position));
    return is_rect_collision(rect_from_vect(unit_offset, 32, 32), rect_from_vect(ZERO_VECTOR, SCREEN_WIDTH, SCREEN_HEIGHT));
}

int_vector get_player_unit_position(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for get_player_unit_position() at index %i\n", index);
        return as_int_vector(ZERO_VECTOR);
    }

    return as_int_vector(player_units[index]->position);
}

void add_unit(int start_x, int start_y){

    player_units[player_units_size] = (unit*)malloc(sizeof(unit));
    *(player_units[player_units_size]) = (unit){

        .position = new_vector((double)start_x, (double)start_y),
        .waypoint = NULL
    };
    player_units_size++;
}

void free_unit(int index){

    while(!vectors_equal(unit_waypoint_pop(index), NULL_VECTOR)){ }
    free(player_units[index]);
}

void unit_waypoint_add(int index, int target_x, int target_y){

    vector new_waypoint = new_vector(target_x, target_y);
    if(player_units[index]->waypoint == NULL){

        player_units[index]->waypoint = (unit_waypoint*)malloc(sizeof(unit_waypoint));
        player_units[index]->waypoint->position = new_waypoint;
        player_units[index]->waypoint->next = NULL;

    }else{

        unit_waypoint* current = player_units[index]->waypoint;
        while(current->next != NULL){

            current = current->next;
        }

        current->next = (unit_waypoint*)malloc(sizeof(unit_waypoint));
        current->next->position = new_waypoint;
        current->next->next = NULL;
    }
}

vector unit_waypoint_peek(int index){

    if(player_units[index]->waypoint == NULL){

        return NULL_VECTOR;
    }

    return player_units[index]->waypoint->position;
}

vector unit_waypoint_pop(int index){

    if(player_units[index]->waypoint == NULL){

        return NULL_VECTOR;
    }

    vector position = player_units[index]->waypoint->position;

    unit_waypoint* current = player_units[index]->waypoint;
    player_units[index]->waypoint = player_units[index]->waypoint->next;
    free(current);

    return position;
}

void update_combat(int delta){

    update_camera(delta);
    update_player_units(delta);
}

void update_camera(int delta){

    // Get camera acceleration
    int_vector target_camera_speed = int_vector_mult_scaler(camera_direction, CAMERA_MAX_SPEED);
    int_vector camera_acceleration = ZERO_INT_VECTOR;
    if(camera_speed.x != target_camera_speed.x){

        bool should_accelerate_negatively = target_camera_speed.x < camera_speed.x;
        camera_acceleration.x = CAMERA_ACCELERATION_SPEED;
        if(should_accelerate_negatively){

            camera_acceleration.x *= -1;
        }
    }
    if(camera_speed.y != target_camera_speed.y){

        bool should_accelerate_negatively = target_camera_speed.y < camera_speed.y;
        camera_acceleration.y = CAMERA_ACCELERATION_SPEED;
        if(should_accelerate_negatively){

            camera_acceleration.y *= -1;
        }
    }

    // Accelerate camera
    camera_speed = int_vector_sum(camera_speed, int_vector_mult_scaler(camera_acceleration, delta));

    // Check that camera didn't accelerate past target speed
    if(camera_speed.x != target_camera_speed.x){

        bool should_accelerate_negatively = target_camera_speed.x < camera_speed.x;
        bool did_accelerate_negatively = camera_acceleration.x < 0;
        bool overshot_target_speed = (did_accelerate_negatively && !should_accelerate_negatively) || (!did_accelerate_negatively && should_accelerate_negatively);
        if(overshot_target_speed){

            camera_speed.x = target_camera_speed.x;
        }
    }
    if(camera_speed.y != target_camera_speed.y){

        bool should_accelerate_negatively = target_camera_speed.y < camera_speed.y;
        bool did_accelerate_negatively = camera_acceleration.y < 0;
        bool overshot_target_speed = (did_accelerate_negatively && !should_accelerate_negatively) || (!did_accelerate_negatively && should_accelerate_negatively);
        if(overshot_target_speed){

            camera_speed.y = target_camera_speed.y;
        }
    }

    // Move camera
    camera_position = int_vector_sum(camera_position, int_vector_mult_scaler(camera_speed, delta));

    // Check camera still in bounds
    camera_position.x = max(0, min(camera_position.x, map_width - SCREEN_WIDTH));
    camera_position.y = max(0, min(camera_position.y, map_height - SCREEN_HEIGHT));
}

void update_player_units(int delta){

    for(int index = 0; index < player_units_size; index++){

        // Update positions
        vector target_position = unit_waypoint_peek(index);
        if(!vectors_equal(target_position, NULL_VECTOR)){

            if(vector_distance(player_units[index]->position, target_position) <= delta * UNIT_SPEED){

                player_units[index]->position = target_position;
                unit_waypoint_pop(index);

            }else{

                vector position_delta = vector_scale(vector_difference(target_position, player_units[index]->position), UNIT_SPEED);
                player_units[index]->position = vector_sum(player_units[index]->position, vector_mult_scaler(position_delta, delta));
            }
        }
    }
}

void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y){

    bool mouse_on_left_edge = mouse_x <= CAMERA_EDGE_SCROLL_GAP;
    bool mouse_on_right_edge = mouse_x >= SCREEN_WIDTH - CURSOR_WIDTH - CAMERA_EDGE_SCROLL_GAP;
    bool mouse_on_top_edge = mouse_y <= CAMERA_EDGE_SCROLL_GAP;
    bool mouse_on_bottom_edge = mouse_y >= SCREEN_HEIGHT - CURSOR_HEIGHT - CAMERA_EDGE_SCROLL_GAP;

    if(mouse_on_left_edge && mouse_relative_x <= 0){

        camera_direction.x = -1;

    }else if(mouse_on_right_edge && mouse_relative_x >= 0){

        camera_direction.x = 1;

    }else{

        camera_direction.x = 0;
    }

    if(mouse_on_top_edge && mouse_relative_y <= 0){

        camera_direction.y = -1;

    }else if(mouse_on_bottom_edge && mouse_relative_y >= 0){

        camera_direction.y = 1;

    }else{

        camera_direction.y = 0;
    }
}

void input_handle_right_click(int mouse_x, int mouse_y){

    unit_waypoint_add(0, mouse_x + camera_position.x, mouse_y + camera_position.y);
}
