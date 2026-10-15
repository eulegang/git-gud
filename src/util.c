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
  if (stdtrace && stdtrace != stderr) {
    (void)fclose(stdtrace);
  }
}

char CONFIG_PATH[PATH_MAX] = {0};

const char *config_path() {
  if (CONFIG_PATH[0] == 0) {
    const char *init = getenv("GIT_GUD_INIT");

    if (init) {
      if (strlcpy(CONFIG_PATH, init, PATH_MAX) >= PATH_MAX) {
        return NULL;
      }
    } else {
      const char *home = getenv("HOME");
      if (!home) {
        die("Can not resolve default config file");
      }

      char candidate[PATH_MAX] = {0};
      snprintf(candidate, PATH_MAX, "%s/.config/git/gud.lua", home);

      if (access(candidate, F_OK) == 0) {
        if (strlcpy(CONFIG_PATH, candidate, PATH_MAX) >= PATH_MAX) {
          return NULL;
        }
      } else {
        if (snprintf(CONFIG_PATH, PATH_MAX, "%s/.config/git-gud/init.lua",
                     home) >= PATH_MAX) {
          return NULL;
        }
      }
    }
  }

  return CONFIG_PATH;
}
