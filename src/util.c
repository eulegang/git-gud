#include "util.h"
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>

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

char CONFIG_PATH[PATH_MAX] = {0};

const char *config_path() {
  if (CONFIG_PATH[0] == 0) {
    const char *init = getenv("GIT_GUD_INIT");

    if (init) {
      strncpy(CONFIG_PATH, init, PATH_MAX);
    } else {
      const char *home = getenv("HOME");
      if (!home) {
        die("Can not resolve default config file");
      }

      strncpy(CONFIG_PATH, home, PATH_MAX);
      strncat(CONFIG_PATH, "/.config/git-gud/init.lua", PATH_MAX);
    }
  }

  return CONFIG_PATH;
}
