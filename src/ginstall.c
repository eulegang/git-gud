#include "ginstall.h"
#include "gitgud.h"
#include "util.h"

#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>
#include <sys/fcntl.h>
#include <sys/syslimits.h>
#include <unistd.h>

static char BUF[PATH_MAX];

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
static int link_script(const char *src, const char *name, install_opts opts) {

  char *path = dirname_r(src, BUF);

  if (strlcat(path, "/", PATH_MAX) >= PATH_MAX) {
    return -1;
  }

  if (strlcat(path, name, PATH_MAX) >= PATH_MAX) {
    return -1;
  }

  int status = symlink(src, path);

  if (status == -1 && opts.force) {
    unlink(path);

    return symlink(src, path);
  }

  return status;
}

static int install_base(char *src, char *dst, install_opts opts) {
  int status = link(src, dst);

  if (status == -1 && opts.force) {
    unlink(dst);

    return link(src, dst);
  }

  return status;
}

enum FailedStage {
  FailedPostCheckout = 1,
  FailedEnd,
};

char *stages[] = {
    NULL,
    "post-checkout",
    NULL,
};

static char EXEC_PATH[PATH_MAX];
int git_gud_install(install_opts opts) {
  git_repository *repo;

  int status = git_repository_open_ext(&repo, CWD, 0, NULL);
  if (status == -1) {
    auto err = git_error_last();

    die("failed to open git repo: %s\n", err->message);
  }

  const char *path = git_repository_commondir(repo);
  if (strlcpy(EXEC_PATH, path, PATH_MAX) >= PATH_MAX) {
    return -1;
  }

  if (strlcat(EXEC_PATH, "hooks/git-gud", PATH_MAX) >= PATH_MAX) {
    return -1;
  }

  if (!access(EXEC_PATH, X_OK) && !opts.force) {
    die("refusing to install over existing version (use --force to ignore)\n");
  } else {
    unsigned int failed_statuses = 0;
    status = install_base(opts.src, EXEC_PATH, opts);
    if (status == -1) {
      die("failed to install git-gud\n");
    }

    for (unsigned int i = 1; i < FailedEnd; i++) {
      status = link_script(EXEC_PATH, stages[i], opts);
      if (status == -1) {
        failed_statuses |= (1ul << i);
      }
    }

    if (failed_statuses) {
      warn("failed to install the following scripts\n");
      for (unsigned int i = 1; i < FailedEnd; i++) {
        if ((failed_statuses & (1ul << i)) != 0) {
          warn("  - %s\n", stages[i]);
        }
      }

      die("");
    }
  }

  git_repository_free(repo);
  return 0;
}
