/**
 * \file   Frontend.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-26
 *
 * \brief Header file for the Frontend class.
 */

#ifndef FRONTEND_OBVGMG1U
#define FRONTEND_OBVGMG1U

#include <Object.h>

using namespace cppapp;

#include "Backend.h"

/**
 * \todo Write documentation for class Frontend.
 */
class Frontend : public Object {
private:
	Frontend(const Frontend& other);

protected:
	Ref<Backend> backend_;
	
	StreamInfo   streamInfo_;
	DataInfo     dataInfo_;
	
	void startStream();
	void endStream();
	void process(const vector<Complex> &data);
	
public:
	Frontend() {}
	virtual ~Frontend() {}
	
	void setBackend(Ref<Backend> backend) { backend_ = backend; }
	
	virtual void run() = 0;
};

#endif /* end of include guard: FRONTEND_OBVGMG1U */

