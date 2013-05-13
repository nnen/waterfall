/**
 * \file   WFTime.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-26
 * 
 * \brief Implementation file for the WFTime class.
 */

#include "WFTime.h"


/**
 * \note This function uses the `strftime` function as declared in the
 *       ctime (time.h) header.
 * \note Implementation of this function assumes that <p>time_t</p> type is
 *       implemented as an integer containing unix timestamp. While this is true
 *       on most systems, there is no guarantee.
 */
string WFTime::format(const char *fmt, bool local) const
{
	time_t timestamp = seconds();
	//struct tm *timeInfo = localtime(&timestamp);
	struct tm *timeInfo;
	if (local)
		timeInfo = localtime(&timestamp);
	else
		timeInfo = gmtime(&timestamp);
	
	char buffer[256];
	size_t length = strftime(buffer, sizeof(buffer), fmt, timeInfo);
	if (length < 1) buffer[0] = '\0';
	
	return string(buffer);
}


