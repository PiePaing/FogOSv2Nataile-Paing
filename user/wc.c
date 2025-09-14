#include "types.h"
#include "user.h"

// Buffer size for reading files in chunks
#define BUF_SIZE 512

// Check if a character is part of a word (not whitespace)
int is_word_char(char c) {
  return !(c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

// Count lines, words, and characters in a file descriptor
// fd: file descriptor to read from (0 for stdin)
// name: filename to display in output
// show_lines, show_words, show_chars: flags for which counts to display
void wc_stats(int fd, char *name, int show_lines, int show_words, int show_chars) {
  int lines = 0, words = 0, chars = 0;
  int inword = 0;  // tracks if we're currently inside a word
  char buf[BUF_SIZE];
  int n;

  // Read file in chunks and count everything
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];
      chars++;  // count every character
      
      if (c == '\n') lines++;  // count newlines
      
      // Word counting logic
      if (is_word_char(c)) {
        if (!inword) {  // start of a new word
          words++;
          inword = 1;
        }
      } else {
        inword = 0;  // end of word
      }
    }
  }

  // Handle read errors
  if (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }

  // Display counts based on flags with descriptive labels
  int flag_used = show_lines || show_words || show_chars;
  
  if (flag_used) {
    // Show specific counts with labels
    if (show_lines) printf("Line count: %d\n", lines);
    if (show_words) printf("Word count: %d\n", words);
    if (show_chars) printf("Character count: %d\n", chars);
    printf("File name: %s\n", name);
  } else {
    // Default: show all counts with labels
    printf("Line count: %d\n", lines);
    printf("Word count: %d\n", words);
    printf("Character count: %d\n", chars);
    printf("File name: %s\n", name);
  }
}

int main(int argc, char *argv[]) {
  int show_lines = 0, show_words = 0, show_chars = 0;
  int file_start = 1;  // index where filenames start in argv
  int i;

  // Parse command line flags (-l, -w, -c)
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j]; j++) {
        if (argv[i][j] == 'l') show_lines = 1;
        else if (argv[i][j] == 'w') show_words = 1;
        else if (argv[i][j] == 'c') show_chars = 1;
      }
      file_start++;  // skip this flag argument
    }
  }

  // If no files specified, read from stdin
  if (argc == file_start) {
    wc_stats(0, "", show_lines, show_words, show_chars);
    exit(0);
  }

  // Process each file
  for (i = file_start; i < argc; i++) {
    int fd = open(argv[i], 0);
    if (fd < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      continue;  // skip to next file
    }
    wc_stats(fd, argv[i], show_lines, show_words, show_chars);
    close(fd);
  }
  exit(0);
}
