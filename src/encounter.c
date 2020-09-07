#include "encounter.h"

static const int MAP_WIDTH = 2560;
static const int MAP_HEIGHT = 1440;
static const int MAP_EDGE_SCROLL_GAP = 5;

static const int CAMERA_MAX_SPEED = 150;
static const int CAMERA_ACCELERATION_SPEED = 50;

static Encounter encounter;

vector get_camera_position(){

    return encounter.camera_position;
}

void init_encounter(){

    encounter.camera_position = ZERO_VECTOR;
    encounter.camera_speed = ZERO_VECTOR;
    encounter.camera_direction = ZERO_VECTOR;
}

void update_encounter(double delta){

    update_camera(delta);
}

void update_camera(double delta){

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
    encounter.camera_speed = vector_sum(encounter.camera_speed, vector_mult_double(camera_acceleration, delta));

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
    encounter.camera_position = vector_sum(encounter.camera_position, vector_mult_double(encounter.camera_speed, delta));

    // Check camera still in bounds
    encounter.camera_position.x = max(0, min(encounter.camera_position.x, MAP_WIDTH - SCREEN_WIDTH));
    encounter.camera_position.y = max(0, min(encounter.camera_position.y, MAP_HEIGHT - SCREEN_HEIGHT));
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
