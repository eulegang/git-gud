#pragma once

#include <stddef.h>
typedef struct {
  char *data;
  size_t len;
} mbytes_t;

mbytes_t mbytes_init(const char *);

void mbytes_deinit(mbytes_t);
