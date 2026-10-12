#ifndef __CONFIG_H
#define __CONFIG_H

#include "git2/types.h"
#include <git2.h>

typedef struct config config_t;

config_t *config_init(const char *path, git_repository *repo,
                      git_repository *parent);
void config_deinit(config_t *);

const char *config_root(config_t *);

#include "on_post_checkout.h"

#endif
