/**
 * \file   SimpleWaterfallBackend.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 * 
 * \brief Implementation file for the SimpleWaterfallBackend class.
 */

#include "SimpleWaterfallBackend.h"

#include "PNGWriter.h"
#include <cmath>


/**
 *
 */
void SimpleWaterfallBackend::processFFT(fftw_complex *data, int size)
{
	fftw_complex *row = (fftw_complex*) fftw_malloc(size * sizeof(fftw_complex)); 
	memcpy(row, data, size * sizeof(fftw_complex));
	rows_.push_back(row);
	
	width_ = size;
}


/**
 * Constructor.
 */
SimpleWaterfallBackend::SimpleWaterfallBackend(Ref<Output> output) :
	FFTBackend(),
	output_(output)
{
}


/**
 * Destructor
 */
SimpleWaterfallBackend::~SimpleWaterfallBackend()
{
	FOR_EACH(rows_, row) {
		fftw_free(*row);
	}
}


/**
 *
 */
void SimpleWaterfallBackend::endStream()
{
	PNGWriter writer(output_);
	writer.start(width_, rows_.size());

   	png_bytep row = new png_byte[width_ * sizeof(png_byte)];
	
	float maxAmp = 1.0;
	
	for (int y = 0; y < (int)rows_.size(); y++) {
		// Pdb = 20 * log10(A / R)
		// A/R = 10 ^ (Pdb / 20)
		// R = A / 10 ^ (Pdb / 20)
		
		float r = maxAmp / (pow(10.0, (100.0 / 20.0)));
		maxAmp = 1.0;
		
		for (int x = 0; x < width_; x++) {
			float amp = sqrt(
				rows_[y][x][0] * rows_[y][x][0] +
				rows_[y][x][1] * rows_[y][x][1]
			);
			
			if (amp > maxAmp) maxAmp = amp;
			
			float db = 20.0 * log10(amp / r);
			
			//if (y == 10) cerr << amp << "\t" << db << endl;
			if (y == 10) PRINT_EXPR(db);
			
			float s = db / 100.0;
			
			//float s = log(
			//	rows_[y][x][0] * rows_[y][x][0] +
			//	rows_[y][x][1] * rows_[y][x][1]
			//);
			
			//if (y == 10) cerr << s << endl;
			//s = s / 20.0;
			
			if (s <= 0.0) s = 0;
			if (s >= 1.0) s = 1;
			
			row[x] = (png_byte)(255.0 * s);
			
			//if (row[x] > 0) cerr << "row[x] = " << row[x] << endl;
		}
		
		for (int x = 0; x < 10; x++) {
			if (((y / 20) % 2) == 0)
				row[x] = 255;
		}
		
		writer.writeRow(row);
	}
	
	delete [] row;
	
	writer.end();
}


