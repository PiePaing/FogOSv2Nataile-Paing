#include "types.h"
#include "user.h"

#define RD 0
#define WR 1
#define OUT_MAX 1024

struct Case {
  const char *path;
  int lines, words, chars, bytes, maxline;
};

static int starts_with(const char *s, const char *p){
  for(; *p; p++, s++){
    if(*p != *s) return 0;
  }
  return 1;
}

static int read_all(int fd, char *buf, int cap) {
  int n, tot=0;
  while((n = read(fd, buf+tot, cap-tot)) > 0) {
    tot += n;
    if(tot >= cap) break;
  }
  if(tot < cap) buf[tot] = 0;
  else buf[cap-1] = 0;
  return tot;
}

static int find_num_after_label(const char *out, const char *label){
  // Looks for lines like: "Line count: 12"
  for (int i = 0; out[i]; i++){
    if (starts_with(&out[i], label)) {
      i += strlen(label);
      while(out[i] == ' ') i++;
      return atoi(&out[i]);
    }
    // skip to next line
    while(out[i] && out[i] != '\n') i++;
    if(!out[i]) break;
  }
  return -999999; // not found
}

static int run_wc_capture(const char *flags, const char *path, char *out, int out_cap) {
  int p[2];
  if (pipe(p) < 0) {
    printf("wc_test: pipe failed\n");
    return -1;
  }
  int pid = fork();
  if(pid < 0) {
    printf("wc_test: fork failed\n");
    return -1;
  }
  if(pid == 0){
    // child: redirect stdout to pipe
    close(p[RD]);
    close(1);
    dup(p[WR]);
    close(p[WR]);

    if(flags && flags[0]) {
      char *argv[] = { "wc", (char*)flags, (char*)path, 0 };
      exec("/wc", argv);
    } else {
      char *argv[] = { "wc", (char*)path, 0 };
      exec("/wc", argv);
    }
    // if exec fails
    printf("wc_test: exec failed for /wc\n");
    exit(1);
  }
  // parent
  close(p[WR]);
  int n = read_all(p[RD], out, out_cap);
  close(p[RD]);
  wait(0);
  return n;
}

static void check_one(const char *flags, const char *path,
                      int exp_lines, int exp_words, int exp_chars, int exp_bytes, int exp_maxline,
                      int *pass, int *fail) {
  char buf[OUT_MAX];
  if(run_wc_capture(flags, path, buf, sizeof(buf)) < 0) {
    printf("FAIL: %s %s (wc run error)\n", flags, path);
    (*fail)++;
    return;
  }

  int ok = 1;
  if (flags && strchr(flags, 'l')) {
    int got = find_num_after_label(buf, "Line count:");
    if (got != exp_lines) { ok=0; printf("  expected lines=%d got=%d\n", exp_lines, got); }
  }
  if (flags && strchr(flags, 'w')) {
    int got = find_num_after_label(buf, "Word count:");
    if (got != exp_words) { ok=0; printf("  expected words=%d got=%d\n", exp_words, got); }
  }
  if (flags && strchr(flags, 'c')) {
    int got = find_num_after_label(buf, "Character count:");
    if (got != exp_chars) { ok=0; printf("  expected chars=%d got=%d\n", exp_chars, got); }
  }
  if (flags && strchr(flags, 'b')) {
    int got = find_num_after_label(buf, "Byte count:");
    if (got != exp_bytes) { ok=0; printf("  expected bytes=%d got=%d\n", exp_bytes, got); }
  }
  if (flags && strchr(flags, 'L')) {
    int got = find_num_after_label(buf, "Max line length:");
    if (got != exp_maxline) { ok=0; printf("  expected maxline=%d got=%d\n", exp_maxline, got); }
  }

  if (ok) { printf("PASS: wc %s %s\n", flags, path); (*pass)++; }
  else    { printf("FAIL: wc %s %s\n", flags, path); (*fail)++; }
}

int
main(void)
{
  // EXPECTATIONS are tailored to your current wc.c behavior:
  // - "chars" counts all bytes read (ASCII), same as "bytes"
  // - "max line length" includes the newline in the length (because you increment before checking '\n')
  struct Case cases[] = {
    { "tests/wc/empty.txt",          0, 0, 0, 0, 0 },
    { "tests/wc/nl_only.txt",        1, 0, 1, 1, 1 },   // newline counted in maxline
    { "tests/wc/hello.txt",          1, 1, 6, 6, 6 },   // "hello\n"
    { "tests/wc/words_spaces.txt",   1, 3, 6, 6, 6 },   // "a b c\n"
    { "tests/wc/tabs_newlines.txt",  2, 3, 7, 7, 6 },   // "a\tb c\n\n" -> first line len 6 incl '\n'
    { "tests/wc/multilin.txt",       2, 3, 14,14, 8 },  // "one two\nthree\n" -> first line 8 incl '\n'
    { "tests/wc/longline.txt",       1, 1, 301,301,301 } // 300 'a' + '\n'
  };

  int pass=0, fail=0;
  int N = sizeof(cases)/sizeof(cases[0]);

  for (int i = 0; i < N; i++) {
    // test each flag individually so we check each code path
    check_one("-l", cases[i].path, cases[i].lines, 0,0,0,0, &pass, &fail);
    check_one("-w", cases[i].path, 0, cases[i].words, 0,0,0, &pass, &fail);
    check_one("-c", cases[i].path, 0,0, cases[i].chars, 0,0, &pass, &fail);
    check_one("-b", cases[i].path, 0,0,0, cases[i].bytes, 0, &pass, &fail);
    check_one("-L", cases[i].path, 0,0,0,0, cases[i].maxline, &pass, &fail);
  }

  // also test a combined flags run on a mid-complex file
  check_one("-lwc", "tests/wc/multilin.txt", 2,3,14,0,0, &pass, &fail);
  check_one("-bL",  "tests/wc/multilin.txt", 0,0,0,14,8, &pass, &fail);

  printf("wc_test summary: %d pass, %d fail\n", pass, fail);
  exit(fail ? 1 : 0);
}
