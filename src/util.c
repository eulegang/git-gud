#include "util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>
#include <syslog.h>

static bool tracing_syslog = false;

void init_trace_syslog(void) {
  const char *env = getenv("GIT_GUD_TRACE_SYSLOG");
  if (env == NULL) {
    return;
  }

  openlog("git-gud", LOG_PID, LOG_USER);
  setlogmask(LOG_UPTO(LOG_INFO));
  if (strcmp(env, "DEBUG") == 0) {
    setlogmask(LOG_UPTO(LOG_DEBUG));
  } else if (strcmp(env, "WARN") == 0 || strcmp(env, "WARNING") == 0) {
    setlogmask(LOG_UPTO(LOG_WARNING));
  }

  tracing_syslog = true;
}

void gud_log(int priority, const char *format, ...) {
  if (!tracing_syslog) {
    return;
  }

  va_list args;
  va_start(args, format);
  vsyslog(priority, format, args);
  va_end(args);
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
