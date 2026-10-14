#include "post-checkout.h"

#include "../gitgud.h"
#include "../util.h"

#include "../config/config.h"
#include "git2/repository.h"

#include <stdio.h>

int hook_post_checkout(int argc, char **argv) {
  if (argc != 4) {
    die("invalid invocation");
  }

  git_libgit2_init();

  post_checkout_t data;
  data.prev = argv[1];
  data.next = argv[2];
  data.checkout = *argv[3] == '0' ? CheckoutFile : CheckoutBranch;

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

  config_post_checkout(config, data);

  git_repository_free(parent);

  git_repository_free(repo);

  config_deinit(config);

  git_libgit2_shutdown();

  return 0;
}
