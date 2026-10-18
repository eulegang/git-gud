#pragma once

#include <git2.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#define REFS_MAX 64
#define STATUS_FAIL 0x1ul

struct lua_refs {
  int n;
  int refs[REFS_MAX];
};

struct config {
  lua_State *state;
  git_repository *repo;
  git_repository *parent;
  size_t status;

  struct lua_refs on_checkout;
  struct lua_refs on_commit_msg;
};

bool luaC_push_ref(struct lua_refs *, int);
