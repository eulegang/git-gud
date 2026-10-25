#include "util.h"
#include <munit.h>
#include <stdarg.h>
#include <syslog.h>

static int opened, closed, logged, mask;

void openlog(const char *ident, int option, int facility) {
  (void)ident;
  (void)option;
  (void)facility;
  opened++;
}

void closelog(void) { closed++; }

int setlogmask(int new_mask) {
  mask = new_mask;
  return 0;
}

void vsyslog(int priority, const char *format, va_list args) {
  (void)priority;
  (void)format;
  (void)args;
  logged++;
}

static MunitResult test_disabled(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  unsetenv("GIT_GUD_TRACE_SYSLOG");
  opened = closed = logged = mask = 0;

  init_trace_syslog();
  gud_log(LOG_ERR, "disabled");
  deinit_trace_log();

  munit_assert_int(opened, ==, 0);
  munit_assert_int(closed, ==, 0);
  munit_assert_int(logged, ==, 0);
  munit_assert_int(mask, ==, 0);
  return MUNIT_OK;
}

static MunitResult test_enabled(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  opened = closed = logged = mask = 0;

  setenv("GIT_GUD_TRACE_SYSLOG", "DEBUG", 1);
  init_trace_syslog();
  munit_assert_int(mask, ==, LOG_UPTO(LOG_DEBUG));
  gud_log(LOG_DEBUG, "enabled");
  deinit_trace_log();
  gud_log(LOG_ERR, "disabled again");
  munit_assert_int(opened, ==, 1);
  munit_assert_int(closed, ==, 1);
  munit_assert_int(logged, ==, 1);

  setenv("GIT_GUD_TRACE_SYSLOG", "WARN", 1);
  init_trace_syslog();
  munit_assert_int(mask, ==, LOG_UPTO(LOG_WARNING));
  deinit_trace_log();

  setenv("GIT_GUD_TRACE_SYSLOG", "", 1);
  init_trace_syslog();
  munit_assert_int(mask, ==, LOG_UPTO(LOG_INFO));
  deinit_trace_log();
  unsetenv("GIT_GUD_TRACE_SYSLOG");
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/disabled", test_disabled, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/enabled", test_enabled, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/syslog", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
