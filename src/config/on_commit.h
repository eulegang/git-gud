#pragma once

#include "config.h"

typedef struct {
  const char *path;
  const char *content;
} commit_msg_t;

void config_on_commit_msg(config_t *config, commit_msg_t commit_msg);
