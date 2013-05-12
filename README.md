waterfall
=========

Radioastronomy utility.

Compilation
-----------

1. Clone the repository using (for instance): `git clone git://github.com/nnen/waterfall.git`.
2. Change directory to the `cppapp` subdirectory and build the cppapp library:
        
        $ cd cppapp
        $ autoreconf --install
        $ ./configure
        $ make 

2. Install the following libraries:
      - libfftw3 (http://www.fftw.org/download.html, `sudo apt-get install libfftw3-dev` on Ubuntu)
      - cfitsio (http://heasarc.gsfc.nasa.gov/fitsio/, `sudo apt-get install cfitsio-dev` on Ubuntu)
      - JACK (http://jackaudio.org/download)

3. In the `waterfall` directory, run `make`. The resulting binary, named `waterfall`, should appear in the project's root directory.
4. If anything goes wrong, please send me an email with the output at milikjan@fit.cvut.cz .

