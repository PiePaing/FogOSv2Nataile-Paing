# wc — Count lines, words, characters, bytes, and max line length

## Usage

The `wc` program prints statistics about its input files. By default, it displays **all counts** with labels. You can use flags to select which counts to display:

- `-l` : **Line count** (number of `\n` newlines)
- `-w` : **Word count** (sequences of non-whitespace: not space, tab, carriage return, newline, vertical tab)
- `-c` : **Character count** (counts every character, including newlines and tabs; ASCII expected)
- `-b` : **Byte count** (same as `-c` on xv6)
- `-L` : **Max line length** (length of the longest line, including its newline)

If multiple flags are given, each selected count is displayed with a label.

### Examples

```sh
$ wc README.md
Line count: 42
Word count: 278
Character count: 1536
Byte count: 1536
Max line length: 62
File name: README.md

$ wc -l README.md
Line count: 42
File name: README.md

$ wc -L longline.txt
Max line length: 301
File name: longline.txt
```

---

## Testing

An automated test runner `/wc_test` is provided. It validates each flag across a set of sample files bundled in the image. **The tests ensure correctness for lines, words, characters, bytes, and max line length.**

### To run tests:

1. Boot the OS and open the shell.
2. Run:
   ```
   /wc_test
   ```

3. The test runner will output `PASS` or `FAIL` for each test case:
   ```
   PASS: wc -l empty.txt
   PASS: wc -w empty.txt
   ...
   wc_test summary: 35 pass, 0 fail
   ```

4. If all tests pass, your implementation is
