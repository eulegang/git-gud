#include "on_post_checkout.h"
#include "config.h"
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char PATH_SCRATCH[PATH_MAX];

static int post_checkout_event_exists(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  bool test = access(path, F_OK) == 0;

  lua_pushboolean(state, test);
  return 1;
}

static int post_checkout_event_root_has(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  strncpy(PATH_SCRATCH, config_root(config), PATH_MAX);
  strncat(PATH_SCRATCH, path, PATH_MAX);

  bool test = access(path, F_OK) == 0;

  lua_pushboolean(state, test);

  return 1;
}

static int post_checkout_event_link(lua_State *) { return 0; }
static int post_checkout_event_copy(lua_State *) { return 0; }
static int post_checkout_event_shell(lua_State *) { return 0; }
static int post_checkout_event_changes(lua_State *) { return 0; }

static const luaL_Reg post_checkout_event_funcs[] = {
    {"link", post_checkout_event_link},
    {"copy", post_checkout_event_copy},
    {"shell", post_checkout_event_shell},
    {"exists", post_checkout_event_exists},
    {"root_has", post_checkout_event_root_has},
    {"changes", post_checkout_event_changes},

    {NULL, NULL}};

void config_post_checkout(config_t *config, post_checkout_t checkout) {

  lua_newtable(config->state);

  lua_pushstring(config->state, checkout.next);
  lua_setfield(config->state, -2, "next");

  lua_pushstring(config->state, checkout.prev);
  lua_setfield(config->state, -2, "prev");

  lua_pushboolean(config->state, checkout.checkout == CheckoutBranch);
  lua_setfield(config->state, -2, "is_branch");

  lua_pushboolean(config->state, checkout.checkout == CheckoutFile);
  lua_setfield(config->state, -2, "is_file");

  lua_pushlightuserdata(config->state, config);
  lua_pushlightuserdata(config->state, &checkout);

  luaL_setfuncs(config->state, post_checkout_event_funcs, 2);

  for (int i = 0; i < config->on_checkout.n; i++) {
    int ref = config->on_checkout.refs[i];

    lua_rawgeti(config->state, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(config->state, -2);
  }

  lua_pop(config->state, -1);
}
