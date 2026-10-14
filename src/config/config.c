
#include "config.h"
#include "git2/types.h"
#include "internal.h"

#include "../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int gud_on_checkout(lua_State *state) {
  trace("gud_on_checkout called\n");
  luaL_checktype(state, 1, LUA_TFUNCTION);
  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  if (config->on_checkout.n >= REFS_MAX) {
    return luaL_error(state, "to many on_checkout handlers registered");
  }

  lua_pushvalue(state, 1);
  int new_ref = luaL_ref(state, LUA_REGISTRYINDEX);

  config->on_checkout.refs[config->on_checkout.n++] = new_ref;
  trace("ref added\n");

  return 0;
}

static int lua_trace(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *msg = lua_tostring(state, 1);

  trace("%s\n", msg);

  return 0;
}

static const luaL_Reg gud_funcs[] = {{"on_checkout", gud_on_checkout},

                                     {NULL, NULL}};

config_t *config_init(const char *path, git_repository *repo,
                      git_repository *parent) {
  config_t *config = malloc(sizeof(config_t));
  if (!config) {
    return NULL;
  }

  config->state = luaL_newstate();
  if (!config->state) {
    goto failed_lua_init;
  }

  config->repo = repo;
  config->parent = parent;

  config->on_checkout.n = 0;
  for (int i = 0; i < REFS_MAX; i++) {
    config->on_checkout.refs[i] = LUA_NOREF;
  }

  luaL_openlibs(config->state);

  lua_pushcfunction(config->state, lua_trace);
  lua_setglobal(config->state, "trace");

  lua_newtable(config->state);
  lua_pushlightuserdata(config->state, config);
  luaL_setfuncs(config->state, gud_funcs, 1);
  lua_setglobal(config->state, "gud");

  if (luaL_dofile(config->state, path) != LUA_OK) {
    const char *msg = lua_tostring(config->state, -1);

    trace("failed to execute config [%s]: %s\n", path, msg);

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
    for (int i = 0; i < config->on_checkout.n; i++) {
      luaL_unref(config->state, LUA_REGISTRYINDEX, config->on_checkout.refs[i]);
    }

    lua_close(config->state);

    git_repository_free(config->parent);

    git_repository_free(config->repo);
  }

  free(config);
}
