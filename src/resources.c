#include "resources.h"

// Define externs
char* image_paths[IMAGENAME_COUNT] = {
    "res/cursor.png"
};
Texture textures[IMAGENAME_COUNT];

bool load_textures(SDL_Renderer* renderer){

    for(int image_name = 0; image_name < IMAGENAME_COUNT; image_name++){

        SDL_Surface* loaded_surface = IMG_Load(image_paths[image_name]);

        if(loaded_surface == NULL){

            printf("Unable to load image! SDL Error: %s\n", IMG_GetError());
            return false;
        }

        textures[image_name].texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);

        if(textures[image_name].texture == NULL){

            printf("Unable to create texture! SDL Error: %s\n", IMG_GetError());
            return false;
        }

        textures[image_name].width = loaded_surface->w;
        textures[image_name].height = loaded_surface->h;

        SDL_FreeSurface(loaded_surface);
    }

    return true;
}

void free_textures(){

    for(int image_name = 0; image_name < IMAGENAME_COUNT; image_name++){

        SDL_DestroyTexture(textures[image_name].texture);
    }
}
