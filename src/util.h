#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define warn(fmt, ...)                                                         \
  do {                                                                         \
    (void)fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                     \
  } while (0)

#define die(fmt, ...)                                                          \
  do {                                                                         \
    (void)fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                     \
    exit(1);                                                                   \
  } while (0)

#define todo(fmt, ...)                                                         \
  do {                                                                         \
    (void)fprintf(stderr, "[%s:%d] TODO: " fmt, __FILE__,                      \
                  __LINE__ __VA_OPT__(, ) __VA_ARGS__);                        \
    abort();                                                                   \
  } while (0)

void init_trace_syslog(void);
void gud_log(int priority, const char *format, ...);
void deinit_trace_log(void);

const char *config_path();
