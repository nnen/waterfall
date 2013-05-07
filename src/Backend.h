/**
 * \file   Backend.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 *
 * \brief Header file for the Backend class.
 */

#ifndef BACKEND_IFO2SX99
#define BACKEND_IFO2SX99

#include <vector>

using namespace std;

#include <cppapp/Object.h>

using namespace cppapp;

#include "WFTime.h"

//typedef double Complex[2];
struct Complex {
	double real;
	double imag;
};


struct StreamInfo {
	bool   knownLength;
	int    length;
	int    sampleRate;
	
	WFTime timeOffset;

	StreamInfo()
	{
		knownLength = false;
		length = 0;
		sampleRate = 48000;

		timeOffset = WFTime(0, 0);
	}
};


struct DataInfo {
	long   offset;
	
	WFTime timeOffset;

	DataInfo()
	{
		offset = 0;
		timeOffset = WFTime(0, 0);
	}
};


/**
 * \todo Write documentation for class Backend.
 */
class Backend : public Object {
private:
	Backend(const Backend& other);

protected:
	StreamInfo streamInfo_;
	
public:
	Backend();
	virtual ~Backend() {}
	
	virtual void startStream(StreamInfo info) { streamInfo_ = info; }
	virtual void process(const vector<Complex> &data, DataInfo info) = 0;
	virtual void endStream() {}
};

#endif /* end of include guard: BACKEND_IFO2SX99 */

