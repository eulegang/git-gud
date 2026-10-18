#include "commit.h"
#include <munit.h>
#include <string.h>

static MunitResult test_subject(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  message_t *parsed = message_parse("Fix issue\n\nDetails\nMore details\n");

  munit_assert(parsed != NULL);
  munit_assert(parsed->subject != NULL);
  munit_assert(parsed->description != NULL);
  munit_assert_string_equal(parsed->subject, "Fix issue");
  munit_assert_string_equal(parsed->description, "Details\nMore details\n");

  message_deinit(parsed);
  return MUNIT_OK;
}

static MunitResult test_comment_subject(const MunitParameter params[],
                                        void *data) {
  (void)params;
  (void)data;
  message_t *parsed =
      message_parse("# Put subject down below this comment\n\nFix "
                    "issue\n\nDetails\nMore details\n");

  munit_assert(parsed != NULL);
  munit_assert(parsed->subject != NULL);
  munit_assert(parsed->description != NULL);
  munit_assert_string_equal(parsed->subject, "Fix issue");
  munit_assert_string_equal(parsed->description, "Details\nMore details\n");

  message_deinit(parsed);
  return MUNIT_OK;
}

static MunitResult test_comment_description(const MunitParameter params[],
                                            void *data) {
  (void)params;
  (void)data;
  message_t *parsed =
      message_parse("# Put subject down below this comment\n\nFix "
                    "issue\n\nDetails\n#hello \nAnd More Details\n");

  munit_assert(parsed != NULL);
  munit_assert(parsed->subject != NULL);
  munit_assert(parsed->description != NULL);
  munit_assert_string_equal(parsed->subject, "Fix issue");
  munit_assert_string_equal(parsed->description, "Details\nAnd More Details\n");

  message_deinit(parsed);
  return MUNIT_OK;
}

static MunitResult test_trailer(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  message_t *parsed =
      message_parse("Fix issue\n"
                    "\n"
                    "Details\n"
                    "\n"
                    "Closes: !13\n"
                    "Co-authored-by: Jane Doe <jane.doe@example.com>\n");

  munit_assert(parsed != NULL);
  munit_assert(parsed->subject != NULL);
  munit_assert(parsed->description != NULL);
  munit_assert(parsed->trailer != NULL);
  munit_assert_string_equal(parsed->subject, "Fix issue");
  munit_assert_string_equal(parsed->description, "Details\n");

  message_trailer_iter_t iter = MESSAGE_TRAILER_ITER_INIT;
  const char *key = NULL, *value = NULL;
  size_t i = 0;

  const char *keys[] = {"Closes", "Co-authored-by", NULL};
  const char *values[] = {"!13", "Jane Doe <jane.doe@example.com>", NULL};

  while (message_trailer_next(parsed, &iter, &key, &value)) {
    munit_assert(key != NULL);
    munit_assert(value != NULL);

    munit_assert_string_equal(key, keys[i]);
    munit_assert_string_equal(value, values[i]);

    i++;
  }

  message_deinit(parsed);
  return MUNIT_OK;
}

static MunitResult test_scissors(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;
  message_t *parsed =
      message_parse("subject\n"
                    "\n"
                    "body\n"
                    "line\n"
                    "# -------------- >8 ----------\n"
                    "issue\n\nDetails\n#hello \nAnd More Details\n");

  munit_assert(parsed != NULL);
  munit_assert(parsed->subject != NULL);
  munit_assert(parsed->description != NULL);
  munit_assert_string_equal(parsed->subject, "subject");
  munit_assert_string_equal(parsed->description, "body\nline\n");

  message_deinit(parsed);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/subject", test_subject, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/comment-subject", test_comment_subject, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/comment-description", test_comment_description, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/scissors", test_scissors, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/trailer", test_trailer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {"/commit", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
