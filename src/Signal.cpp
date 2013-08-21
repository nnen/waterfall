/**
 * \file   Signal.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-08-20
 * 
 * \brief  Implementation file for the Signal class.
 */

#include "Signal.h"


#define DEF_SIG(name) Signal_<SIG##name> Signal::name(#name);
DEF_SIG(INT);
DEF_SIG(TERM);
#undef DEF_SIG


//define DEF_SIG(var, name) Signal Signal::var(name, #name);
//
//
//DEF_SIG(interrupt_, SIGINT);
//DEF_SIG(terminate_, SIGTERM);
//
//
//void Signal::setHandler(Handler handler)
//{
//	VAR(result, signal(number_, handler));
//	
//	if (result == SIG_ERR) {
//		int error = errno;
//		LOG_ERROR("Setting signal handler for " << name_ <<
//				" failed: " << strerror(error) <<
//				" (code: " << error << ")");
//	}
//}


