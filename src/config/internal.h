#pragma once

#include <git2.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#define REFS_MAX 64

struct lua_refs {
  int n;
  int refs[REFS_MAX];
};

struct config {
  lua_State *state;
  git_repository *repo;
  git_repository *parent;

  struct lua_refs on_checkout;
};
