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
void SimpleWaterfallBackend::processFFT(const fftw_complex *data, int size, DataInfo info)
{
	fftw_complex *row = (fftw_complex*) fftw_malloc(size * sizeof(fftw_complex)); 
	memcpy(row, data, size * sizeof(fftw_complex));
	rows_.push_back(row);
	time_.push_back(info.timeOffset);
	
	spectrumWidth_ = size;
	left_ = (int)(leftRatio_ * (float)size);
	right_ = left_ + (int)(widthRatio_ * (float)size);
	width_ = right_ - left_;
}


/**
 * Constructor.
 */
SimpleWaterfallBackend::SimpleWaterfallBackend(Ref<Output> output,
									  float left,
									  float width) :
	FFTBackend(),
	output_(output),
	leftRatio_(left),
	widthRatio_(width),
	markFreq_(20), markWidth_(2)
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
	
	float maxAmp2 = 1.0;
	
	time_t lastMark = time_[0].time.tv_sec / markFreq_;
	int mark = 0;
	
	for (int y = (rows_.size() - 1); y >= 0; y--) {
		// Pdb = 20 * log10(A / R)
		// A/R = 10 ^ (Pdb / 20)
		// R = A / 10 ^ (Pdb / 20)
		
		float r = maxAmp2 / (pow(10.0, (100.0 / 10.0)));
		maxAmp2 = 1.0;
		
		for (int x = left_; x < right_; x++) {
			float amp2 = (
				rows_[y][x][0] * rows_[y][x][0] +
				rows_[y][x][1] * rows_[y][x][1]
			);
			
			if (amp2 > maxAmp2) maxAmp2 = amp2;
			
			float db = 10.0 * log10(amp2 / r);
			
			//if (y == 10) cerr << amp << "\t" << db << endl;
			//if (y == 10) PRINT_EXPR(db);
			
			float s = db / 100.0;
			
			if (s <= 0.0) s = 0;
			if (s >= 1.0) s = 1;
			
			row[x - left_] = (png_byte)(255.0 * s);
		}

		if (((time_[y].time.tv_sec % markFreq_) == 0) &&
		    (time_[y].time.tv_sec / markFreq_) != lastMark) {
			mark = markWidth_;
			lastMark = (time_[y].time.tv_sec / markFreq_);
		}
		if (mark-- > 0) {
			for (int x = 0; x < width_; x++) {
				int d = (255 - row[x]) * 3 / 4;
				row[x] += d;
			}
		}
		
		if ((time_[y].time.tv_sec % 2) == 0) {
			for (int x = 0; x < 20; x++) {
				int d = (255 - row[x]) / 2;
				row[x] += d;
			}
		}
		
		writer.writeRow(row);
	}
	
	delete [] row;
	
	writer.end();
}


