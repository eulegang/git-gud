#include "util.h"
#include <stdio.h>

FILE *stdtrace = NULL;

void init_trace_log(const char *path) {
  if (path) {
    stdtrace = fopen(path, "w");
  } else {
    stdtrace = stderr;
  }
}

void deinit_trace_log() {
  if (stdtrace) {
    fclose(stdtrace);
  }
}
