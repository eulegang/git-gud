#pragma once

#include <stdbool.h>

typedef struct {
  bool force;
  char *src;
  const char *directory;
} install_opts;

int git_gud_install(install_opts opts);
