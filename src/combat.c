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
static const int UNIT_BLOCKED_PATH_TIMEOUT = 25 * 15; // 15 seconds

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

// Selection variables
static int* player_selection;
static int player_selection_size;

// INIT AND DEINIT STATE FUNCTIONS

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

    player_selection_clear();

    add_unit(100, 100);
    add_unit(150, 100);
}

void deinit_combat(){

    for(int index = 0; index < player_units_size; index++){

        free_unit(index);
    }
    free(player_units);
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

int get_player_units_size(){

    return player_units_size;
}

int get_player_selection_size(){

    return player_selection_size;
}

bool is_unit_on_screen(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for is_unit_on_screen() at index %i\n", index);
        return false;
    }

    vector unit_offset = vector_difference(player_units[index]->position, as_float_vector(camera_position));
    return is_rect_collision(rect_from_vect(unit_offset, 32, 32), rect_from_vect(ZERO_VECTOR, SCREEN_WIDTH, SCREEN_HEIGHT));
}

bool is_unit_selected(int index){

    for(int i = 0; i < player_selection_size; i++){

        if(player_selection[i] == index){

            return true;
        }
    }

    return false;
}

int_vector get_player_unit_position(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for get_player_unit_position() at index %i\n", index);
        return as_int_vector(NULL_VECTOR);
    }

    return as_int_vector(player_units[index]->position);
}

rectangle get_player_unit_rect(int index){

    if(index >= player_units_size){

        printf("Error! Player unit index out of bounds for get_player_unit_position() at index %i\n", index);
        return rect_from_vect(NULL_VECTOR, 0, 0);
    }

    return rect_from_vect(player_units[index]->position, 32, 32);
}

// UNIT FUNCTIONS

void add_unit(int start_x, int start_y){

    player_units[player_units_size] = (unit*)malloc(sizeof(unit));
    *(player_units[player_units_size]) = (unit){

        .position = new_vector((double)start_x, (double)start_y),
        .blocked_path_timer = 0,
        .waypoint = NULL
    };
    player_units_size++;
}

void free_unit(int index){

    unit_waypoint_pop_all(index);
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

void unit_waypoint_pop_all(int index){

    while(!vectors_equal(unit_waypoint_pop(index), NULL_VECTOR)){}
}

// SELECTION FUNCTIONS

void player_selection_clear(){

    if(player_selection != NULL){

        free(player_selection);
    }
    player_selection_size = 0;
    player_selection = NULL;
}

void player_selection_select_one(int target_x, int target_y){

    player_selection_clear();

    vector selection_point = new_vector((float)target_x, (float)target_y);
    for(int index = 0; index < player_units_size; index++){

        if(is_point_in_rect(selection_point, get_player_unit_rect(index))){

            player_selection = (int*)malloc(sizeof(int));
            player_selection[0] = index;
            player_selection_size = 1;
            return;
        }
    }
}

void player_selection_select_many(rectangle selection_rect){

    player_selection_clear();

    // Figure out how many will be selected
    int to_select = 0;
    for(int index = 0; index < player_units_size; index++){

        if(is_rect_collision(selection_rect, get_player_unit_rect(index))){

            to_select++;
        }
    }

    if(to_select == 0){

        return;
    }

    // Initialize player selection array
    player_selection = (int*)malloc(sizeof(int) * to_select);
    for(int index = 0; index < player_units_size; index++){

        if(is_rect_collision(selection_rect, get_player_unit_rect(index))){

            player_selection[player_selection_size] = index;
            player_selection_size++;
            if(player_selection_size == to_select){

                return;
            }
        }
    }
}

// UPDATE FUNCTIONS

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

            // Check if path will be blocked before moving
            int square_magnitude = min(vector_distance(target_position, player_units[index]->position), 16);
            vector square_delta = vector_scale(vector_difference(target_position, player_units[index]->position), square_magnitude);
            vector square_position = vector_sum(player_units[index]->position, square_delta);
            rectangle square_unit_rect = rect_from_vect(square_position, 32, 32);
            bool is_collision = false;
            for(int other_index = 0; other_index < player_units_size; other_index++){

                if(index == other_index){

                    continue;
                }

                if(is_rect_collision(square_unit_rect, get_player_unit_rect(other_index))){

                    is_collision = true;
                    break;
                }
            }

            if(is_collision){

                player_units[index]->blocked_path_timer += delta;
                if(player_units[index]->blocked_path_timer >= UNIT_BLOCKED_PATH_TIMEOUT){

                    unit_waypoint_pop_all(index);
                    player_units[index]->blocked_path_timer = 0;
                }

            }else{

                player_units[index]->blocked_path_timer = 0;

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

    player_selection_select_one(mouse_x + camera_position.x, mouse_y + camera_position.y);
}

void input_handle_right_click(int mouse_x, int mouse_y){

    for(int index = 0; index < player_selection_size; index++){

        if(player_selection[index] == -1){

            return;
        }

        unit_waypoint_pop_all(player_selection[index]);
        unit_waypoint_add(player_selection[index], mouse_x + camera_position.x, mouse_y + camera_position.y);
    }
}

void input_handle_select_rect(rectangle select_rect){

    rectangle offset_rect = select_rect;
    offset_rect.x += camera_position.x;
    offset_rect.y += camera_position.y;
    player_selection_select_many(offset_rect);
}
