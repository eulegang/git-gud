

#include "internal.h"

bool luaC_push_ref(struct lua_refs *refs, int ref) {
  if (refs->n >= REFS_MAX) {
    return false;
  }

  refs->refs[refs->n++] = ref;

  return true;
}
