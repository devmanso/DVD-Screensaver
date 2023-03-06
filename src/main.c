/*
    Credit to Thomas Lively (@tlively on github), for sdl_seminar/hello4_animation.c
    I have modified some of his code, but it is mostly the same, I have added bindings
    for this game to the Lua scripting language.

    The game will load a Lua file and check for any function calls, if it finds one
    (like "setspeed(200)"), it will change the behavior of the program, such as the
    speed of the logo present on the screen, or a delay before executing.
*/

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>


int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;
int SPEED = 100;
char title[] = "DVD ScreenSaver";

int delay(Uint32 ms) {
    SDL_Delay(ms);
    return 0;
}

int setSpeed(int speed) {
    SPEED = speed;
    return 0;
}

int setTitle(char title_name[]) {
    // This is unsafe code, check the commented code below
    strcpy(title, title_name);

    // I've commented out the code below, because I'm using the C99 compiler,
    // which does not have support for strcpy_s; strcpy_s is not a part of
    // the standard C library, but rather a part of the C11 standard Annex K,
    // which provides bounds-checking interfaces for certain functions.
    // Use strcpy_s instead of strcpy to avoid buffer overflow issues
//    if (strcpy_s(title, sizeof(title), title_name) != 0) {
//        return 1;
//    }
    // if you have a C11 compliant compiler, then you can uncomment the code above
    return 0;
}

// Lua functions, which are basically Lua function wrappers for C functions

static int lua_setSpeed(lua_State *L) {
    int lua_speed = luaL_checkinteger(L, 1);
    setSpeed(lua_speed);
    return 0; // return statement is arguments to the Lua stack
}

static int lua_wait(lua_State *L) {
    Uint32 ms = luaL_checkinteger(L, 1);
    delay(ms);
    return 0;
}

static int lua_setTitle(lua_State *L) {
    const char *new_title = luaL_checkstring(L, 1);
    setTitle(new_title);
    return 0;
}

int main() {
    // Initialise Lua before initialising SDL or anything else
    printf("CREATING LUA STATE... \n");
    lua_State *L = luaL_newstate();

    // push C functions to lua
    lua_pushcfunction(L, lua_setSpeed);
    lua_setglobal(L, "setspeed");
    lua_pushcfunction(L, lua_wait);
    lua_setglobal(L, "wait");
    lua_pushcfunction(L, lua_setTitle);
    lua_setglobal(L, "settitle");

    // load Lua file, be sure to load the file AFTER pushing the C functions to lua
    luaL_dofile(L, "config.lua");
    
    // check if file is actually loaded
    if (luaL_dofile(L, "src/config.lua") != 0) {
        printf("could not find, or load config file! \n");
        printf("error info: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return 1;
    }

    // Testing lua methods to ensure functions work as intended
    //luaL_dostring(L, "settitle('shmebulock')");
    //luaL_dostring(L, "wait(1000)");
    //luaL_dostring(L, "setspeed(1000)");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL failed to initialise");
    }

    // create a window with properties
    SDL_Window* window = SDL_CreateWindow(title,
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    // check if window is created
    if (!window) {
        printf("failed to create window");
        SDL_Quit();
        return 1;
    }

    // use whatever graphics driver is availaible
    Uint32 hardware_flag = SDL_RENDERER_ACCELERATED;
    // use software rendering, not as good as using a dedicated GPU driver
    Uint32 software_flag = SDL_RENDERER_SOFTWARE;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, hardware_flag);
    if (!renderer) {
        printf("could not create renderer, check the renderflags");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load image to memory
    SDL_Surface* dvdSurface = IMG_Load("art/dvd_screensaver.png");
    if (!dvdSurface) {
        printf("surface (images) could not be loaded into memory");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load surface (image data) into graphics memory
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, dvdSurface);
    // now that the image has been loaded into graphics memory, we can free it from
    // the regular memory (RAM)
    SDL_FreeSurface(dvdSurface);
    
    // struct used to hold the position and size of the texture
    SDL_Rect dest;

    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 4;
    dest.h /= 4;

    // display image at the center of the screen
    float x_pos = (WINDOW_WIDTH -dest.w) /2;
    float y_pos = (WINDOW_HEIGHT -dest.h) /2;

    float x_vel = SPEED;
    float y_vel = SPEED;
    // set to 1 when 'X', red dot, or whatever icon is used to quit an app is pressed.
    int close_requested = 0;

    // animation
    while (!close_requested) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                close_requested = 1;
            }
        }
        if (x_pos <= 0) {
            x_pos = 0;
            x_vel = -x_vel;
        }
        if (y_pos <= 0) {
            y_pos = 0;
            y_vel = -y_vel;
        }
        if (x_pos >= WINDOW_WIDTH - dest.w) {
            x_pos = WINDOW_WIDTH - dest.w;
            x_vel = -x_vel;
        }
        if (y_pos >= WINDOW_HEIGHT - dest.h) {
            y_pos = WINDOW_HEIGHT - dest.h;
            y_vel = -y_vel;
        }

        // update positions
        x_pos += x_vel / 60;
        y_pos += y_vel / 60;

        // set the positions in the struct
        dest.y = (int) y_pos;
        dest.x = (int) x_pos;
        
        // clear the window
        SDL_RenderClear(renderer);

        // draw the image to the window
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        // this blatantly assumes that what we are doing is not that computationally
        // expensive, and can be done very quick. IMO this is bad practice, but it
        // this is a small project.
        SDL_Delay(1000/60);
    }
    
    //clean up before exiting
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    lua_close(L);
    return 0;
}