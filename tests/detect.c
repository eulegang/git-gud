#include "detect.h"
#include <munit.h>

static MunitResult test_driver(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = "/usr/local/bin/git-gud";
  char *argv[] = {path};
  munit_assert_int(detect_cmd(1, argv), ==, CMD_GIT_GUD);
  return MUNIT_OK;
}

static MunitResult test_hook(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = ".git/hooks/post-checkout";
  char *argv[] = {path};
  munit_assert_int(detect_cmd(1, argv), ==, CMD_POST_CHECKOUT);
  return MUNIT_OK;
}

static MunitResult test_unknown(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  char path[] = "unknown";
  char *argv[] = {path};
  munit_assert_int(detect_cmd(1, argv), ==, CMD_INVALID);
  return MUNIT_OK;
}

static MunitResult test_no_args(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  munit_assert_int(detect_cmd(0, NULL), ==, CMD_INVALID);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/driver", test_driver, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hook", test_hook, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/unknown", test_unknown, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/no-args", test_no_args, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/detect", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
