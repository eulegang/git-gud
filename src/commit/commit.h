#pragma once

#include <stdbool.h>
#include <stddef.h>

#define MESSAGE_TRAILER_ITER_INIT {0, NULL}

typedef struct {
  const char *subject;
  const char *description;
  const char *trailer;
} message_t;

typedef struct {
  size_t i;
  const char *buf;
} message_trailer_iter_t;

message_t *message_parse(const char *message);
void message_deinit(message_t *message);

bool message_trailer_next(message_t *message, message_trailer_iter_t *iter,
                          const char **key, const char **value);
