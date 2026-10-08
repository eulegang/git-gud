#ifndef _G_INSTALL_H
#define _G_INSTALL_H

#include <stdbool.h>

typedef struct {
  bool force;
  char *src;
} install_opts;

int git_gud_install(install_opts opts);

#endif
