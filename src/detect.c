#include "detect.h"

#include <string.h>

const hook_t *detect_hook(int argc, char **argv, const hook_t *hooks,
                          size_t hook_count) {
  if (argc < 1 || !argv || !argv[0] || !hooks) {
    return NULL;
  }

  const char *slash = strrchr(argv[0], '/');
  const char *name = slash ? slash + 1 : argv[0];

  for (size_t i = 0; i < hook_count; i++) {
    if (strcmp(name, hooks[i].name) == 0) {
      return &hooks[i];
    }
  }

  return NULL;
}
