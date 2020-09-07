#ifndef RESOURCES_H
#define RESOURCES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct{

    SDL_Texture* texture;
    int width;
    int height;
} Texture;

typedef enum{
    IMAGE_CURSOR,
    IMAGE_MAP,
    IMAGE_KNIGHT,
    IMAGENAME_COUNT
} ImageName;

extern char* image_paths[IMAGENAME_COUNT];
extern Texture textures[IMAGENAME_COUNT];

bool load_textures(SDL_Renderer* renderer);
void free_textures();

#endif
