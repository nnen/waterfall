waterfall
=========

Radioastronomy utility.

Compilation
-----------

1. Clone the repository using (for instance): `git clone git://github.com/nnen/waterfall.git`.
2. Checkout and build the cppapp library (in the `waterfall` directory):

        git submodule init
        git submodule update
        cd cppapp
        make

3. Install the following libraries:
      - libpng (http://www.libpng.org/pub/png/libpng.html)
      - libfftw3 (http://www.fftw.org/download.html)
      - cfitsio (http://heasarc.gsfc.nasa.gov/fitsio/)

4. In the `waterfall` directory, run `make`. The resulting binary, named `waterfall`, should appear in the project's root directory.
5. If anything goes wrong, please send me an email with the output at milikjan@fit.cvut.cz .
