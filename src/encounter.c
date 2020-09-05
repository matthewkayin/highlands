#include "encounter.h"

Encounter init_encounter(){

    Encounter encounter;

    encounter.camera_x = 0;
    encounter.camera_y = 0;

    return encounter;
}

Encounter update_encounter(Encounter previous_state, double delta, int mouse_x, int mouse_y){

    Encounter next_state = previous_state;

    // Update camera movement
    if(mouse_x == 0){

        next_state.camera_x -= CAMERA_SPEED * delta;

    }else if(mouse_x == SCREEN_WIDTH - CURSOR_WIDTH){

        next_state.camera_x += CAMERA_SPEED * delta;
    }
    if(mouse_y == 0){

        next_state.camera_y -= CAMERA_SPEED * delta;

    }else if(mouse_y == SCREEN_HEIGHT - CURSOR_HEIGHT){

        next_state.camera_y += CAMERA_SPEED * delta;
    }
    if(next_state.camera_x < 0){

        next_state.camera_x = 0;

    }else if(next_state.camera_x > MAP_WIDTH - SCREEN_WIDTH){

        next_state.camera_x = MAP_WIDTH - SCREEN_WIDTH;
    }
    if(next_state.camera_y < 0){

        next_state.camera_y = 0;

    }else if(next_state.camera_y > MAP_HEIGHT - SCREEN_HEIGHT){

        next_state.camera_y = MAP_HEIGHT - SCREEN_HEIGHT;
    }

    return next_state;
}
