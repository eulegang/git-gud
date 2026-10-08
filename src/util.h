#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <unistd.h>

#define die(fmt, ...)                                                          \
  do {                                                                         \
    fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                           \
    exit(1);                                                                   \
  } while (0)

#endif
