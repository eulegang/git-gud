#pragma once

#include <stddef.h>

typedef struct {
  const char *name;
  int (*run)(int argc, char **argv);
} hook_t;

const hook_t *detect_hook(int argc, char **argv, const hook_t *hooks,
                          size_t hook_count);
const hook_t *detect_registered_hook(int argc, char **argv);
