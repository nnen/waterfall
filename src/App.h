/**
 * \file   App.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-22
 *
 * \brief Header file for the App class.
 */

#ifndef APP_TBSBM2YW
#define APP_TBSBM2YW

#include <cppapp/AppBase.h>
#include <cppapp/Input.h>

using namespace cppapp;

// TODO: Remove later.
#include "WAVStream.h"
#include "JackFrontend.h"
#include "WaterfallBackend.h"
#include "Signal.h"


/**
 * \todo Write documentation for class App.
 */
class App : public AppBase {
private:
	// Ref<Input> input_;
	
	App(const App& other);

protected:
	// inline Ref<Input> input() { return input_; }
	
	Ref<Frontend> frontend_;
	Ref<Backend>  backend_;
	
	Ref<Frontend> createFrontend();
	Ref<Backend>  createBackend();
	
	virtual void setUp();
	virtual int onRun();
	
	void interruptHandler(int sigNum);

public:
	App();
	virtual ~App();
};


#endif /* end of include guard: APP_TBSBM2YW */

