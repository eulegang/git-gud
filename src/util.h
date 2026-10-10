#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern FILE *stdtrace;

#define warn(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                           \
  } while (0)

#define trace(fmt, ...)                                                        \
  do {                                                                         \
    if (stdtrace)                                                              \
      fprintf(stdtrace, fmt __VA_OPT__(, ) __VA_ARGS__);                       \
  } while (0)

#define die(fmt, ...)                                                          \
  do {                                                                         \
    fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                           \
    exit(1);                                                                   \
  } while (0)

#define todo(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[%s:%d] TODO: " fmt, __FILE__,                            \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__);                              \
    abort();                                                                   \
  } while (0)

#define when_tracing if (stdtrace)

void init_trace_log(const char *path);
void deinit_trace_log();

const char *config_path();

#endif
