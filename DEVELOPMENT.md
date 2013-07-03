Development
===========

Version Number Change
---------------------

Use the bash script `$ change_version MAJOR MINOR [REV]` to:

* In `Doxyfile`, change the settings `PROJECT_NUMBER = x.x`.
* In `Makefile`, change the variable `VERSION = x.x`.
* In `src/header.h`, change the preprocessor symbol
  `#define PACKAGE_VERSION "x.x"` (name uses autotools convention).

