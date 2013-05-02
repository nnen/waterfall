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

#include <utils.h>


/**
 * Constructor.
 */
FFTBackend::FFTBackend() :
	Backend(),
	binOverlap_(32768 - 8192),
	bins_(32768)
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
	
	fftSampleRate_ = ((float)info.sampleRate /
				   (float)(bins_ - binOverlap_));
	
	info_ = DataInfo();
}


/**
 *
 */
void FFTBackend::process(const vector<Complex> &data, DataInfo info)
{
	assert(sizeof(Complex) == sizeof(in_[0]));
	//assert(binOverlap_ <= (bins_ - binOverlap_));
	
	int size = data.size();
	const Complex *src = &(data[0]);
	
	info_.timeOffset = info.timeOffset;

	while (size >= (inEnd_ - inMark_)) {
		int count = inEnd_ - inMark_;
		
		memcpy(inMark_, src, count * sizeof(in_[0]));
		
		fftw_execute(fftPlan_);
		//memcpy(in_, inEnd_ - binOverlap_, binOverlap_ * sizeof(in_[0]));
		memmove(in_, inEnd_ - binOverlap_, binOverlap_ * sizeof(in_[0]));
		inMark_ = in_ + binOverlap_;
		
		processFFT(out_, bins_, info_);
		
		size -= count;
		src += count;
		
		info_.offset++;
		//info_.timeOffset = info_.timeOffset.addSamples(count, fftSampleRate_);
		info_.timeOffset = info_.timeOffset.addSamples(count, streamInfo_.sampleRate);
	}
	
	if (size > 0) {
		memcpy(inMark_, src, size * sizeof(in_[0]));
		inMark_ += size;
	}
}


