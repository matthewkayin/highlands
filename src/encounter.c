#include "encounter.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

static const int MAP_WIDTH = 2560;
static const int MAP_HEIGHT = 1440;
static const int MAP_EDGE_SCROLL_GAP = 5;

static const int CAMERA_MAX_SPEED = 150;
static const int CAMERA_ACCELERATION_SPEED = 50;

static const int UNIT_SPEED = 3;

static Encounter encounter;
static unit** player_units;
static int player_units_capacity;
static int player_units_size;

void init_encounter(){

    encounter.camera_position = ZERO_VECTOR;
    encounter.camera_speed = ZERO_VECTOR;
    encounter.camera_direction = ZERO_VECTOR;

    player_units_capacity = 20;
    player_units_size = 0;
    player_units = (unit**)malloc(sizeof(unit*) * player_units_capacity);
    for(int index = 0; index < player_units_capacity; index++){

        player_units[index] = NULL;
    }

    add_unit(100, 100);
}

void deinit_encounter(){

    for(int index = 0; index < player_units_size; index++){

        free(player_units[index]);
    }
    free(player_units);
}


vector get_camera_position(){

    return encounter.camera_position;
}

int get_player_units_size(){

    return player_units_size;
}

bool is_unit_on_screen(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for is_unit_on_screen() at index %i\n", index);
        return false;
    }

    vector unit_offset = vector_difference(player_units[index]->position, encounter.camera_position);
    return is_rect_collision(rect_from_vect(unit_offset, 32, 32), rect_from_vect(ZERO_VECTOR, SCREEN_WIDTH, SCREEN_HEIGHT));
}

vector get_player_unit_position(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for get_player_unit_position() at index %i\n", index);
        return ZERO_VECTOR;
    }

    return player_units[index]->position;
}

void add_unit(int start_x, int start_y){

    player_units[player_units_size] = (unit*)malloc(sizeof(unit));
    *(player_units[player_units_size]) = (unit){

        .position = new_vector(start_x, start_y),
        .speed = ZERO_FLOAT_VECTOR,
        .target_position = ZERO_VECTOR
    };
    player_units_size++;
}

void direct_unit(int index, int target_x, int target_y){

    player_units[index]->target_position = new_vector(target_x, target_y);
    player_units[index]->speed = float_vector_scale(as_float_vector(vector_difference(player_units[index]->target_position, player_units[index]->position)), (float)UNIT_SPEED);
}

void update_encounter(float delta){

    update_camera(delta);
    update_player_units(delta);
}

void update_camera(float delta){

    // Get camera acceleration
    vector target_camera_speed = vector_mult_int(encounter.camera_direction, CAMERA_MAX_SPEED);
    vector camera_acceleration = ZERO_VECTOR;
    if(encounter.camera_speed.x != target_camera_speed.x){

        bool should_accelerate_negatively = target_camera_speed.x < encounter.camera_speed.x;
        camera_acceleration.x = CAMERA_ACCELERATION_SPEED;
        if(should_accelerate_negatively){

            camera_acceleration.x *= -1;
        }
    }
    if(encounter.camera_speed.y != target_camera_speed.y){

        bool should_accelerate_negatively = target_camera_speed.y < encounter.camera_speed.y;
        camera_acceleration.y = CAMERA_ACCELERATION_SPEED;
        if(should_accelerate_negatively){

            camera_acceleration.y *= -1;
        }
    }

    // Accelerate camera
    encounter.camera_speed = vector_sum(encounter.camera_speed, vector_mult_float(camera_acceleration, delta));

    // Check that camera didn't accelerate past target speed
    if(encounter.camera_speed.x != target_camera_speed.x){

        bool should_accelerate_negatively = target_camera_speed.x < encounter.camera_speed.x;
        bool did_accelerate_negatively = camera_acceleration.x < 0;
        bool overshot_target_speed = did_accelerate_negatively && !should_accelerate_negatively;
        if(overshot_target_speed){

            encounter.camera_speed.x = target_camera_speed.x;
        }
    }
    if(encounter.camera_speed.y != target_camera_speed.y){

        bool should_accelerate_negatively = target_camera_speed.y < encounter.camera_speed.y;
        bool did_accelerate_negatively = camera_acceleration.y < 0;
        bool overshot_target_speed = did_accelerate_negatively && !should_accelerate_negatively;
        if(overshot_target_speed){

            encounter.camera_speed.y = target_camera_speed.y;
        }
    }

    // Move camera
    encounter.camera_position = vector_sum(encounter.camera_position, vector_mult_float(encounter.camera_speed, delta));

    // Check camera still in bounds
    encounter.camera_position.x = max(0, min(encounter.camera_position.x, MAP_WIDTH - SCREEN_WIDTH));
    encounter.camera_position.y = max(0, min(encounter.camera_position.y, MAP_HEIGHT - SCREEN_HEIGHT));
}

void update_player_units(float delta){

    for(int index = 0; index < player_units_size; index++){

        // Update positions
        player_units[index]->position = vector_sum(player_units[index]->position, as_int_vector(float_vector_mult_float(player_units[index]->speed, delta)));

        // Snap to target position if close to prevent overshooting goal
        if(vector_distance(player_units[index]->position, player_units[index]->target_position) <= (float)UNIT_SPEED){

            player_units[index]->position = player_units[index]->target_position;
            player_units[index]->speed = ZERO_FLOAT_VECTOR;
        }
    }
}

void input_update_camera(int mouse_x, int mouse_y, int mouse_relative_x, int mouse_relative_y){

    bool mouse_on_left_edge = mouse_x <= MAP_EDGE_SCROLL_GAP;
    bool mouse_on_right_edge = mouse_x >= SCREEN_WIDTH - CURSOR_WIDTH - MAP_EDGE_SCROLL_GAP;
    bool mouse_on_top_edge = mouse_y <= MAP_EDGE_SCROLL_GAP;
    bool mouse_on_bottom_edge = mouse_y >= SCREEN_HEIGHT - CURSOR_HEIGHT - MAP_EDGE_SCROLL_GAP;

    if(mouse_on_left_edge && mouse_relative_x <= 0){

        encounter.camera_direction.x = -1;

    }else if(mouse_on_right_edge && mouse_relative_x >= 0){

        encounter.camera_direction.x = 1;

    }else{

        encounter.camera_direction.x = 0;
    }

    if(mouse_on_top_edge && mouse_relative_y <= 0){

        encounter.camera_direction.y = -1;

    }else if(mouse_on_bottom_edge && mouse_relative_y >= 0){

        encounter.camera_direction.y = 1;

    }else{

        encounter.camera_direction.y = 0;
    }
}

void input_handle_right_click(int mouse_x, int mouse_y){

    direct_unit(0, mouse_x + encounter.camera_position.x, mouse_y + encounter.camera_position.y);
}

int min(int a, int b){

    if(a <= b){

        return a;

    }else{

        return b;
    }
}

int max(int a, int b){

    if(a >= b){

        return a;

    }else{

        return b;
    }
}
