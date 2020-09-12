#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif
#include "global.h"
#include "resources.h"
#include "combat.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

bool init_engine();
void quit_engine();

void input();
void update();
void render();

void render_combat();

void render_text(TTF_Font* font, char* text, SDL_Color color, int x, int y);
void render_image(ImageName image_name, int x, int y);
void render_part(ImageName image_name, int x, int y, int source_x, int source_y, int width, int height);
void render_fps();

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

const SDL_Color color_white = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 225 };
const SDL_Color color_red = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 225 };

TTF_Font* font_small = NULL;

bool mouse_captured = false;
int mouse_x = 0;
int mouse_y = 0;
int mouse_relative_x = 0;
int mouse_relative_y = 0;

const unsigned long SECOND = 1000;
const int TARGET_FPS = 60;
const double FRAME_TIME = SECOND / (double)TARGET_FPS;
const int TARGET_UPS = 25;
const double UPDATE_TIME = SECOND / (double)TARGET_UPS;
unsigned long second_before_time;
unsigned long frame_before_time;
unsigned long last_update_time;
bool running = true;
double delta = 0;
int frames_this_second = 0;
double deltas_this_second = 0;
int fps = 0;
int ups = 0;

int main(){

    bool init_successful = init_engine();
    if(!init_successful){

        printf("Exiting...\n");
        return 0;
    }

    second_before_time = SDL_GetTicks();
    frame_before_time = second_before_time;
    last_update_time = second_before_time;

    init_combat();

    while(running){

        input();
        update();
        render();

        frames_this_second++;
        unsigned long current_time = SDL_GetTicks();

        if(current_time - second_before_time >= SECOND){

            fps = frames_this_second;
            ups = (int)deltas_this_second;
            frames_this_second = 0;
            deltas_this_second -= ups;
            second_before_time += SECOND;
        }

        double current_delta = (current_time - last_update_time) / UPDATE_TIME;
        delta += current_delta;
        deltas_this_second += current_delta;
        last_update_time = current_time;

        if(current_time - frame_before_time < FRAME_TIME){

            unsigned long delay_time = FRAME_TIME - (current_time - frame_before_time);
            SDL_Delay(delay_time);
        }

        frame_before_time = SDL_GetTicks();
    }

    deinit_combat();
    quit_engine();
    return 0;
}

void input(){

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0){

        if(e.type == SDL_QUIT){

            running = false;

        }else if(e.type == SDL_KEYDOWN){

            int key = e.key.keysym.sym;

            if(key == SDLK_ESCAPE){

                if(mouse_captured){

                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    mouse_captured = false;

                }else{

                    running = false;
                }
            }

        }else if(e.type == SDL_MOUSEBUTTONDOWN){

            if(!mouse_captured){

                SDL_SetRelativeMouseMode(SDL_TRUE);
                mouse_captured = true;
                continue;
            }

        }else if(e.type == SDL_MOUSEBUTTONUP){

            if(!mouse_captured){

                continue;
            }

            if(e.button.button == SDL_BUTTON_RIGHT){

                input_handle_right_click(e.button.x, e.button.y);
            }

        }else if(e.type == SDL_MOUSEMOTION){

            mouse_x = min(e.motion.x, SCREEN_WIDTH - CURSOR_WIDTH);
            mouse_y = min(e.motion.y, SCREEN_HEIGHT - CURSOR_HEIGHT);
            mouse_relative_x = e.motion.xrel;
            mouse_relative_y = e.motion.yrel;

            input_update_camera(mouse_x, mouse_y, mouse_relative_x, mouse_relative_y);
        }
    }
}

void update(){

    if(delta < 1){

        return;
    }

    if(mouse_captured){

        int full_deltas = (int)delta;
        delta -= full_deltas;
        update_combat(full_deltas);
    }
}

void render(){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    render_combat();

    if(mouse_captured){

        render_image(IMAGE_CURSOR, mouse_x, mouse_y);
    }

    render_fps();
    SDL_RenderPresent(renderer);
}

