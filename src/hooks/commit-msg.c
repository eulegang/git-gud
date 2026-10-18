#include "commit-msg.h"

#include "../gitgud.h"
#include "../mbytes.h"
#include "../util.h"

#include "../config/config.h"
#include "git2/repository.h"

#include <stdio.h>

int hook_commit_msg(int argc, char **argv) {
  if (argc != 2) {
    die("invalid invocation");
  }

  git_libgit2_init();

  commit_msg_t data;

  data.path = argv[1];

  auto bytes = mbytes_init(argv[1]);
  data.content = bytes.data;

  git_repository *repo, *parent;
  int status = git_repository_open_ext(&repo, CWD, 0, NULL);
  if (status == -1) {
    auto err = git_error_last();

    die("failed to open git repo: %s\n", err->message);
  }

  status =
      git_repository_open_ext(&parent, git_repository_commondir(repo), 0, NULL);
  if (status == -1) {
    auto err = git_error_last();

    die("failed to open parent git repo: %s\n", err->message);
  }

  config_t *config = config_init(config_path(), repo, parent);
  if (!config) {
    die("failed to initialize configuration\n");
  }

  config_on_commit_msg(config, data);

  config_deinit(config);

  git_libgit2_shutdown();

  return 0;
}
