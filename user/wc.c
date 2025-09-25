#include "types.h"
#include "user.h"

#define BUF_SIZE 512

int is_word_char(char c) {
  return !(c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

void wc_stats(int fd, char *name, int show_lines, int show_words, int show_chars, int show_bytes, int show_max_line) {
  int lines = 0, words = 0, chars = 0, bytes = 0;
  int max_line_len = 0, current_line_len = 0, inword = 0;
  char buf[BUF_SIZE];
  int n;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    bytes += n;
    for (int i = 0; i < n; i++) {
      char c = buf[i];
      chars++;
      current_line_len++;
      if (c == '\n') {
        lines++;
        if (current_line_len > max_line_len)
          max_line_len = current_line_len;
        current_line_len = 0;
      }
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
  // Check for last line without newline
  if (current_line_len > 0 && current_line_len > max_line_len)
    max_line_len = current_line_len;

  if (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }

  int flag_used = show_lines || show_words || show_chars || show_bytes || show_max_line;
  if (flag_used) {
    if (show_lines) {
      printf("Line count: %d\t(Each '\\n' in the file is counted as a line)\n", lines);
    }
    if (show_words) {
      printf("Word count: %d\t(A word is a sequence of non-whitespace characters)\n", words);
    }
    if (show_chars) {
      printf("Character count: %d\t(Counts every character including newlines and tabs)\n", chars);
    }
    if (show_bytes) {
      printf("Byte count: %d\t(Total bytes read from the file)\n", bytes);
    }
    if (show_max_line) {
      printf("Max line length: %d\t(The longest line, including its newline, in characters)\n", max_line_len);
    }
    printf("File name: %s\n", name);
  } else {
    printf("Line count: %d\t(Each '\\n' in the file is counted as a line)\n", lines);
    printf("Word count: %d\t(A word is a sequence of non-whitespace characters)\n", words);
    printf("Character count: %d\t(Counts every character including newlines and tabs)\n", chars);
    printf("Byte count: %d\t(Total bytes read from the file)\n", bytes);
    printf("Max line length: %d\t(The longest line, including its newline, in characters)\n", max_line_len);
    printf("File name: %s\n", name);
  }
}

int main(int argc, char *argv[]) {
  int show_lines = 0, show_words = 0, show_chars = 0, show_bytes = 0, show_max_line = 0, file_start = 1;
  int i;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j]; j++) {
        if (argv[i][j] == 'l') show_lines = 1;
        else if (argv[i][j] == 'w') show_words = 1;
        else if (argv[i][j] == 'c') show_chars = 1;
        else if (argv[i][j] == 'b') show_bytes = 1;
        else if (argv[i][j] == 'L') show_max_line = 1;
      }
      file_start++;
    }
  }
  if (argc == file_start) {
    wc_stats(0, "", show_lines, show_words, show_chars, show_bytes, show_max_line);
    exit(0);
  }
  for (i = file_start; i < argc; i++) {
    int fd = open(argv[i], 0);
    if (fd < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      continue;
    }
    wc_stats(fd, argv[i], show_lines, show_words, show_chars, show_bytes, show_max_line);
    close(fd);
  }
  exit(0);
}
