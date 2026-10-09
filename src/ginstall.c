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

static int link_script(const char *src, const char *name, install_opts opts) {
  char buf[PATH_MAX];

  char *path = dirname_r(src, buf);

  strncat(path, "/", PATH_MAX);
  strncat(path, name, PATH_MAX);

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

int git_gud_install(install_opts opts) {
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

  if (!access(exec_path, X_OK) && !opts.force) {
    die("refusing to install over existing version (use --force to ignore)\n");
  } else {
    int failed_statuses = 0;
    status = install_base(opts.src, exec_path, opts);
    if (status == -1) {
      die("failed to install git-gud\n");
    }

    for (int i = 1; i < FailedEnd; i++) {
      status = link_script(exec_path, stages[i], opts);
      if (status == -1) {
        failed_statuses |= 1 << i;
      }
    }

    if (failed_statuses) {
      warn("failed to install the following scripts\n");
      for (int i = 1; i < FailedEnd; i++) {
        if ((failed_statuses & (1 << i)) != 0) {
          warn("  - %s\n", stages[i]);
        }
      }

      die("");
    }
  }

  git_repository_free(repo);
  return 0;
}
