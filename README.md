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
      - libpng (http://www.libpng.org/pub/png/libpng.html, `sudo apt-get install libpng-dev` on Ubuntu)
      - libfftw3 (http://www.fftw.org/download.html, `sudo apt-get install libfftw3-dev` on Ubuntu)
      - cfitsio (http://heasarc.gsfc.nasa.gov/fitsio/, `sudo apt-get install cfitsio-dev` on Ubuntu)

4. In the `waterfall` directory, run `make`. The resulting binary, named `waterfall`, should appear in the project's root directory.
5. If anything goes wrong, please send me an email with the output at milikjan@fit.cvut.cz .
