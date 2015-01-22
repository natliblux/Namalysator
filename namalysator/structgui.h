#ifndef STRUCTGUI_H_
#define STRUCTGUI_H_

#include <QDate>
#include "../common_files/struct.h"


/**
* A test class. A more elaborate class description.
*/
struct TestSet
{
	int id;
	std::string batchName;
	QDate date;
};

struct LinkedFiles
{
	int id;
	int idMets;
	std::string type;
	std::string grouId;
	std::string checksum;
	double size;
	std::string fileName;
	std::string fileId;
	int dpi;
};

struct MetsFile
{
	int idMets;
	int idTestSet;
	std::string docType; // The type of document (e.g. "Newspaper" or "Serial")
	std::string path;
	std::string fileName;
	std::string issueNumber;
	std::vector<int> vectIssueNumber;
	std::string title;
	int pages;
	std::vector<std::string> supplements;
	QDate date;
	int year;
	std::map<std::string,std::map<int,LinkedFiles> > mapLinked;
};


struct ErrorCategory{
	int id;
	int id_category;
	std::string name;
};

struct ErrorSeverity {
	int id;
	int id_sevrity;
	std::string gravity;
};
struct ErrorType {
	int id;
	int id_type;
	std::string error;
	std::string details;
	int id_severity;
	ErrorSeverity severity;
	ErrorCategory category;
};

struct ErrorSummary
{
	int count;
	ErrorType errorType;
};

struct MetsError
{
	MetsFile mets;
	LinkedFiles linkedFiles;
	int id;
	int idRelatedFile;	
	std::string relatedType;
	std::string filePart;
	int errorLine;
	int errorColumn;
	ErrorType errorType;
	std::string message;
	std::string id_search;
};



//TODO comment
class BatchDetail
{
private:
	BatchDetail(){};
	//dbrequest db;
	static BatchDetail bd;
public:
	Inventaire inventaire;
	std::string path; // Path of disk	
	std::string database;
	std::string databaseInv; 
	std::string batchName;
	std::string minDate;
	std::string maxDate;
	int count;	//number of Mets
	std::string testDate;
	int idTestSet;

	// Get a METS file by ID
	MetsFile *getMetsByID(int id);
	bool getMetsByID(int id, MetsFile &result);

	// Get a METS file by Date
	std::vector<MetsFile> *getMetsByDate(QDate date);
	bool getMetsByDate(QDate date, std::vector<MetsFile> &result);

	static BatchDetail& getBatchDetail(){return bd;};
	/*
	dbrequest& getDataBase(){
		db.setDataBaseName(database);
		return db;
	};
	*/
};

struct DateComment
{
	int id;
	int id_DateError;
	QDate date;
	std::string comment;
};

struct DateError
{
	int id;
	int id_testset;
	std::string dateBegin;
	std::string dateEnd;
	std::string issues;
	std::string comment;
	std::string valider;
	ErrorType errortype;
	std::vector<DateComment> dateComment;
	int errorType;
};


struct Sampling_Structure
{
	int id;
	int id_Mets;
	int checked;
	MetsFile Mets;
};

struct StructureError {

	int id;
	int id_mets;
	std::string pathImage;
	std::string message;
	ErrorType errorType;
	int pageNb;


};

struct Title 
{
	int id;
	int id_article;
	int id_mets;
	int countString;
	int countError;
	std::string title;
	MetsFile mets;
	Article article;
};

struct Excel
{
	int id;	
	int id_sheet;
	QDate date;
	int day;
	int month;
	int year;
	std::vector<int> issueNumber;
	int pages;
	std::string supplement;
	int supPages;
	std::string type;
	int jg;
	int annexes; // if 1 there's an annexe , if 0 it's not a annexe
	int missing; // if 1 missing else 0 not missing 
};



#endif //STRUCTGUI_H_  
