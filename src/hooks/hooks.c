#include "hooks.h"

#include <string.h>

#include "commit-msg.h"
#include "post-checkout.h"
#include "../gitgud.h"

static const hook_t registered_hooks[] = {
    {"git-gud", git_gud_main},
    {"post-checkout", hook_post_checkout},
    {"commit-msg", hook_commit_msg},
};

const hook_t *detect_registered_hook(int argc, char **argv) {
  return detect_hook(argc, argv, registered_hooks,
                     sizeof(registered_hooks) / sizeof(registered_hooks[0]));
}

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
