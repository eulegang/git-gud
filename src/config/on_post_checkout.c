#include "on_post_checkout.h"
#include "../util.h"
#include "config.h"
#include "git2/commit.h"
#include "git2/diff.h"
#include "git2/oid.h"
#include "git2/repository.h"
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/syslimits.h>
#include <unistd.h>

static char PATH_SCRATCH[PATH_MAX];
static char PATH_TARGET_SCRATCH[PATH_MAX];

static int post_checkout_event_exists(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  if (strlcpy(PATH_SCRATCH, git_repository_workdir(config->repo), PATH_MAX)) {
    return luaL_error(state, "overflown path buffer");
  }

  if (strlcat(PATH_SCRATCH, path, PATH_MAX) >= PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }

  trace("exists('%s')\n", PATH_SCRATCH);
  bool test = access(PATH_SCRATCH, F_OK) == 0;

  lua_pushboolean(state, test);
  return 1;
}

static int post_checkout_event_root_has(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  const char *base = git_repository_workdir(config->repo);
  if (base == NULL) {
    lua_pushboolean(state, false);
    return 1;
  }

  if (strlcpy(PATH_SCRATCH, git_repository_workdir(config->parent), PATH_MAX) >=
      PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }

  if (strlcat(PATH_SCRATCH, path, PATH_MAX) >= PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }

  trace("root_has('%s')\n", PATH_SCRATCH);
  bool test = access(PATH_SCRATCH, F_OK) == 0;

  lua_pushboolean(state, test);

  return 1;
}

static int post_checkout_event_link(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));

  if (strlcpy(PATH_SCRATCH, git_repository_workdir(config->parent), PATH_MAX) >=
      PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }
  if (strlcat(PATH_SCRATCH, path, PATH_MAX) >= PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }

  if (strlcpy(PATH_TARGET_SCRATCH, git_repository_workdir(config->repo),
              PATH_MAX) >= PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }
  if (strlcat(PATH_TARGET_SCRATCH, path, PATH_MAX) >= PATH_MAX) {
    return luaL_error(state, "overflown path buffer");
  }

  int res = link(PATH_SCRATCH, PATH_TARGET_SCRATCH);

  bool test = res != -1;

  lua_pushboolean(state, test);
  return 1;
}

static int post_checkout_event_changes(lua_State *state) {
  luaL_checktype(state, 1, LUA_TSTRING);
  const char *path = lua_tostring(state, 1);

  auto config = (config_t *)lua_touserdata(state, lua_upvalueindex(1));
  auto checkout = (post_checkout_t *)lua_touserdata(state, lua_upvalueindex(2));

  git_oid oid_old, oid_new;
  git_commit *commit_old = NULL, *commit_new = NULL;
  git_tree *tree_old = NULL, *tree_new = NULL;
  git_diff *diff = NULL;

  if (git_oid_fromstr(&oid_old, checkout->prev) != 0) {
    return luaL_error(state, "failed to convert git hash to oid: %s",
                      checkout->prev);
  }

  if (git_oid_fromstr(&oid_new, checkout->next) != 0) {
    return luaL_error(state, "failed to convert git hash to oid: %s",
                      checkout->next);
  }

  git_commit_lookup(&commit_old, config->repo, &oid_old);
  git_commit_lookup(&commit_new, config->repo, &oid_new);

  if (git_commit_tree(&tree_old, commit_old) != 0) {
    return luaL_error(state, "failed to resolve commit content %s",
                      checkout->prev);
  }

  if (git_commit_tree(&tree_new, commit_new) != 0) {
    return luaL_error(state, "failed to resolve commit content %s",
                      checkout->next);
  }

  git_diff_options opts;
  git_diff_options_init(&opts, GIT_DIFF_OPTIONS_VERSION);

  const char *paths[] = {path};
  opts.pathspec.count = 1;
  opts.pathspec.strings = (char **)paths;

  if (git_diff_tree_to_tree(&diff, config->repo, tree_old, tree_new, &opts) !=
      0) {
    return luaL_error(state, "failed to calculate diff between %s and %s",
                      checkout->prev, checkout->next);
  }

  auto test = git_diff_num_deltas(diff) > 0;

  lua_pushboolean(state, test);

  return 1;
}

static const luaL_Reg post_checkout_event_funcs[] = {
    {"link", post_checkout_event_link},
    {"exists", post_checkout_event_exists},
    {"root_has", post_checkout_event_root_has},
    {"changes", post_checkout_event_changes},

    {NULL, NULL}};

void config_post_checkout(config_t *config, post_checkout_t checkout) {
  git_oid oid_old, oid_new;

  git_oid_fromstr(&oid_old, checkout.prev);
  git_oid_fromstr(&oid_new, checkout.next);

  lua_newtable(config->state);

  lua_pushstring(config->state, checkout.next);
  lua_setfield(config->state, -2, "next");

  lua_pushstring(config->state, checkout.prev);
  lua_setfield(config->state, -2, "prev");

  lua_pushboolean(config->state, checkout.checkout == CheckoutBranch);
  lua_setfield(config->state, -2, "is_branch");

  lua_pushboolean(config->state, checkout.checkout == CheckoutFile);
  lua_setfield(config->state, -2, "is_file");

  {
    git_oid oid_from;
    bool is_new_worktree = false;
    if (git_oid_fromstr(&oid_from, checkout.prev) == 0) {
      is_new_worktree = git_oid_is_zero(&oid_from);
    }

    lua_pushboolean(config->state, is_new_worktree);
    lua_setfield(config->state, -2, "is_new_worktree");
  }

  lua_pushlightuserdata(config->state, config);
  lua_pushlightuserdata(config->state, &checkout);

  luaL_setfuncs(config->state, post_checkout_event_funcs, 2);

  for (int i = 0; i < config->on_checkout.n; i++) {
    int ref = config->on_checkout.refs[i];

    lua_rawgeti(config->state, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(config->state, -2);

    if (lua_pcall(config->state, 1, 0, 0) != LUA_OK) {
      const char *msg = lua_tostring(config->state, -1);
      trace("failed to call on_checkout: %s\n", msg);
    }
  }

  lua_pop(config->state, -1);
}
