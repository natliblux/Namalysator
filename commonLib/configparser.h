#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "parserCheck.h"
#include "common.h"
#include <xercesc/sax2/DefaultHandler.hpp>

//! reader of config file to feed xml
//

class HelperMethod; 

class configparser: public parserCheck, public DefaultHandler {
	public:	
		virtual void 	characters (const XMLCh *const chars, const XMLSize_t length);
		virtual void    startElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname, const Attributes &attrs);
		virtual void    endElement (const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);
		configparser(Parameters *_ctx);
		virtual ~configparser();

    protected:
		virtual void init();
	private:
		Parameters* ctx;
		HelperMethod* helpMth; // Help Methode
};

#endif