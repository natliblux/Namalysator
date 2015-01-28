#include "configparser.h"

#include <memory>
#include <iostream>


#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "../common_files/xmltostr.h"

using namespace xercesc;
using namespace std;


class HelperMethod {
public : 
	void start(std::string* p){

		//cout << "Helper start" << endl;

		handleString = p;
		*handleString = std::string("");
		methodType = 1;
	};
	
	void end(){
		handleString = NULL;
		methodType = 0;
	};
	
	void charachter(char* s,int len){  
		if ( methodType == 1 ) {
			//cout << "Helper character" << len << endl;
			*handleString +=  xml2stringData(s,len);
		};
	};

private :
	std::string * handleString;
	int methodType;

};



void    configparser::endElement (const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname){
	char* name =XMLString::transcode (qname);
	//cout << "END qname : "<< name << endl ;
	helpMth->end();
	XMLString::release(&name);
	if ( save && position.length() > 1 ){
		ctx->setValue(position.substr(1),value);
		save=false;
	}
	value ="";
	currPoss.pop();
	position=currPoss.top();
}

void configparser::startElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname, const xercesc_3_1::Attributes &attrs){

	char* name =XMLString::transcode (qname);
	std::string tmpName = std::string(name);
	save = true;
	value ="";
	if ( tmpName.compare("config") ) {
		position+="."+tmpName;
	}else{
		position="";
		currPoss.push(position);
	}

	currPoss.push(position);

    if (strcmp(name,"inventoryBook")== 0)	
	{
		helpMth->start(& (ctx->inventoryBook));
	}
	else if (strcmp(name,"checkFile") == 0)		
	{
		helpMth->start(& (ctx->checkFile));
	}
	else if (strcmp(name,"checkSum") == 0)		
	{
		helpMth->start(& (ctx->checkSum));
	}
	else if (strcmp(name,"oddsPages") == 0)		
	{
		helpMth->start(& (ctx->oddsPages));
	}
	else if(strcmp(name,"unlinkedIdentifier") == 0)		
	{
		helpMth->start(& (ctx->unlinkedIdentifier));
	}
	else if (strcmp(name,"identifierMix") == 0)		
	{
		helpMth->start(& (ctx->identifierMix));
	}
	else if (strcmp(name,"dateFolderIssue") == 0)		
	{
		helpMth->start(& (ctx->dateFolderIssue));
	}
	else if( strcmp(name,"noIssueDefined") == 0)		
	{
		helpMth->start(& (ctx->noIssueDefined));
	}
	else if(strcmp(name,"invalidSupplement") == 0)		
	{
		helpMth->start(& (ctx->invalidSupplement));
	}
	else if(strcmp(name,"divs") == 0)		
	{
		helpMth->start(& (ctx->divs));
	}
	else if(strcmp(name,"altoblockPerPage") == 0)		
	{
		helpMth->start(& (ctx->altoblockPerPage));
	}
	else if(strcmp(name,"blockStructure") == 0)		
	{
		helpMth->start(& (ctx->blockStructure));
	}		
	else if(strcmp(name,"coveragePercentAlto") == 0)		
	{
		helpMth->start(& (ctx->coveragePercentAlto));
	}
	else if (strcmp(name,"multipleBlockUse") == 0)		
	{
		helpMth->start(& (ctx->multipleBlockUse));
	}
	else if (strcmp(name,"checkTitle") == 0)		
	{
		helpMth->start(& (ctx->checkTitle));
	}	
	else if (strcmp(name,"issueNumber") == 0)		
	{
		helpMth->start(& (ctx->issueNumber));
	}
	else if (strcmp(name,"measurementSTD") == 0)		
	{
		helpMth->start(& (ctx->measurementSTD));
	}

	XMLString::release(&name);
}

void 	configparser::characters (const XMLCh *const chars, const XMLSize_t length){
	
	char* _chars =XMLString::transcode (chars);
	helpMth->charachter( _chars, strlen(_chars));

	value+=_chars;
	XMLString::release(&_chars);

};


void configparser::init(){
		std::auto_ptr<SAX2XMLReader> tmpParser (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager));
		parser = tmpParser;
		parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
		// Commonly useful configuration.
	
		parser->setFeature (XMLUni::fgSAX2CoreNameSpaces, false);
	    parser->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false);
	    parser->setFeature (XMLUni::fgSAX2CoreValidation, false);

	    // Enable validation.
	    //
	    parser->setFeature (XMLUni::fgXercesSchema, false);
	    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
	    parser->setFeature (XMLUni::fgXercesValidationErrorAsFatal, false);

	    // Use the loaded grammar during parsing.
	    //
	    parser->setFeature (XMLUni::fgXercesUseCachedGrammarInParse, false);

	    // Don't load schemas from any other source (e.g., from XML document's
	    // xsi:schemaLocation attributes).
	    //
	    parser->setFeature (XMLUni::fgXercesLoadSchema, false);

	    // Xerces-C++ 3.1.0 is the first version with working multi import
	    // support.
	    //
#if _XERCES_VERSION >= 30100
       parser->setFeature (XMLUni::fgXercesHandleMultipleImports, false);
#endif
	    
        
	    
	    addXSD("xml.xsd");
		setContentHandler(this);

}

configparser::configparser(Parameters *_ctx):ctx(_ctx){
		lockaddXSD();
		helpMth = new HelperMethod ;
}

configparser::~configparser(){
		delete helpMth;
}