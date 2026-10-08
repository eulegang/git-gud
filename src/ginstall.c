#include "ginstall.h"
#include "gitgud.h"
#include "util.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>
#include <sys/fcntl.h>
#include <unistd.h>

static int install_base(char *src, char *dst) {
  int fin = open(src, O_RDONLY);
  int fout = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0755);

  char buf[4096];
  int written = 0;

  while (true) {
    ssize_t len = read(fin, buf, 4096);
    ssize_t off = 0;

    if (len == -1) {
      return -1;
    }

    if (len == 0) {
      break;
    }

    while (off < len) {
      ssize_t w = write(fout, buf + off, (size_t)(len - off));

      if (w == -1) {
        return -1;
      }

      off += w;
      written += w;
    }
  }

  return written;
}

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
    status = install_base(opts.src, exec_path);
    if (status == -1) {
      die("failed to install git-gud\n");
    }
  }

  git_repository_free(repo);
  return 0;
}
