/**
 * \file   App.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-22
 *
 * \brief Header file for the App class.
 */

#ifndef APP_TBSBM2YW
#define APP_TBSBM2YW

#include <AppBase.h>
#include <Input.h>

using namespace cppapp;

// TODO: Remove later.
#include "WAVStream.h"
#include "SimpleWaterfallBackend.h"


/**
 * \todo Write documentation for class App.
 */
class App : public AppBase {
private:
	Ref<Input> input_;
	
	App(const App& other);

protected:
	inline Ref<Input> input() { return input_; }
	
	virtual void setUp();
	virtual int run();

public:
	App();
	virtual ~App();

	int run(int argc, char* argv[]);
};

#endif /* end of include guard: APP_TBSBM2YW */

