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


struct WaterfallBuffer {
	int            size;
	int            bins;
	vector<float>  data;
	vector<WFTime> times;

	int            mark;
	
	WaterfallBuffer() :
		size(0), bins(0), mark(0)
	{}
	
	WaterfallBuffer(int size, int bins) :
		size(size), bins(bins)
	{
		resize(size, bins);
	}
	
	void resize(int size, int bins)
	{
		assert(size > 0);
		assert(bins > 0);
		
		data.resize(size * bins);
		times.resize(size);
	}
	
	void rewind()
	{
		mark = 0;
	}
	
	float* addRow(WFTime time)
	{
		times[mark] = time;
		float *row = &(data[0]) + bins * mark;
		mark++;
		return row;
	}
	
	float* getRow(int index)
	{
		assert(index >= 0);
		assert(index < size);
		
		float *first = &(data[0]);
		return first + bins * index;
	}
	
	bool isFull() const
	{
		return mark >= size;
	}
	
	void swap(WaterfallBuffer &other)
	{
		int mark = this->mark;
		this->mark = other.mark;
		other.mark = mark;
		
		data.swap(other.data);
		times.swap(other.times);
	}
};


/**
 * \todo Write documentation for class WaterfallBackend.
 */
class WaterfallBackend : public FFTBackend {
private:
	WaterfallBackend(const WaterfallBackend& other);
	
	string           origin_;
	
	int              bufferSize_; // in samples
	//float          **buffer_;
	//int              bufferMark_;
	
	//vector<WFTime>   timeBuffer_;
	
	WaterfallBuffer  inBuffer_;
	WaterfallBuffer  outBuffer_;
	
	float            leftFrequency_;
	float            rightFrequency_;
	int              leftBin_;
	int              rightBin_;
	
	MethodThread<void, WaterfallBackend> *snapshotThread_;
	Mutex                                 mutex_;
	Condition                             snapshotCondition_;
	bool                                  endSnapshotThread_;
	
	void writeHeader(fitsfile   *file,
				  const char *keyword,
				  int         type,
				  void       *value,
				  const char *comment,
				  int        *status);
	void writeHeader(fitsfile   *file,
				  const char *keyword,
				  const char *value,
				  const char *comment,
				  int        *status);
	void writeHeader(fitsfile   *file,
				  const char *keyword,
				  float       value,
				  const char *comment,
				  int        *status);
	void writeHeader(fitsfile   *file,
				  const char *keyword,
				  int         value,
				  const char *comment,
				  int        *status);
	
	void* snapshotThread();
	void  makeSnapshot();
	void  startSnapshot();

protected:
	virtual void processFFT(const fftw_complex *data, int size, DataInfo info);
	
public:
	WaterfallBackend(int bins,
				  int overlap,
				  string origin,
				  int bufferSize,
				  float leftFrequency,
				  float rightFrequency);
	virtual ~WaterfallBackend();
	
	virtual void startStream(StreamInfo info);
	virtual void endStream();
};

#endif /* end of include guard: WATERFALLBACKEND_YGIIIZR2 */

