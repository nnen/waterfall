/**
 * \file   FITSWriter.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-06-29
 *
 * \brief  Header file for the FITSWriter class.
 */

#ifndef FITSWRITER_N9AFZ3HN
#define FITSWRITER_N9AFZ3HN


#include <string>
using namespace std;

#include <fitsio.h>

#include <cppapp/cppapp.h>
using namespace cppapp;


/**
 * \todo Write documentation for class FITSWriter.
 */
class FITSWriter {
private:
	string    fileName_;
	
	fitsfile *file_;
	int       status_;
	
	FITSWriter(const FITSWriter& other);
public:
	/**
	 * Constructor.
	 */
	FITSWriter();
	/**
	 * Destructor.
	 */
	virtual ~FITSWriter();
	
	void open();
	void close();
	void createImage();

	void writeHeader(const char *keyword,
				  int         type,
				  void       *value,
				  const char *comment);
	
	void writeHeader(const char *keyword,
				  const char *value,
				  const char *comment);

	void writeHeader(const char *keyword,
				  float       value,
				  const char *comment);

	void writeHeader(const char *keyword,
				  int         value,
				  const char *comment);
};

#endif /* end of include guard: FITSWRITER_N9AFZ3HN */

