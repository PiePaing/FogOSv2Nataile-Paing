# wc — count lines, words, characters, bytes, and max line length

## Usage


With no flags, all counts are displayed with labels. With flags, only the
requested counts (plus the file name) are shown:

- `-l` : Line count (number of `'\n'` newlines)
- `-w` : Word count (sequences of non-whitespace: not space, tab, CR, LF, VT)
- `-c` : Character count (counts every byte read; ASCII expected on xv6)
- `-b` : Byte count (same as `-c` on xv6)
- `-L` : Max line length (***includes the newline*** in the length)

## Examples

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

$ wc -L tests/wc/longline.txt
Max line length: 301
File name: tests/wc/longline.txt


## Testing

An automated test runner `/wc_test` is provided. It validates each flag across a
set of sample files bundled in the image under `tests/wc`.

Run:



$ /wc_test
PASS: wc -l tests/wc/empty.txt
...
wc_test summary: 16 pass, 0 fail
