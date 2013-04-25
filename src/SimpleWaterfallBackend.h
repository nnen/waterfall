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
	int width_;

protected:
	virtual void processFFT(fftw_complex *data, int size);
	
public:
	SimpleWaterfallBackend(Ref<Output> output);
	virtual ~SimpleWaterfallBackend();

	virtual void endStream();
};

#endif /* end of include guard: SIMPLEWATERFALLBACKEND_X31NFT7Y */

