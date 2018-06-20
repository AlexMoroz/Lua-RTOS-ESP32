/**
 * Author: Alex Moroz; https://github.com/AlexMoroz
 * 
 * Lua RTOS, Lua protocols module
 **/

#include "sdkconfig.h"

// #if CONFIG_LUA_RTOS_LUA_USE_PROTOCOLS

#include "lua.h"
#include "lauxlib.h"
#include "protocols_coap_server.inc"


static const LUA_REG_TYPE protocols_map[] = {
    { LSTRKEY( "coap" ), LROVAL ( lcoap_map ) },
    { LNILKEY, LNILVAL }
};

// #endif