void render_combat(){

    int_vector camera_position = get_camera_position();
    rectangle screen_rect = (rectangle){ .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT };
    for(int x = 0; x < get_map_tile_width(); x++){

        for(int y = 0; y < get_map_tile_height(); y++){

            int draw_x = (x * MAP_TILE_SIZE) - camera_position.x;
            int draw_y = (y * MAP_TILE_SIZE) - camera_position.y;
            if(is_rect_collision((rectangle){ .x = draw_x, .y = draw_y, .width = MAP_TILE_SIZE, .height = MAP_TILE_SIZE }, screen_rect)){

                render_image(IMAGE_GRASS_TILE, (x * MAP_TILE_SIZE) - camera_position.x, (y * MAP_TILE_SIZE) - camera_position.y);
            }
        }
    }

    for(int index = 0; index < get_player_units_size(); index++){

        if(is_unit_on_screen(index)){

            int_vector unit_position = get_player_unit_position(index);
            render_image(IMAGE_KNIGHT, unit_position.x - camera_position.x, unit_position.y - camera_position.y);
        }
    }
}

void render_text(TTF_Font* font, char* text, SDL_Color color, int x, int y){

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, color);

    if(text_surface == NULL){

        printf("Unable to render text to surface! SDL Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    if(text_texture == NULL){

        printf("Unable to create texture! SDL Error: %s\n", SDL_GetError());
        return;
    }

    int draw_x = x;
    int draw_y = y;

    if(draw_x == -1){

        draw_x = (SCREEN_WIDTH / 2) - (text_surface->w / 2);
    }
    if(draw_y == -1){

        draw_y = (SCREEN_HEIGHT / 2) - (text_surface->h / 2);
    }

    SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
    SDL_Rect dest_rect = (SDL_Rect){ .x = draw_x, .y = draw_y, .w = text_surface->w, .h = text_surface->h };
    SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_image(ImageName image_name, int x, int y){

    SDL_Rect dest_rect = (SDL_Rect){ .x = x, .y = y, .w = textures[image_name].width, .h = textures[image_name].height };
    SDL_RenderCopy(renderer, textures[image_name].texture, NULL, &dest_rect);
}

void render_part(ImageName image_name, int x, int y, int source_x, int source_y, int width, int height){

    if(source_x < 0 || source_y < 0 || source_x >= textures[image_name].width || source_y >= textures[image_name].height){

        printf("Cannot render part! Source pos of (%i, %i) is out of bounds for image_name %i\n", source_x, source_y, image_name);
        return;
    }

    int source_width = width;
    int source_height = height;

    if(source_width > textures[image_name].width - source_x){

        source_width = textures[image_name].width - source_x;
    }
    if(source_height > textures[image_name].height - source_y){

        source_height = textures[image_name].height - source_y;
    }

    SDL_Rect source_rect = (SDL_Rect){ .x = source_x, .y = source_y, .w = source_width, .h = source_height };
    SDL_Rect dest_rect = (SDL_Rect){ .x = x, .y = y, .w = source_width, .h = height };
    SDL_RenderCopy(renderer, textures[image_name].texture, &source_rect, &dest_rect);
}

void render_fps(){

    char fps_text[10];
    sprintf(fps_text, "FPS: %i", fps);
    render_text(font_small, fps_text, color_red, 0, 0);
    char ups_text[15];
    sprintf(ups_text, "UPS: %i", ups);
    render_text(font_small, ups_text, color_red, 0, 10);
}

bool init_engine(){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){

        printf("Unable to initialize SDL!\n");
        return false;
    }

    window = SDL_CreateWindow("Highlands", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    int img_flags = IMG_INIT_PNG;

    if(!(IMG_Init(img_flags) & img_flags)){

        printf("Unable to initialize SDL_image! SDL Error: %s\n", IMG_GetError());
        return false;
    }

    if(TTF_Init() == -1){

        printf("Unable to initialize SDL_ttf! SDL Error: %s\n", TTF_GetError());
        return false;
    }

    if(!window || !renderer){

        printf("Unable to initialize engine!\n");
        return false;
    }

    font_small = TTF_OpenFont("./res/notosans.ttf", 10);
    if(font_small == NULL){

        printf("Unable to initialize font_small! TTF Error: %s\n", TTF_GetError());
        return false;
    }

    if(!load_textures(renderer)){

        return false;
    };

    return true;
}

void quit_engine(){

    TTF_CloseFont(font_small);

    free_textures();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
