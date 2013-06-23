waterfall
=========

Radioastronomy utility. For more information, see [the MLAB wiki](http://wiki.mlab.cz/doku.php?id=en:programming_tasks#open-source_meteor_detection_toolkit).

Compilation
-----------

1. Install the following libraries:
      - libfftw3 (http://www.fftw.org/download.html, `sudo apt-get install libfftw3-dev` on Ubuntu)
      - cfitsio (http://heasarc.gsfc.nasa.gov/fitsio/, `sudo apt-get install cfitsio-dev` on Ubuntu)
      - JACK (http://jackaudio.org/download)

2. Clone the repository using (for instance):
   `git clone git://github.com/nnen/waterfall.git`.
3. Checkout and build the `cppapp` submodule:
        
        $ git submodule init
        $ git submodule update
        $ cd cppapp
        $ make

4. Change directory to the `cppapp` subdirectory and build the cppapp library:
        
        $ cd cppapp
        $ autoreconf --install
        $ ./configure
        $ make 

5. In the `waterfall` directory, run `make`. The resulting binary, named
   `waterfall`, should appear in the project's root directory.
6. If anything goes wrong, please send me an email with the output at
   milikjan@fit.cvut.cz .

