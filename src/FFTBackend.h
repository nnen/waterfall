/**
 * \file   FFTBackend.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 *
 * \brief Header file for the FFTBackend class.
 */

#ifndef FFTBACKEND_QYQ7WJUZ
#define FFTBACKEND_QYQ7WJUZ

#include <fftw3.h>

#include "Backend.h"

/**
 * \todo Write documentation for class FFTBackend.
 */
class FFTBackend : public Backend {
private:
	FFTBackend(const FFTBackend& other);
	
	int bins_;
	int binOverlap_;
	int bufferSize_;
	
	fftw_complex *in_, *out_;
	fftw_complex *inMark_, *inEnd_;
	fftw_plan     fftPlan_;
	
protected:
	virtual void processFFT(fftw_complex *data, int size) {}
	
public:
	FFTBackend();
	virtual ~FFTBackend();
	
	virtual void startStream(StreamInfo info);
	virtual void process(const vector<Complex> &data);
};

#endif /* end of include guard: FFTBACKEND_QYQ7WJUZ */

