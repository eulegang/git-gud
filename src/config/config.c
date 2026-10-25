
#include "config.h"
#include "git2/types.h"
#include "internal.h"

#include "../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>

static int gud_on_checkout(lua_State *state) {
  syslog(LOG_DEBUG, "gud_on_checkout called");
  luaL_checktype(state, 1, LUA_TFUNCTION);
  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  lua_pushvalue(state, 1);
  int new_ref = luaL_ref(state, LUA_REGISTRYINDEX);

  if (!luaC_push_ref(&config->on_checkout, new_ref)) {
    luaL_unref(state, LUA_REGISTRYINDEX, new_ref);
    return luaL_error(state, "to many on_checkout handlers registered");
  }

  return 0;
}

static int gud_on_commit_msg(lua_State *state) {
  syslog(LOG_DEBUG, "gud_on_commit_msg called");
  luaL_checktype(state, 1, LUA_TFUNCTION);
  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  lua_pushvalue(state, 1);
  int new_ref = luaL_ref(state, LUA_REGISTRYINDEX);

  if (!luaC_push_ref(&config->on_commit_msg, new_ref)) {
    luaL_unref(state, LUA_REGISTRYINDEX, new_ref);
    return luaL_error(state, "to many on_checkout handlers registered");
  }

  return 0;
}

static int lua_trace(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *msg = lua_tostring(state, 1);

  syslog(LOG_INFO, "%s", msg);

  return 0;
}

static int lua_fail(lua_State *state) {
  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  int argc = lua_gettop(state);

  config->status |= STATUS_FAIL;

  const char *msg = "failing custom script";

  if (argc > 0) {
    msg = lua_tostring(state, 1);
    (void)fprintf(stderr, "%s\n", msg);
  }

  return luaL_error(state, "%s\n", msg);
}

static const luaL_Reg gud_funcs[] = {{"on_checkout", gud_on_checkout},
                                     {"on_commit_msg", gud_on_commit_msg},
                                     {NULL, NULL}};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
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

  // NOLINTNEXTLINE(bugprone-signed-bitwise)
  luaL_openlibs(config->state);

  lua_pushcfunction(config->state, lua_trace);
  lua_setglobal(config->state, "trace");

  lua_pushlightuserdata(config->state, config);
  lua_pushcclosure(config->state, lua_fail, 1);
  lua_setglobal(config->state, "fail");

  lua_newtable(config->state);
  lua_pushlightuserdata(config->state, config);
  luaL_setfuncs(config->state, gud_funcs, 1);
  lua_setglobal(config->state, "gud");

  if (luaL_dofile(config->state, path) != LUA_OK) {
    const char *msg = lua_tostring(config->state, -1);

    syslog(LOG_ERR, "failed to execute config [%s]: %s", path, msg);
    (void)fprintf(stderr, "failed to execute config [%s]: %s\n", path, msg);

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
