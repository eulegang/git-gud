
#include "config.h"
#include "git2/repository.h"
#include "git2/strarray.h"
#include "git2/worktree.h"
#include "internal.h"

#include "../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int gud_on_checkout(lua_State *state) {
  luaL_checktype(state, 1, LUA_TFUNCTION);
  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  if (config->on_checkout.n >= REFS_MAX) {
    return luaL_error(state, "to many on_checkout handlers registered");
  }

  lua_pushvalue(state, 1);
  int new_ref = luaL_ref(state, LUA_REGISTRYINDEX);

  config->on_checkout.refs[config->on_checkout.n++] = new_ref;

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

config_t *config_init(const char *path, git_repository *repo) {
  config_t *config = malloc(sizeof(config_t));
  if (!config) {
    return NULL;
  }

  config->state = luaL_newstate();
  if (!config->state) {
    goto failed_lua_init;
  }

  config->repo = repo;

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
    for (int i = 0; i < config->on_checkout.n; i++) {
      luaL_unref(config->state, LUA_REGISTRYINDEX, config->on_checkout.refs[i]);
    }

    lua_close(config->state);
  }

  free(config);
}

static bool str_startswith(const char *restrict prefix,
                           const char *restrict content, int n) {

  if (!prefix || !content)
    return false;

  for (int i = 0; i < n; i++) {
    if (prefix[i] == 0)
      return true;
    if (prefix[i] != content[i])
      return false;
  }

  return false;
}

static char REPO_ROOT[PATH_MAX] = {0};

const char *config_root(config_t *config) {
  if (REPO_ROOT[0] == 0) {
    git_strarray paths;
    git_worktree_list(&paths, config->repo);

    auto common = git_repository_commondir(config->repo);

    for (size_t i = 0; i < paths.count; i++) {
      auto path = paths.strings[i];

      if (str_startswith(path, common, PATH_MAX)) {
        strncpy(REPO_ROOT, path, PATH_MAX);
        break;
      }
    }
  }

  return REPO_ROOT;
}
