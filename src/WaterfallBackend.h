/**
 * \file   WaterfallBackend.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-25
 *
 * \brief Header file for the WaterfallBackend class.
 */

#ifndef WATERFALLBACKEND_YGIIIZR2
#define WATERFALLBACKEND_YGIIIZR2


#include "FFTBackend.h"

#include <cmath>

using namespace std;

#include <fitsio.h>


/**
 * \todo Write documentation for class WaterfallBackend.
 */
class WaterfallBackend : public FFTBackend {
private:
	WaterfallBackend(const WaterfallBackend& other);

	string           origin_;
	
	int              bufferSize_; // in samples
	float          **buffer_;
	int              bufferMark_;
	
	vector<WFTime>   timeBuffer_;
	
	float            leftFrequency_;
	float            rightFrequency_;
	int              leftBin_;
	int              rightBin_;
	
	void makeSnapshot();

protected:
	virtual void processFFT(const fftw_complex *data, int size, DataInfo info);
	
public:
	WaterfallBackend(string origin,
				  int bufferSize,
				  float leftFrequency,
				  float rightFrequency);
	virtual ~WaterfallBackend();
	
	virtual void startStream(StreamInfo info);
	virtual void endStream();
};

#endif /* end of include guard: WATERFALLBACKEND_YGIIIZR2 */

