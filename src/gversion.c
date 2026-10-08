
#include "gversion.h"
#include "gitgud.h"
#include "util.h"
#include "version.h"

#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    char exec_path[PATH_MAX];
    strncpy(exec_path, path, PATH_MAX);
    strncat(exec_path, "hooks/git-gud", PATH_MAX);

    if (!access(exec_path, X_OK)) {
      char *env[] = {
          "GIT_GUD_HOOK_VERSION=1",
          0,
      };

      execle(exec_path, exec_path, "version", 0, env);
      die("failed to exec the hooked version\n");
    } else {
      printf("hook: -\n");
    }
  }

  git_repository_free(repo);
  return 0;
}
