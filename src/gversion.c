
#include "gversion.h"
#include "gitgud.h"
#include "util.h"
#include "version.h"

#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char EXEC_PATH[PATH_MAX];
int git_gud_version(void) {
  char *hook = getenv("GIT_GUD_HOOK_VERSION");

  if (hook) {
    printf("hook: %s\n", GIT_GUD_VERSION);
  } else {
    printf("cmd: %s\n", GIT_GUD_VERSION);
  }
  git_repository *repo;

  int status = git_repository_open_ext(&repo, CWD, 0, NULL);

  if (status == -1) {
    auto err = git_error_last();

    die("failed to open git repo: %s\n", err->message);
  }

  if (!hook) {
    const char *path = git_repository_commondir(repo);
    if (strlcpy(EXEC_PATH, path, PATH_MAX) >= PATH_MAX) {
      die("failed to exec the hooked version\n");
    }

    if (strlcat(EXEC_PATH, "hooks/git-gud", PATH_MAX) >= PATH_MAX) {
      die("failed to exec the hooked version\n");
    }

    if (!access(EXEC_PATH, X_OK)) {
      char *env[] = {
          "GIT_GUD_HOOK_VERSION=1",
          0,
      };

      execle(EXEC_PATH, EXEC_PATH, "version", 0, env);
      die("failed to exec the hooked version\n");
    } else {
      printf("hook: -\n");
    }
  }

  git_repository_free(repo);
  return 0;
}
