/**
 * \file   SimpleWaterfallBackend.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 *
 * \brief Header file for the SimpleWaterfallBackend class.
 */

#ifndef SIMPLEWATERFALLBACKEND_X31NFT7Y
#define SIMPLEWATERFALLBACKEND_X31NFT7Y

#include <utils.h>

#include <Output.h>

using namespace cppapp;

#include "FFTBackend.h"

/**
 * \todo Write documentation for class SimpleWaterfallBackend.
 */
class SimpleWaterfallBackend : public FFTBackend {
private:
	SimpleWaterfallBackend(const SimpleWaterfallBackend& other);
	
	Ref<Output> output_;
	vector<fftw_complex*> rows_;
	vector<WFTime> time_;
	
	float leftRatio_;
	float widthRatio_;
	
	int spectrumWidth_;
	int left_;
	int right_;
	int width_;
	
	int markFreq_;
	int markWidth_;

protected:
	virtual void processFFT(const fftw_complex *data, int size, DataInfo info);
	
public:
	SimpleWaterfallBackend(Ref<Output> output, float left, float width);
	virtual ~SimpleWaterfallBackend();

	virtual void endStream();
};

#endif /* end of include guard: SIMPLEWATERFALLBACKEND_X31NFT7Y */

