#include "combat.h"
#include "quicksort.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

// Camera constants
static const int CAMERA_EDGE_SCROLL_GAP = 5;
static const int CAMERA_MAX_SPEED = 40;
static const int CAMERA_ACCELERATION_SPEED = 7;

// Map variables
static int map_tile_width;
static int map_tile_height;
static int map_width;
static int map_height;

// Camera variables
static int_vector camera_position;
static int_vector camera_speed;
static int_vector camera_direction;

// INIT AND DEINIT STATE FUNCTIONS

void init_combat(){

    map_tile_width = 40;
    map_tile_height = 40;
    map_width = map_tile_width * MAP_TILE_SIZE;
    map_height = map_tile_height * MAP_TILE_SIZE;

    camera_position = ZERO_INT_VECTOR;
    camera_speed = ZERO_INT_VECTOR;
    camera_direction = ZERO_INT_VECTOR;

    init_units();
    create_unit(new_vector(100, 100));
    create_unit(new_vector(25, 25));
    create_unit(new_vector(150, 50));
    create_unit(new_vector(75, 150));
}

void deinit_combat(){

    free_units();
}

// GETTERS

int get_map_tile_width(){

    return map_tile_width;
}

int get_map_tile_height(){

    return map_tile_height;
}

int_vector get_camera_position(){

    return camera_position;
}

// UPDATE FUNCTIONS

void update_combat(int delta){

    update_camera(delta);
    update_units(delta);
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

// INPUT FUNCTIONS

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

void input_handle_left_click(int mouse_x, int mouse_y){

    select_point(new_vector(mouse_x + camera_position.x, mouse_y + camera_position.y));
}

void input_handle_right_click(int mouse_x, int mouse_y){

    command_move(new_vector(mouse_x + camera_position.x, mouse_y + camera_position.y), map_width, map_height);
}

void input_handle_select_rect(rectangle rect){

    rectangle camera_adjusted = rect_from_vect(new_vector(rect.x + camera_position.x, rect.y + camera_position.y), rect.width, rect.height);
    select_rect(camera_adjusted);
}
