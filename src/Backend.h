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

#include <Object.h>

using namespace cppapp;

//typedef double Complex[2];
struct Complex {
	double real;
	double imag;
};


struct StreamInfo {
	bool knownLength;
	int  length;
	int  sampleRate;
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
	virtual void process(const vector<Complex> &data) = 0;
	virtual void endStream() {}
};

#endif /* end of include guard: BACKEND_IFO2SX99 */

