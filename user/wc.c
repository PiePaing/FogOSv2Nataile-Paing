#include "types.h"
#include "user.h"

#define BUF_SIZE 512

int is_word_char(char c) {
  return !(c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

void wc_stats(int fd, char *name, int show_lines, int show_words, int show_chars) {
  int lines = 0, words = 0, chars = 0;
  int inword = 0;
  char buf[BUF_SIZE];
  int n;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];
      chars++;
      if (c == '\n') lines++;
      if (is_word_char(c)) {
        if (!inword) {
          words++;
          inword = 1;
        }
      } else {
        inword = 0;
      }
    }
  }

  if (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }

  int flag_used = show_lines || show_words || show_chars;
  if (show_lines) printf("%d ", lines);
  if (show_words) printf("%d ", words);
  if (show_chars) printf("%d ", chars);
  if (!flag_used) printf("%d %d %d ", lines, words, chars);

  printf("%s\n", name);
}

int main(int argc, char *argv[]) {
  int show_lines = 0, show_words = 0, show_chars = 0;
  int file_start = 1;
  int i;

  // Parse flags
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j]; j++) {
        if (argv[i][j] == 'l') show_lines = 1;
        else if (argv[i][j] == 'w') show_words = 1;
        else if (argv[i][j] == 'c') show_chars = 1;
      }
      file_start++;
    }
  }

  if (argc == file_start) { // No files, read stdin
    wc_stats(0, "", show_lines, show_words, show_chars);
    exit(0);
  }

  for (i = file_start; i < argc; i++) {
    int fd = open(argv[i], 0);
    if (fd < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      continue;
    }
    wc_stats(fd, argv[i], show_lines, show_words, show_chars);
    close(fd);
  }
  exit(0);
}
