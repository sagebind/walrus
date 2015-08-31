# Walrus
A lightweight compiler for the Decaf programming language.

[![Build Status](https://img.shields.io/travis/coderstephen/walrus.svg)](https://travis-ci.org/coderstephen/walrus)

## Compiling the source
You will need a GNU compiler in your `$PATH` installed to compile. [Cygwin](http://cygwin.com) or [MinGW-w64](http://mingw-w64.sourceforge.net) should be sufficient for compiling on Windows machines, as Walrus is POSIX-compatible and uses minimal library functions.

All builds and tests are done using [GNU make](http://www.gnu.org/software/make/). To compile all sources and produce an executable, simply run:

```sh
make
```

That's it. No crazy stuff.

## Running tests
You can run all tests by running:

```sh
make test
```

Sets of input files will be compared to expected output files to find any errors with scanning. You can also run individual tests for the scanner and parser separately:

```sh
make test-scanner
```

```sh
make test-parser
```

## Usage
To compile a Decaf program, pass the source code files to Walrus:

```sh
bin/walrus inputfile1 inputfile2...
```

To just run the scanner, set the `-s` option. You can also pass the `-T` option along with `-s` to print out the scanned tokens to STDOUT for testing and debugging purposes.

Below are all command line options (also accessible with `--help`):

* `--help`: Displays the help message
* `--debug`: Outputs debugging information
* `-p`: Scan and parse, but do not analyze
* `-s`: Scan only; do not parse or compile
* `-T`, `--print-tokens`: Print out tokens as they are scanned
