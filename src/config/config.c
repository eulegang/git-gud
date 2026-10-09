
#include "config.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

struct config {
  lua_State *state;
};

config_t *config_init(const char *path) {
  config_t *config = malloc(sizeof(config_t));
  if (!config) {
    return NULL;
  }

  config->state = luaL_newstate();
  if (!config->state) {
    goto failed_lua_init;
  }

  luaL_openlibs(config->state);

  if (luaL_dofile(config->state, path) != LUA_OK) {
    goto failed_eval;
  };

  return config;

failed_eval:
  lua_close(config->state);

failed_lua_init:
  free(config);

  return NULL;
}

void config_deinit(config_t *config) {
  if (config) {
    lua_close(config->state);
  }

  free(config);
}
