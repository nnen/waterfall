/**
 * \file   FFTBackend.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 * 
 * \brief Implementation file for the FFTBackend class.
 */

#include "FFTBackend.h"

#include <cassert>
using namespace std;


/**
 * Constructor.
 */
FFTBackend::FFTBackend() :
	Backend(),
	bins_(8192),
	binOverlap_(4096)
{
	bufferSize_ = sizeof(fftw_complex) * bins_;
	
	in_ = (fftw_complex *) fftw_malloc(bufferSize_);
	out_ = (fftw_complex *) fftw_malloc(bufferSize_);
	fftPlan_ = fftw_plan_dft_1d(bins_, in_, out_, FFTW_FORWARD, FFTW_ESTIMATE);
	
	//inMark_ = in_ + binOverlap_;
	inMark_ = in_;
	inEnd_ = in_ + bins_;
}


/**
 * Destructor
 */
FFTBackend::~FFTBackend()
{
	fftw_destroy_plan(fftPlan_);
	fftw_free(in_);
	fftw_free(out_);
}


/**
 *
 */
void FFTBackend::startStream(StreamInfo info)
{
	Backend::startStream(info);
	inMark_ = in_;
}


/**
 *
 */
void FFTBackend::process(const vector<Complex> &data)
{
	assert(sizeof(Complex) == sizeof(in_[0]));
	
	int size = data.size();
	//int binIncrement = bins_ - binOverlap_;
	
	const Complex *src = &(data[0]);
	
	//fftw_complex *inEnd = in_ + bins_;
	
	while (size >= (inEnd_ - inMark_)) {
		int count = inEnd_ - inMark_;
		
		memcpy(inMark_, src, count * sizeof(in_[0]));
		
		fftw_execute(fftPlan_);
		memcpy(in_, inEnd_ - binOverlap_, binOverlap_ * sizeof(in_[0]));
		inMark_ = in_ + binOverlap_;
		
		processFFT(out_, bins_);
		
		size -= count;
		src += count;
	}
	
	if (size > 0) {
		memcpy(inMark_, src, size * sizeof(in_[0]));
		inMark_ += size;
	}
}


