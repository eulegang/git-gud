#include "util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>
#include <sys/syslog.h>
#include <syslog.h>

static bool tracing_syslog = false;

void init_trace_syslog(void) {
  openlog("git-gud", LOG_PID, LOG_USER);

  const char *env = getenv("GIT_GUD_TRACE_SYSLOG");

  setlogmask(LOG_UPTO(LOG_INFO));
  if (env == NULL) {
  } else if (strcmp(env, "DEBUG") == 0) {
    setlogmask(LOG_UPTO(LOG_DEBUG));
  } else if (strcmp(env, "WARN") == 0 || strcmp(env, "WARNING") == 0) {
    setlogmask(LOG_UPTO(LOG_WARNING));
  }

  tracing_syslog = true;
}

void deinit_trace_log(void) {
  if (tracing_syslog) {
    closelog();
  }
  tracing_syslog = false;
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
