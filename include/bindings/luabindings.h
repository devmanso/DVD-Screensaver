#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <stdio.h>

int delay(Uint32 ms) {
    SDL_Delay(ms);
    return 0;
}

// Declare/define lua functions here

static int lua_wait(lua_State *L) {
    Uint32 ms = luaL_checkinteger(L, 1);
    delay(ms);
    return 0;
}

void createLuaBindings() {
    printf("CREATING LUA STATE... \n");
    lua_State *L = luaL_newstate();

    // Register C functions to Lua here
    lua_pushcfunction(L, lua_wait);
    lua_setglobal(L, "wait");

    // lua function testing
    // Execute a Lua script that calls a function
    luaL_dostring(L, "wait(5000)");
    
    // Close the Lua state
    lua_close(L);
    printf("LUA STATE CLOSED \n");
}