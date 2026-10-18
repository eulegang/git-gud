
#include "mbytes.h"
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

mbytes_t mbytes_init(const char *path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    return (mbytes_t){NULL, 0};
  }

  struct stat st;
  if (fstat(fd, &st) < 0 || st.st_size == 0) {
    close(fd);
    return (mbytes_t){NULL, 0};
  }

  size_t len = (size_t)st.st_size;
  char *bytes = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (bytes == MAP_FAILED) {
    return (mbytes_t){NULL, 0};
  }

  return (mbytes_t){bytes, len};
}

void mbytes_deinit(mbytes_t bytes) { munmap(bytes.data, bytes.len); }
