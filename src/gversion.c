
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
int git_gud_version(const char *directory) {
  if (getenv("GIT_GUD_HOOK_VERSION")) {
    printf("hook: %s\n", GIT_GUD_VERSION);
    return 0;
  }

  printf("cmd: %s\n", GIT_GUD_VERSION);

  if (directory) {
    if (!realpath(directory, EXEC_PATH)) {
      die("failed to resolve install directory: %s\n", directory);
    }
  } else {
    git_repository *repo;
    if (git_repository_open_ext(&repo, CWD, 0, NULL) == -1) {
      auto err = git_error_last();
      die("failed to open git repo: %s\n", err->message);
    }

    const char *path = git_repository_commondir(repo);
    if (strlcpy(EXEC_PATH, path, PATH_MAX) >= PATH_MAX) {
      die("failed to find the hooked version\n");
    }
    git_repository_free(repo);
  }

  if (strlcat(EXEC_PATH, directory ? "/git-gud" : "/hooks/git-gud",
              PATH_MAX) >= PATH_MAX) {
    die("failed to find the hooked version\n");
  }

  if (!access(EXEC_PATH, X_OK)) {
    char *env[] = {
        "GIT_GUD_HOOK_VERSION=1",
        0,
    };

    execle(EXEC_PATH, EXEC_PATH, "version", 0, env);
    die("failed to exec the hooked version\n");
  }

  printf("hook: -\n");
  return 0;
}
