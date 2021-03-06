/**
 * \file   FFTBackend.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 * 
 * \brief Implementation file for the FFTBackend class.
 */

#include "FFTBackend.h"

#include <cassert>
#include <cmath>
#include <cstring>
using namespace std;

#include <cppapp/utils.h>


const double FFTBackend::PI = 4.0 * atan(1.0);


FFTBackend::FFTBackend(int bins, int overlap) :
	Backend(),
	binOverlap_(overlap /* 32768 - 8192 */),
	bins_(bins /* 32768 */)
{
	if (binOverlap_ < 0) binOverlap_ = 0;
	if (binOverlap_ >= bins_) binOverlap_ = bins_ - 1;

	LOG_DEBUG("FFT backend: bins = " << bins_ << ", overlap = " << binOverlap_);
	
	bufferSize_ = sizeof(fftw_complex) * bins_;
	
	windowFn_ = new float[bufferSize_];
	
	window_ = (fftw_complex *) fftw_malloc(bufferSize_);
	in_ = (fftw_complex *) fftw_malloc(bufferSize_);
	out_ = (fftw_complex *) fftw_malloc(bufferSize_);
	fftPlan_ = fftw_plan_dft_1d(bins_, in_, out_, FFTW_FORWARD, FFTW_ESTIMATE);
	
	//inMark_ = in_ + binOverlap_;
	inMark_ = window_;
	inEnd_ = window_ + bins_;
}


FFTBackend::~FFTBackend()
{
	delete [] windowFn_;
	
	fftw_destroy_plan(fftPlan_);
	fftw_free(window_);
	fftw_free(in_);
	fftw_free(out_);
}


void FFTBackend::startStream(StreamInfo info)
{
	Backend::startStream(info);
	
	inMark_ = window_;
	
	fftSampleRate_ = ((float)info.sampleRate /
				   (float)(bins_ - binOverlap_));
	
	info_ = DataInfo();
	
	LOG_DEBUG("Starting FFT stream with time offset " << info.timeOffset << ", sample rate " << info.sampleRate << "Hz.");
	
	for (int i = 0; i < bins_; i++) {
		//windowFn_[i] = sin(((float)i / (float)bufferSize_) * PI);
		//windowFn_[i] = 0.5 * (
		//	1.0 -
		//	cos(
		//		2.0 * PI * (float)i /
		//		(float)(bins_ - 1)
		//	)
		//);
		
		float a0 = 0.355768;
		float a1 = 0.487396;
		float a2 = 0.144232;
		float a3 = 0.012604;
		
		windowFn_[i] = (
			a0 -
			a1 * cos(
				2.0 * PI * (float)i /
				(float)(bins_ - 1)
			) +
			a2 * cos(
				4.0 * PI * (float)i /
				(float)(bins_ - 1)
			) -
			a3 * cos(
				6.0 * PI * (float)i /
				(float)(bins_ - 1)
			)
		);
	}
}


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
		
		for (int i = 0; i < bins_; i++) {
			in_[i][0] = window_[i][0] * windowFn_[i];
			in_[i][1] = window_[i][1] * windowFn_[i];
		}
		
		fftw_execute(fftPlan_);
		memmove(window_, inEnd_ - binOverlap_, binOverlap_ * sizeof(in_[0]));
		
		inMark_ = window_ + binOverlap_;
		size -= count;
		src += count;
		
		processFFT(out_, bins_, info_);
		
		info_.offset++;
		info_.timeOffset = info_.timeOffset.addSamples(count, streamInfo_.sampleRate);
		//LOG_DEBUG("offset = " << info_.timeOffset << ", count = " << count << ", sr = " << streamInfo_.sampleRate);
	}
	
	if (size > 0) {
		memcpy(inMark_, src, size * sizeof(in_[0]));
		inMark_ += size;
	}
}


void FFTBackend::endStream()
{
	Backend::endStream();
	LOG_DEBUG("Ending FFT stream.");
}


