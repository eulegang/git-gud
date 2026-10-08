
#include "gversion.h"
#include "gitgud.h"
#include "util.h"
#include "version.h"

#include <git2.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int git_gud_version() {
  printf("cmd: %s\n", GIT_GUD_VERSION);
  git_repository *repo;

  int status = git_repository_open_ext(&repo, CWD, 0, NULL);

  if (status == -1) {
    auto err = git_error_last();

    die("failed to open git repo: %s\n", err->message);
  }

  const char *path = git_repository_commondir(repo);
  char exec_path[PATH_MAX];
  strncpy(exec_path, path, PATH_MAX);
  strncat(exec_path, "hooks/git-gud", PATH_MAX);

  if (!access(exec_path, X_OK)) {
    printf("hook: todo\n");
  } else {
    printf("hook: -\n");
  }

  git_repository_free(repo);
  return 0;
}
