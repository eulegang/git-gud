#include "commit.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t size;
  size_t n_trailers;
} header_t;

static bool skip_to_content(const char **it) {
  const char *snapshot = *it;
  while (**it) {
    switch (**it) {
    case '\n':
    case '\r':
      snapshot = (*it) + 1;
      (*it)++;
      break;

    case '\t':
    case ' ':
      (*it)++;
      break;

    case '#': {
      while (**it) {
        if (**it == '\n' || **it == '\r')
          break;

        if (strncmp(*it, " >8 ", 4) == 0)
          return false;

        (*it)++;
      }

      break;
    }

    default:
      *it = snapshot;
      return true;
    }
  }

  return false;
}

static bool copy_line(char **buf, const char **content) {
  const char *snapshot = *content;
  while (**content) {
    if (**content == '\n' || **content == '#' || **content == '\r') {
      memcpy(*buf, snapshot, *content - snapshot);
      *buf += *content - snapshot;
      return true;
    }

    (*content)++;
  }

  return false;
}

static bool copy_trailer(char **buf, const char **content) {
  const char *snapshot = *content;

  while (**content) {
    if (**content == '\n' || **content == '#' || **content == '\r') {
      return false;
    }

    if (**content == ':' || **content == '=') {
      memcpy(*buf, snapshot, *content - snapshot);
      *buf += *content - snapshot;
      **buf = 0;
      (*buf)++;
      (*content)++;
      break;
    }

    (*content)++;
  }

  while (**content == ' ') {
    (*content)++;
  }

  snapshot = *content;

  while (**content) {
    if (**content == '\n' || **content == '#' || **content == '\r') {
      memcpy(*buf, snapshot, *content - snapshot);
      *buf += *content - snapshot;
      **buf = 0;
      (*buf)++;
      return true;
    }

    (*content)++;
  }

  return false;
}

static bool is_trailer(const char **buf) {
  bool ret = false;

  while (**buf) {
    if ((**buf) == '\n' || (**buf) == '\r') {
      (*buf)++;
      break;
    }

    if ((**buf) == ':' || (**buf) == '=') {
      ret = true;
    }

    (*buf)++;
  }

  return ret;
}

static void terminate(char **buf) {
  **buf = 0;
  (*buf)++;
}

static void line_end(char **buf) {
  **buf = '\n';
  (*buf)++;
}

static bool message_parse_init(header_t *header, const char *message) {
  message_t *msg = (message_t *)(header + 1);
  char *buf = ((char *)header) + sizeof(header_t) + sizeof(message_t);

  msg->subject = NULL;
  msg->description = NULL;
  msg->trailer = NULL;
  header->n_trailers = 0;

  const char *it = message;

  if (!skip_to_content(&it))
    return true;

  const char *subject = buf;
  copy_line(&buf, &it);
  terminate(&buf);

  msg->subject = subject;

  const char *body = buf;
  const char *trailer_start = NULL;

  const char *snapshot = it;
  while (true) {
    if (!skip_to_content(&it)) {
      break;
    }

    const char *line_start = it;
    if (is_trailer(&it)) {
      if (!trailer_start) {
        trailer_start = line_start;
      }
    } else {
      trailer_start = NULL;
    }
  }

  it = snapshot;

  if (!trailer_start) {
    while (true) {
      if (!skip_to_content(&it)) {
        break;
      }

      if (!copy_line(&buf, &it))
        break;

      line_end(&buf);
    }

    terminate(&buf);

    msg->description = body;
  } else {
    while (it < trailer_start) {
      if (!skip_to_content(&it)) {
        break;
      }

      if (it >= trailer_start) {
        break;
      }

      if (!copy_line(&buf, &it))
        break;

      line_end(&buf);
    }

    terminate(&buf);
    msg->description = body;
    msg->trailer = buf;

    it = trailer_start;
    while (true) {
      if (!skip_to_content(&it)) {
        break;
      }

      if (!copy_trailer(&buf, &it)) {
        break;
      }

      header->n_trailers++;
    }
  }

  return true;
}

message_t *message_parse(const char *message) {
  size_t len = strlen(message);

  header_t *header = malloc(sizeof(header_t) + sizeof(message_t) + 2 * len);
  if (!header) {
    return NULL;
  }

  header->size = len;

  if (!message_parse_init(header, message)) {
    free(header);
    return NULL;
  };

  return (message_t *)(header + 1);
}

void message_deinit(message_t *message) {
  if (message) {
    free(((header_t *)message) - 1);
  }
}

bool message_trailer_next(message_t *message, message_trailer_iter_t *iter,
                          const char **key, const char **value) {
  assert(iter != NULL);
  assert(message != NULL);
  assert(key != NULL);
  assert(value != NULL);

  header_t *header = ((header_t *)message) - 1;

  if (iter->i >= header->n_trailers) {
    return false;
  }

  if (iter->i == 0) {
    iter->buf = message->trailer;
  }

  *key = iter->buf;

  const char *it = iter->buf;

  while (*it)
    it++;

  it++;

  *value = it;

  while (*it)
    it++;

  it++;

  iter->buf = it;
  iter->i++;

  return true;
}
