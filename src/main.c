#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif
#include "resources.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360

bool init_engine();
void quit_engine();

void input();
void update();
void render();

void render_text(TTF_Font* font, char* text, SDL_Color color, int x, int y);
void render_image(ImageName image_name, int x, int y);
void render_fps();

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

const SDL_Color color_white = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 225 };

TTF_Font* font_small = NULL;

bool mouse_captured = false;
int mouse_x = 0;
int mouse_y = 0;

const unsigned long SECOND = 1000;
const double TARGET_FPS = 60;
const unsigned long FRAME_TIME = (unsigned long)(SECOND / (double)TARGET_FPS);
unsigned long second_before_time;
unsigned long frame_before_time;
unsigned long current_time;
bool running = true;
double delta = 0;
int frames = 0;
int fps = 0;

int main(){

    bool init_successful = init_engine();
    if(!init_successful){

        printf("Exiting...\n");
        return 0;
    }

    second_before_time = SDL_GetTicks();
    frame_before_time = second_before_time;

    while(running){

        input();
        update();
        render();

        frames++;
        current_time = SDL_GetTicks();

        if(current_time - second_before_time >= SECOND){

            fps = frames;
            frames = 0;
            second_before_time += SECOND;
        }

        delta = (current_time - frame_before_time) / (double)FRAME_TIME;

        if(current_time - frame_before_time < FRAME_TIME){

            unsigned long delay_time = FRAME_TIME - (current_time - frame_before_time);
            SDL_Delay(delay_time);
        }

        frame_before_time = SDL_GetTicks();
    }

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
        }
    }

    SDL_GetMouseState(&mouse_x, &mouse_y);
}

void update(){


}

void render(){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if(mouse_captured){

        render_image(IMAGE_CURSOR, mouse_x, mouse_y);
    }

    render_fps();

    SDL_RenderPresent(renderer);
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

void render_fps(){

    char fps_text[10];
    sprintf(fps_text, "FPS: %i", fps);
    render_text(font_small, fps_text, color_white, 0, 0);
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
