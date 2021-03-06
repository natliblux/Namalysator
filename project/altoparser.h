#ifndef ALTOPARSER_H_
#define ALTOPARSER_H_

#include "datafactory.h"
#include "errorhandler.h"
#include "stateparser.h"


class altoparserContext : public StateParserContext{
	
public :

	std::string alto;
	datafactory *dfalto;
	std::string altoFile;
	altoblock altoBlock;
	//errorHandler *hError;
	bool is_mm10;	
	int  dpi;
};

class StateParserAltoRootState:public StateParserState{
public:
	StateParserAltoRootState(){};
	virtual ~StateParserAltoRootState(){};

	virtual StateParserState* getNext(const char* const name);
};

//! parse ALTO files and store in a datafactory
class altoparser: public StateParserCH
{
		
private :
	altoparserContext ctx;
	StateParserAltoRootState root;

public:	
	altoparser(const std::string &altoFile,const std::string &altoId,errorHandler *h,datafactory *df);
};

#endif	//ALTOPARSER_H_