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
#include "FITSWriter.h"

#include <cmath>

using namespace std;

#include <fitsio.h>


////////////////////////////////////////////////////////////////////////////////
// WATERFALL BUFFER
////////////////////////////////////////////////////////////////////////////////


struct WaterfallBuffer {
	/// Number of rows of the buffer (height).
	int            size;
	/// Width of a row.
	int            bins;
	vector<float>  data;
	vector<WFTime> times;
	
	int            mark;
	
	WaterfallBuffer() :
		size(0), bins(0), mark(0)
	{}
	
	WaterfallBuffer(int size, int bins) :
		size(size), bins(bins), mark(0)
	{
		resize(size, bins);
	}
	
	/**
	 * \brief Resize the buffer to the specified number of rows of specified
	 *        width.
	 *
	 * Resizing the buffer also resets buffer's mark. This means that the
	 * buffer is effectively erased and all data previously held by the buffer
	 * is lost.
	 *
	 * \param size number of rows (height)
	 * \param bins width of a row (width)
	 */
	void resize(int size, int bins)
	{
		assert(size >= 0);
		assert(bins > 0);
		
		this->size = size;
		this->bins = bins;
		
		data.resize(size * bins);
		times.resize(size);
		
		rewind();
	}
	
	/**
	 * \brief Resize the buffer to fit within specified memory size.
	 *
	 * \param bins    width of a row
	 * \param maxSize maximal memory size of the buffer
	 * \returns       the resulting number of rows
	 */
	int autoResize(int bins, long maxSize)
	{
		int rows = maxSize / (bins * sizeof(float));
		rows = (rows == 0) ? 1 : rows;
		
		resize(rows, bins);
		
		return rows;
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
	
	/**
	 * \brief Returns number of remaining row in the buffer.
	 */
	inline int remaining() const
	{
		return (size - mark);
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


////////////////////////////////////////////////////////////////////////////////
// WATERFALL RECORDER
////////////////////////////////////////////////////////////////////////////////


class WaterfallRecorder : public Object {
private:
	typedef MethodThread<void, WaterfallRecorder> Thread;
	
	Thread          *workerThread_;
	Mutex            mutex_;
	Condition        condition_;
	bool             exitWorkerThread_;
	
	WaterfallBuffer  inputBuffer_;
	WaterfallBuffer  outputBuffer_;
	
	long             rowCount_;
	long             currentRow_;
	
	FITSWriter       writer_;
	
	void* workerThreadMethod();

public:
	WaterfallRecorder();
	virtual ~WaterfallRecorder();
	
	void   resize(int bins, long maxBufferSize);
	
	void   start();
	void   stop();
	float* addRow(WFTime time);
};


////////////////////////////////////////////////////////////////////////////////
// WATERFALL BACKEND
////////////////////////////////////////////////////////////////////////////////


/**
 * \todo Write documentation for class WaterfallBackend.
 */
class WaterfallBackend : public FFTBackend {
private:
	WaterfallBackend(const WaterfallBackend& other);
	
	string           origin_;
	
	/// Snapshot length in seconds (determines the size of the buffer).
	float            snapshotLength_;
	
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
				  float snapshotLength,
				  float leftFrequency,
				  float rightFrequency);
	virtual ~WaterfallBackend();
	
	virtual void startStream(StreamInfo info);
	virtual void endStream();
};

#endif /* end of include guard: WATERFALLBACKEND_YGIIIZR2 */

