/**
 * \file   App.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-22
 * 
 * \brief Implementation file for the App class.
 */

#include "App.h"


/**
 *
 */
void App::setUp()
{
	AppBase::setUp();
}


/**
 *
 */
int App::onRun()
{
	AppBase::onRun();
	
	if (options().args().size() > 0) {
		input_ = new FileInput(options().args()[0]);
	} else {
		input_ = new StandardInput();
	}
	
	if (!(bool)options().get('o')) {
		setOutput(new FileOutput(input_->getFileNameWithExt("png")));
	}
	
	WAVStream stream(input_);
	//Ref<Backend> backend = new SimpleWaterfallBackend(output(), 0.2, 0.1);
	Ref<Backend> backend = new WaterfallBackend("uflu", 100);
	stream.setBackend(backend);
	stream.run();

	return 0;
}


/**
 * Constructor.
 */
App::App() :
	AppBase()
{
}


/**
 * Destructor
 */
App::~App()
{
}


