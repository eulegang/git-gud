#include "hooks/hooks.h"
#include "hooks/commit-msg.h"
#include "hooks/post-checkout.h"
#include "gitgud.h"
#include <munit.h>

static int driver(int argc, char **argv) {
  (void)argc;
  (void)argv;
  return 7;
}

static int checkout(int argc, char **argv) {
  (void)argc;
  (void)argv;
  return 8;
}

static int commit_msg(int argc, char **argv) {
  (void)argc;
  (void)argv;
  return 9;
}

// Stand-ins for the entry points registered by hooks.c.
int git_gud_main(int argc, char **argv) { return driver(argc, argv); }
int hook_post_checkout(int argc, char **argv) { return checkout(argc, argv); }
int hook_commit_msg(int argc, char **argv) { return commit_msg(argc, argv); }

static const hook_t hooks[] = {
    {"git-gud", driver},
    {"post-checkout", checkout},
    {"commit-msg", commit_msg},
};

#define DETECT(argc, argv)                                                     \
  detect_hook(argc, argv, hooks, sizeof(hooks) / sizeof(hooks[0]))

static MunitResult test_driver(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = "/usr/local/bin/git-gud";
  char *argv[] = {path};
  const hook_t *hook = DETECT(1, argv);
  munit_assert_ptr_equal(hook, &hooks[0]);
  munit_assert_int(hook->run(1, argv), ==, 7);
  return MUNIT_OK;
}

static MunitResult test_hook(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = ".git/hooks/post-checkout";
  char *argv[] = {path};
  const hook_t *hook = DETECT(1, argv);
  munit_assert_ptr_equal(hook, &hooks[1]);
  munit_assert_int(hook->run(1, argv), ==, 8);
  return MUNIT_OK;
}

static MunitResult test_commit_msg(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = "commit-msg";
  char *argv[] = {path};
  const hook_t *hook = DETECT(1, argv);
  munit_assert_ptr_equal(hook, &hooks[2]);
  munit_assert_int(hook->run(1, argv), ==, 9);
  return MUNIT_OK;
}

static MunitResult test_registered(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = ".git/hooks/post-checkout";
  char *argv[] = {path};
  const hook_t *hook = detect_registered_hook(1, argv);
  munit_assert_not_null(hook);
  munit_assert_string_equal(hook->name, "post-checkout");
  munit_assert_int(hook->run(1, argv), ==, 8);
  return MUNIT_OK;
}

static MunitResult test_unknown(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = "unknown";
  char *argv[] = {path};
  munit_assert_null(DETECT(1, argv));
  return MUNIT_OK;
}

static MunitResult test_no_args(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  munit_assert_null(DETECT(0, NULL));
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/driver", test_driver, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hook", test_hook, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/commit-msg", test_commit_msg, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/registered", test_registered, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/unknown", test_unknown, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/no-args", test_no_args, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/detect", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
