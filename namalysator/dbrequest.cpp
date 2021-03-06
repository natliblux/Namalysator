#include "dbrequest.h"
#include <sstream>
#include <iomanip>
#include "gui_utilities.h"
#include <QDate>
#include <QTime>
#include <QIcon>
#include <QErrorMessage>

#include <iostream>

#ifdef LOG_TIMING
#define DEBUG_ME	{ fprintf(conn.fpLog, "SQL: %s\n\t%s\n", selectSql.c_str(), qPrintable(QTime::currentTime().toString("hh:mm:ss:zzz"))); fflush(conn.fpLog); }
#else
#define DEBUG_ME
#endif

#pragma warning(disable:4503)

ConnectionPool g_pool;

/////////////////////////////////////////////////////////////
// class ConnectionDB

ConnectionDB::ConnectionDB() 
{ 
	db = 0;
#ifdef LOG_TIMING
	fpLog = 0;
#endif
}

ConnectionDB::~ConnectionDB() 
{
	close();
}

bool ConnectionDB::openConnection(const std::string &dbname)
{
	int	rc = sqlite3_open(dbname.c_str(), &db);
	if (rc)	{			
		sqlite3_close(db);
		return false;
	}
#ifdef LOG_TIMING
	fpLog = fopen("C:\\Development\\sqlite-shell\\logging.txt", "a");
#endif
	return true;
}

void ConnectionDB::close()
{
	if (db) {
		sqlite3_close(db);
		db = 0;
	}
#ifdef LOG_TIMING
	if (fpLog) {
		fclose(fpLog);
		fpLog = 0;
	}
#endif
}


/////////////////////////////////////////////////////////////
// class ConnectionPool

ConnectionDB* ConnectionPool::getConnection(const std::string &dbname)
{
	std::map<std::string, ConnectionDB*>::iterator it = Databases.find(dbname);
	if (it != Databases.end()) {
		return it->second;
	} else {
		ConnectionDB* db=new ConnectionDB();
		db->openConnection(dbname);
		Databases[dbname] = db;
		return db;
	}
}

ConnectionPool::~ConnectionPool(){
	std::map<std::string, ConnectionDB*>::iterator it;
	for ( it =Databases.begin();it != Databases.end();it++){
		delete it->second;
	}
}

/////////////////////////////////////////////////////////////
// class dbrequest

void dbrequest::raiseError(ConnectionDB* Conn, std::string message){
	std::string error ( sqlite3_errmsg(Conn->db) );
	std::stringstream ss;
	ss << "ERROR DB :" << error << std::endl << message ;
	static QErrorMessage* Qerror= new QErrorMessage();
	Qerror->showMessage(ss.str().c_str());
}

void dbrequest::setDataBaseName(std::string dbName)
{
	databaseName = dbName;
}

// get all testsets from database
std::map<std::string,std::map<std::string,std::vector<std::pair<int,std::string> > > > dbrequest::getvTestSet()
{
	std::map<std::string,std::map<std::string,std::vector<std::pair<int,std::string> > > > ret;
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 

	static const std::string selectSql = "SELECT BATCHNAME,SUBBATCHNAME,ID_TESTSET, DATE FROM TESTSET";
	
	rc = sqlite3_prepare_v2(conn->db, selectSql.c_str(), -1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{		
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col=0;
			std::pair<int,std::string> p;
			std::string batchName = (char*)safe_sqlite3_column_text(pStmt, col++);
			std::string subbatchName = (char*)safe_sqlite3_column_text(pStmt, col++);
			int idtestset = sqlite3_column_int(pStmt, col++);
			std::string date = (char*)safe_sqlite3_column_text(pStmt, col++);
			p.first = idtestset;
			p.second = date;
			((ret[batchName])[subbatchName]).push_back(p);
		}
	}else{
		raiseError(conn,selectSql);
	};
	sqlite3_finalize(pStmt);
	return ret;
}

std::map<std::string,std::string> dbrequest::key2Label(std::map<std::string,std::string> map){
	std::map<std::string,std::string> ret;
	for ( std::map<std::string,std::string>::iterator it=map.begin(); it !=map.end(); it++){
		ret [ getLabel ( it->first ) ] = it->second;
	};
	return ret;
}

std::string dbrequest::getLabel(std::string key){
	static std::map<std::string,std::string> internal; // must be define in db_connection like could be dependant of the database name ( cherry on the cake )
	std::string ret="";
	if ( internal.size() == 0) { // LOAD normally in a table

		ConnectionDB* conn = g_pool.getConnection(databaseName);
		sqlite3_stmt *pStmt;
		int rc;	
		const char *zErrMsg= 0; 

		static const std::string selectSql = "SELECT KEY,LABEL FROM KEY2LABEL";
		
		rc = sqlite3_prepare_v2(conn->db, selectSql.c_str(), -1, &pStmt,&zErrMsg);

		if(rc == SQLITE_OK)
		{		
			while(sqlite3_step(pStmt) == SQLITE_ROW)
			{
				internal[safe_sqlite3_column_text(pStmt, 0)]=safe_sqlite3_column_text(pStmt, 1);
			}
		}else{ // BUT IF NO Table 
			internal["dataintegrity.checkSum"]="CheckSum";
			internal["semanticchecks.dateFolderIssue"]="Date Structure of Issue";
			internal["semanticchecks.divs"]="Div Structure";
			internal["dataintegrity.unlinkedIdentifier"]="not link identifier";
			internal["semanticchecks.identifierMix"]="Mix";
			internal["blocks.altoblockPerPage"]="One alto per page";
			internal["blocks.blockStructure"]="Block Structure";
			internal["blocks.coveragePercentAlto"]="Cover Percentage";
			internal["blocks.multipleBlockUse"]="Use Multi";
			internal["semanticchecks.noIssueDefined"]="Issue not define";
			internal["semanticchecks.invalidSupplement"]="Invalid Supplement";
			internal["blocks.measurementSTD"]="Measurement standard";
			internal["dataintegrity.checkFile"]="Check File";
		}
		sqlite3_finalize(pStmt);
			

	};

	std::map<std::string,std::string>::iterator it = internal.find(key);
	if ( it == internal.end() ){
		internal[key]=key;
		ret = key;
	}else{
		ret = it->second;
	}
	return ret;
}


void  dbrequest::getBatch(BatchDetail& bdetail,int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
    sqlite3_stmt *pStmt;	
	const char *zErrMsg=0; 	
	bdetail.idTestSet = id_testset;
	static const std::string selectSql = "select min(m.Date),max(m. Date),count(id_mets),batchname,t.date from Mets m,Testset t where t.id_testset = m.id_testset and m.id_testset =?";

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt,1,id_testset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			std::pair<std::string,std::string> p;
			bdetail.minDate = safe_sqlite3_column_text(pStmt, 0);
			bdetail.maxDate = safe_sqlite3_column_text(pStmt, 1);
			bdetail.count = sqlite3_column_int(pStmt, 2);
			bdetail.batchName = safe_sqlite3_column_text(pStmt, 3);
			bdetail.testDate = safe_sqlite3_column_text(pStmt, 4);
		}
	}else{
		raiseError(conn,selectSql);
	};
	sqlite3_finalize(pStmt);
}

/// <summary>get details of a mets</summary>	
/// <param name="id_mets">id mets</param>
/// <returns>MetsFile</returns>	
MetsFile dbrequest::getMets(int id_mets)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	MetsFile mets;  
	static const std::string selectSql = "SELECT ID_METS,ID_TESTSET,PATH,"
		                                         "FILENAME,ISSUE_NUMBER,TITLE,"
												 "PAGES,DATE,YEAR,PAPERTYPE FROM METS where ID_METS = ?";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt,1,id_mets);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			mets.idMets = sqlite3_column_int(pStmt, 0);
			mets.mapLinked["IMGGRP"] = getMapLinkedFiles(mets.idMets, "IMGGRP");
			mets.mapLinked["ALTOGRP"] = getMapLinkedFiles(mets.idMets, "ALTOGRP");
			mets.mapLinked["PDFGRP"] = getMapLinkedFiles(mets.idMets, "PDFGRP");
			mets.mapLinked["BWGRP"] = getMapLinkedFiles(mets.idMets, "BWGRP");
			mets.mapLinked["THUMBGRP"] = getMapLinkedFiles(mets.idMets, "THUMBGRP");

			mets.idTestSet = sqlite3_column_int(pStmt, 1);
			mets.path = safe_sqlite3_column_text(pStmt, 2);
			mets.fileName = safe_sqlite3_column_text(pStmt, 3);
			mets.issueNumber = safe_sqlite3_column_text(pStmt, 4);
			mets.vectIssueNumber = getIssueNumber(mets.issueNumber);
			mets.title = safe_sqlite3_column_text(pStmt, 5);
			mets.pages = sqlite3_column_int(pStmt, 6);
			mets.date =  mets.date.fromString(safe_sqlite3_column_text(pStmt, 7), "yyyy-MM-dd");
			mets.year = sqlite3_column_int(pStmt, 8);
			mets.docType = safe_sqlite3_column_text(pStmt, 9);
		}	  
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	// Get names of supplements
	static const std::string selectSql2 = "SELECT TITLE_SUPPLEMENT FROM SUPPLEMENTS WHERE ID_METS = ?";
	
	rc = sqlite3_prepare_v2(conn->db,selectSql2.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK)
	{
		sqlite3_bind_int(pStmt,1,id_mets);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			mets.supplements.push_back(safe_sqlite3_column_text(pStmt, 0));
		}
	}else{
		raiseError(conn,selectSql2);
	}
	sqlite3_finalize(pStmt);
	return mets;
}

/// <summary>get details of LinkedFiles</summary>	
/// <param name="id_mets">id of mets </param>
/// <param file_part="">filepart(alto,img,pdf)</param>
/// <returns>LinkedFiles</returns>	
LinkedFiles dbrequest::getLinkedFiles(int id,std::string file_part)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::stringstream oid;
	oid << id;	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0; 
	LinkedFiles lf;  
	std::string selectSql = "SELECT * FROM LINKEDFILES where ID_METS ='" + oid.str() + "'and FILEID = '"+ file_part + "' ";
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK)
	{	  
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		int count = sqlite3_data_count(pStmt);		
		 for (int i =0;i<  count ;i++)
		 {
			const char *result = safe_sqlite3_column_text(pStmt, i);			
			if (i==0) lf.id = atoi(result);									
			else if (i==1) lf.idMets = atoi(result);
			else if (i==2) lf.type = atoi(result);
			else if (i==3) lf.grouId = result;
			else if (i==4) lf.checksum = result;
			else if (i==5) lf.size =  atof(result) ;
			else if (i==6) lf.fileName = result;
			else if (i==7) lf.fileId = result;		
		 }		 
	  }
   }
	sqlite3_finalize(pStmt);
   return lf;
}

Parameters dbrequest::getParameterVerifiers(int id_testset){

		ConnectionDB* conn = g_pool.getConnection(databaseName);
	Parameters param;
	static std::string sql = "SELECT KEY,VALUE FROM PARAMTESTSET WHERE ID_TESTSET=?";
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0; 

	int rc = sqlite3_prepare_v2(conn->db,sql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc != SQLITE_OK) 
	{   // try with old implementation
		sqlite3_finalize(pStmt);
		return _getParameterVerifiers(id_testset); 
	};

	sqlite3_bind_int(pStmt,1,id_testset);

	while(sqlite3_step(pStmt) == SQLITE_ROW)
    {
		int col=0;
		const char *key = safe_sqlite3_column_text(pStmt, col++);
		const char *value = safe_sqlite3_column_text(pStmt, col++);

		param.addParam(Parameters::prefix+key,value);
	};

	sqlite3_finalize(pStmt);


	return param; 
}

// old implementation @deprecated
// return the parameters of verified tests
Parameters dbrequest::_getParameterVerifiers(int id_testset)
{
	std::stringstream oId;
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	oId << id_testset;	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0; 
	Parameters param;
	
	std::string sql = "select FILECHECK,CHECKSUM,DIVS,UNLINKEDIDENTIFIER, \
	IDENTIFIERMIX,INVALIDSUPPLEMENT,NOISSUEDEFINED,ALTOBLOCKPERPAGE, \
	BLOCKSTRUCTURE,COVERAGEPERCENTALTO,MULTIBLOCKUSE,DATES,SCHEMACHECK from VERIFIERS where ID_TESTSET ='" + oId.str() + "'";
	int rc = sqlite3_prepare_v2(conn->db,sql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		int count = sqlite3_data_count(pStmt);
		std::pair<int,DateError> p;
		 for (int i =0;i<  count ;i++)
		 {
			const char *result = safe_sqlite3_column_text(pStmt, i);
			
			if (i==0) param.addParam("verifiers.dataintegrity.checkFile",result);								
			else if (i==1) param.addParam("verifiers.dataintegrity.checkSum",result);	
			else if (i==2) param.addParam("verifiers.semanticchecks.divs",result);
			else if (i==3) param.addParam("verifiers.dataintegrity.unlinkedIdentifier",result);
			else if (i==4) param.addParam("verifiers.semanticchecks.identifierMix",result);		
			else if (i==5) param.addParam("verifiers.semanticchecks.invalidSupplement",result);	
			else if (i==6) param.addParam("verifiers.semanticchecks.noIssueDefined",result);		
			else if (i==7) param.addParam("verifiers.blocks.altoblockPerPage",result);
			else if (i==8) param.addParam("verifiers.blocks.blockStructure",result);	
			else if (i==9) param.addParam("verifiers.blocks.coveragePercentAlto",result);		
			else if (i==10) param.addParam("verifiers.blocks.multipleBlockUse",result);
			//else if (i==11) param.dates = result;
			//else if (i==12) param.schemaValidation = result;					
		 }			
	  }
    }else{
		raiseError(conn,sql);
	}
	sqlite3_finalize(pStmt);
	return param;  	
}



/// <summary>get all errors of the current testset (without dates) </summary>
/// <param name="id">id_testset</param>
/// <returns>vector of ErrorSummary</returns>
std::vector<ErrorSummary> dbrequest::getvErrorSummary(int id_testset){
	std::vector<ErrorSummary> ret;
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0;
	
    static const std::string selectSql ="SELECT ID_ERRORTYPE,COUNT (ID_ERRORTYPE) FROM MetsError where ID_TESTSET = ? GROUP BY ID_ERRORTYPE";
	
    int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	
    if(rc == SQLITE_OK)
    {	 
		ErrorSummary es;
		sqlite3_bind_int(pStmt,1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;
			es.errorType = getErrorTypeWithId( sqlite3_column_int(pStmt,col++));
			es.count = sqlite3_column_int(pStmt,col++);
					
			ret.push_back(es);		 
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return ret;
}

/// <summary>get the summary of the date errors of the current testset</summary>
/// <param name="id">id_testset</param>
/// <returns>vector of ErrorSummary</returns>
std::vector<ErrorSummary> dbrequest::getvErrorDate(int id_testset)
{
	std::vector<ErrorSummary> v ;
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	ErrorSummary es;
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0;

	const std::string selectSql ="SELECT ID_ERRORTYPE,COUNT (ID_ERRORTYPE) FROM DATEERROR where ID_TESTSET = ? GROUP BY ID_ERRORTYPE";
	
    int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

    if(rc == SQLITE_OK)
    {	 
		sqlite3_bind_int(pStmt,1,id_testset);

      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		  int col = 0;
		  es.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
		  es.count = sqlite3_column_int(pStmt,col++);					
		 		
		 v.push_back(es);		 
	  }
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

/// <summary>get Errortype of the current id </summary>
/// <param name="id">id of type of error</param>
/// <returns>ErrorType</returns>
ErrorType dbrequest::getErrorTypeWithId(int id)
{
	sqlite3_stmt *pStmt;
	ConnectionDB* conn = g_pool.getConnection(databaseName);

	static std::map<int,ErrorType> cache;
	std::map<int,ErrorType>::iterator it ;

	it = cache .find(id);

	if ( it != cache.end() ) return it->second;

	const char *zErrMsg= 0; 
	const std::string selectSql = "SELECT ID,ID_TYPE,ERROR,DETAILS,ID_SEVERITY,ID_CATEGORY FROM ERRORTYPE where id_type = ?"; 

	std::vector<ErrorType> v;
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorType et;  
	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt,1,id);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0; 
			et.id = sqlite3_column_int(pStmt, col++);
			et.id_type = sqlite3_column_int(pStmt, col++);
			et.error = safe_sqlite3_column_text(pStmt, col++);
			et.details = safe_sqlite3_column_text(pStmt, col++);
			et.severity = getErrorSeverityWithId(sqlite3_column_int(pStmt, col++));	
			et.category = getErrorCategoryWithId(sqlite3_column_int(pStmt, col++));				 
		}		
    }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	cache[id]=et;
	return et;
}

/// <summary>get ErrorSeverity of the current id of ErrorType </summary>
/// <param name="id">id of Errortype</param>
/// <returns>ErrorSeverity</returns>
ErrorSeverity dbrequest::getErrorSeverityWithId(int id)
{
	sqlite3_stmt *pStmt;
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	
	const char *zErrMsg= 0; 
	static const std::string selectSql = "SELECT * FROM ErrorSeverity where ID_SEVERITY =?"; 	
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorSeverity es;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0; 
			es.id = sqlite3_column_int(pStmt, col++);
			es.id_sevrity = sqlite3_column_int(pStmt, col++);
			es.gravity = safe_sqlite3_column_text(pStmt, col++);					 
		}		
    }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return es;
}

/// <summary>get ErrorCategory of the current id of ErrorType </summary>
/// <param name="id">id of Errortype</param>
/// <returns>ErrorCategory</returns>
ErrorCategory dbrequest::getErrorCategoryWithId(int id)
{	
	sqlite3_stmt *pStmt;
	ConnectionDB* conn = g_pool.getConnection(databaseName);

	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT * FROM ErrorCategory where ID_CATEGORY =?"; 	

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorCategory ec;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{			
			int col = 0; 
			ec.id = sqlite3_column_int(pStmt, col++);
			ec.id_category = sqlite3_column_int(pStmt, col++);
			ec.name = safe_sqlite3_column_text(pStmt, col++);
				 
		}		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return ec;
}

/// <summary>get ErrorCategory of the current id of ErrorType </summary>
/// <returns>vector of ErrorType</returns>
std::vector<ErrorType> dbrequest::getErrorType()
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
		
	const char *zErrMsg=0; 
	const std::string selectSql = "SELECT ID,ID_TYPE,ERROR,DETAILS,ID_SEVERITY,ID_CATEGORY FROM ERRORTYPE"; 
	
	//std::string selectSql = "SELECT * FROM ERRORTYPE where ID_CATEGORY = 6"; 
	std::vector<ErrorType> v;
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	  
	if(rc == SQLITE_OK)
	{
		ErrorType et;
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0; 
			et.id = sqlite3_column_int(pStmt, col++);
			et.id_type = sqlite3_column_int(pStmt, col++);
			et.error = safe_sqlite3_column_text(pStmt, col++);
			et.details = safe_sqlite3_column_text(pStmt, col++);
			et.id_severity = sqlite3_column_int(pStmt, col++);
			et.severity = getErrorSeverityWithId(et.id_severity);	
			et.category = getErrorCategoryWithId(sqlite3_column_int(pStmt, col++));
			
			v.push_back(et);				 
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}



/// <summary>count number of error for distinct errortype </summary>
/// <param name="id">id of Errortype</param>
/// <param name="id">id testset</param>
/// <returns>int count</returns>
int dbrequest::getErrorTypeCountWithTestset(int idError,int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	
	int count =0;
	const char *zErrMsg= 0; 
	static const std::string selectSql = "SELECT count(ID) FROM MetsError where id_errortype = ? and id_testset = ?"; 
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	if(rc == SQLITE_OK)
	{
		sqlite3_bind_int(pStmt,1,idError);
		sqlite3_bind_int(pStmt,2,id_testset);
		while(sqlite3_step(pStmt) == SQLITE_ROW){				
			count = sqlite3_column_int(pStmt, 0);
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return count;
}

/// <summary>count number of date error </summary>
/// <param name="id">id  Errortype</param>
/// <param name="id">id testset</param>
/// <returns>int count</returns>
int dbrequest::getErrorTypeCountWithTestsetDates(int idError,int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	int count =0;
	const char *zErrMsg= 0; 
	const std::string selectSql = "SELECT count(ID) FROM DATEERROR where id_errortype =? and id_testset = ?"; 
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,idError);
		sqlite3_bind_int(pStmt,2,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{						
			count = sqlite3_column_int(pStmt, 0);	
		}		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return count;
}

/// <summary>get all mets</summary>
/// <param name="id">id _testset</param>
/// <returns>map QDate,MetsFile </returns>
std::map<QDate,std::vector<int> > dbrequest::getmMetsDate(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);

    sqlite3_stmt *pStmt;
	const char *zErrMsg= 0;
	std::map<QDate,std::vector<int> >  mapMetsDate;
	std::vector<int> vMets;
	const std::string selectSql = "SELECT ID_METS, DATE FROM METS where ID_TESTSET =?";

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
   if(rc == SQLITE_OK)
   {	
	   sqlite3_bind_int(pStmt,1,id_testset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int result_id_mets = sqlite3_column_int(pStmt, 0);
			const char *result_date = safe_sqlite3_column_text(pStmt, 1);
		    QDate date = date.fromString(result_date, "yyyy-MM-dd");
			if (mapMetsDate.find(date) == mapMetsDate.end())
			{
				vMets.clear();
				vMets.push_back(result_id_mets);
				mapMetsDate[date] = vMets;
			}
			else
			{
				mapMetsDate[date].push_back(result_id_mets);
			}	  
		}		
	}else{
		raiseError(conn,selectSql);
	}
   sqlite3_finalize(pStmt);
   return mapMetsDate;
}

/// <summary>get linked files of an mets for the selected group</summary>
/// <param name="idMets">idMets</param>
/// <param name="fileGroup">string fileGroup</param>
/// <returns>map int,LinkedFiles </returns>
std::map<int,LinkedFiles> dbrequest::getMapLinkedFiles(int idMets,std::string fileGroup)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::map<int,LinkedFiles> altoPath;

    sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	int page=1;
	static const std::string selectSql = "SELECT a.ID,a.ID_METS,a.TYPE,a.GROUPID,a.CHECKSUM,a.SIZE,a.FILENAME,a.FILEID,a.DPI, b.FILEID "
		                    "FROM LINKEDFILES a LEFT JOIN STRUCTUREERROR b on a.ID_METS = b.ID_METS and a.FILEID =b.FILEID "
							"where a.ID_METS = ? and a.TYPE = ? "
							"ORDER BY a.ID";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	LinkedFiles lf;  
	if(rc == SQLITE_OK)
	{	int count = -1 ;
	    lf.id =-1;
		int previous;
		sqlite3_bind_int(pStmt,1,idMets);
		sqlite3_bind_text(pStmt,2,fileGroup.c_str(),fileGroup.length(),SQLITE_STATIC);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{	
			int col = 0;
		    previous = sqlite3_column_int(pStmt,col++);
			if ( previous == lf.id ){
				count ++;
				continue;
			}else{
				if ( lf.id != -1 ) {
					lf.countError=count;
					altoPath[page]= lf;	
					page++;
					count = -1;   
				};
				count ++;
				lf.id = previous;
				
			}

			lf.idMets = sqlite3_column_int(pStmt,col++);
			lf.type = safe_sqlite3_column_text(pStmt, col++);
			lf.grouId = safe_sqlite3_column_text(pStmt, col++);
			lf.checksum = safe_sqlite3_column_text(pStmt, col++);
			lf.size =  sqlite3_column_double(pStmt,col++);
			lf.fileName = safe_sqlite3_column_text(pStmt, col++);
			lf.fileId = safe_sqlite3_column_text(pStmt, col++);
			lf.dpi = sqlite3_column_int(pStmt,col++);

			if ( sqlite3_column_text ( pStmt,col++ ) ){ 
				count ++;
			};
		}	
			lf.countError=count;
			altoPath[page]= lf;	
			page++;
   }else{
		raiseError(conn,selectSql);
	}
   sqlite3_finalize(pStmt);
   return altoPath;
}


/// <summary>get the total of mets per year</summary>
/// <param name="id_testset">id_testset</param>
/// <returns>map year - pair total mets,total Supplement</returns>
std::map<int,std::pair<int,int>> dbrequest::getSumMetsYear(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);

    sqlite3_stmt *pStmt;
		
	const char *zErrMsg= 0; 
   MetsFile mets;  
   const std::string selectSql = "select year,count(year) from Mets where id_testset =? group by year";  
   
   int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
   std::map<int,std::pair<int,int>>  mapYearCount;
   if(rc == SQLITE_OK)
   {
	   sqlite3_bind_int(pStmt,1,id_testset);	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{	
			int count = sqlite3_data_count(pStmt);	
			int year =0;	
			std::pair<int,int> pair;
			for (int i =0;i<  count ;i++)
			{			
				const char *result = safe_sqlite3_column_text(pStmt, i);
				if (result!=NULL)
				{			
					if (i==0) year = atoi(result);											
					else if (i==1)pair.first = atoi(result);
				}
			}	
			if (year!=0)
			{		
				pair.second = getSumSupplYear(id_testset,year);			
				mapYearCount[year]  = pair;
			}			
		}
   }else{
		raiseError(conn,selectSql);
	}
   sqlite3_finalize(pStmt);
   return mapYearCount;
}

/// <summary>get the total of supplements</summary>
/// <param name="id_testset">id_testset</param>
/// <param name="year">current year</param>
/// <returns>sum of supllement for the year</returns>
int dbrequest::getSumSupplYear(int id_testset,int year)
{
	int numberSuppl =0;
	std::stringstream oyear;
	oyear << year;

	ConnectionDB* conn = g_pool.getConnection(databaseName);	


	sqlite3_stmt *pStmt;	
	const char *zErrMsg= 0;    
	static std::string selectSql = "select count(a.title_supplement) from SUPPLEMENTS a, METS b"
		                    " where a.ID_METS = b.ID_METS and b.id_testset =?"
							" and b.year = ? and a.title_supplement !='' group by b.year";  

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	if(rc == SQLITE_OK){	  
		sqlite3_bind_int(pStmt,1,id_testset);
		sqlite3_bind_text(pStmt,2,oyear.str().c_str(),oyear.str().length(),SQLITE_STATIC);

		while( sqlite3_step(pStmt) == SQLITE_ROW ){							
			numberSuppl = sqlite3_column_int(pStmt,0);			 
		}
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return numberSuppl;
}

/// <summary>get comments of specific date</summary>
/// <param name="id_testset">id_testset</param>
/// <returns>map of date,DateComment </returns>
std::map<QDate,DateComment> dbrequest::getDateComment(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
		
	std::map<QDate,DateComment>  mapComment;
    sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
    DateComment dateComment;  

	static std::string selectSql = "select dc.ID,dc.ID_DATEERROR,dc.DATE,dc.COMMENT  from DATECOMMENT dc, dateerror de"
		                           " where dc.ID_DATEERROR = de.Id and id_TESTSET=?"; 
   
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_int(pStmt,1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col=0;
			dateComment.id=sqlite3_column_int(pStmt,col++);	
			dateComment.id_DateError=sqlite3_column_int(pStmt,col++);
			dateComment.date= dateComment.date.fromString(safe_sqlite3_column_text(pStmt, col++),"yyyy-MM-dd");
			dateComment.comment= safe_sqlite3_column_text(pStmt, col++);
		
			mapComment[dateComment.date] = dateComment;	 
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return mapComment;
}

/// <summary>get vector of date error found by mets verifier</summary>
/// <param name="id_testset">id_testset</param>
/// <returns>vector with pair id,DateError </returns>
std::vector<std::pair<int,DateError> > dbrequest::getvDateError(int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);

    sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	DateError derror;
	std::vector<std::pair<int,DateError>>  vDateError;
	const std::string selectSql = "SELECT ID,ID_TESTSET,DATE_BEGIN,DATE_END,ISSUES,COMMENT,ID_ERRORTYPE FROM DATEERROR where ID_TESTSET =?";

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{
		sqlite3_bind_int(pStmt,1,id_testset);
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		int count = sqlite3_data_count(pStmt);
		std::pair<int,DateError> p;
		 for (int i =0;i<  count ;i++)
		 {
			const char *result = safe_sqlite3_column_text(pStmt, i);
			
			if (i==0)
			{ 
				derror.id = atoi(result);
				p.first = atoi(result);
			}
							
			else if (i==1) derror.id_testset = atoi(result);
			else if (i==2) derror.dateBegin = result;
			else if (i==3) derror.dateEnd = result;
			else if (i==4)			
			{
				derror.issues = result;
			//	derror.vectIssueNumber = getIssueNumber(result);
			}
			else if (i==5) derror.comment = result;
			else if (i==6) derror.valider = result;		
			else if (i==7) derror.errorType = atoi(result);			
		 }
		 
		derror.comment = messageMisisngIssue(derror);
		 
		p.second = derror;
		 vDateError.push_back(p);
		 
	  }
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
   return vDateError;
}


///<summary>update comment of calendar</summary>
void dbrequest::updateDateComment(int id,std::string comment)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	char *zErrMsg =0;
	std::stringstream o;
	o << id;	
	std::string sql =	"UPDATE DATECOMMENT SET COMMENT ='" + comment + "' where ID = '"+ o.str() +"'";					
		
	int rc = sqlite3_exec(conn->db, sql.c_str(), NULL, 0, &zErrMsg); //null because no calll back needed
	
	if( rc )
	{
		fprintf(stderr, "Can't update data COMMENTS: %s\n",  zErrMsg);
	}
}


//! insert comment of calendar
bool dbrequest::insertComment(int id_mets,std::string date, std::string comment)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::stringstream id;
	
	id << id_mets;
	char *zErrMsg=0;
	
	std::string sql = "INSERT INTO DATECOMMENT ('ID_DATEERROR','DATE','COMMENT')  \
					  VALUES  ('" + id.str() + "','" + date + "','" + comment +"')"; 							
	
	int rc = sqlite3_exec(conn->db, sql.c_str(), NULL, 0, &zErrMsg); 	
	if( rc )
	{
		fprintf(stderr, "Can't insert data TestSet: %s\n",zErrMsg);
		return false;
	}	
	return true;
}

/// <summary>count of erros of each errotype</summary>
/// <param name="idTestset">id testset</param>
/// <param name="errortype">id errortype</param>
/// <returns>int</returns>
int dbrequest::getcountMetsErrorForEachErrorType(int idTestset,int errortype)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	MetsError es;
	sqlite3_stmt *pStmt;	
	
	const char *zErrMsg= 0; 	
	set<int> pileMets;
	std::string selectSql = "select distinct (id_related) from MetsError where ID_TESTSET = ? and id_errortype = ?";
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

   if(rc == SQLITE_OK)
   {	   
	   sqlite3_bind_int(pStmt,1,idTestset);
		sqlite3_bind_int(pStmt,2,errortype);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		  {
			  int res = sqlite3_column_int(pStmt, 0);
				if (pileMets.find(res) == pileMets.end())
				{
					pileMets.insert(res);
				}
			
		}
	} else{
		raiseError(conn,selectSql);
	}
    sqlite3_finalize(pStmt);
	std::string selectSqlLinked = "select distinct (ID_METS) from MetsError S,LINKEDFILES l where s.ID_TESTSET =? and RELATED_TYPE = 'LINKEDFILES' and s.id_related = l.id and id_errortype = ?";
	
	 rc = sqlite3_prepare_v2(conn->db, selectSqlLinked.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	
	    sqlite3_bind_int(pStmt,1,idTestset);
		sqlite3_bind_int(pStmt,2,errortype);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		 {
			 int res = sqlite3_column_int(pStmt, 0);
	
				if (pileMets.find(res) == pileMets.end())
				{
					pileMets.insert(res);
				}
			
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return pileMets.size();
}

/// <summary>count of schema Errors except dates</summary>
/// <param name="id">id testset</param>
/// <returns>int</returns>
int dbrequest::getcountMetsError(int idTestset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	MetsError es;
	sqlite3_stmt *pStmt;	
	std::stringstream sId_testset;	
	sId_testset << idTestset; 		
	
	const char *zErrMsg= ""; 	
	set<int> pileMets;
	std::string selectSql = "select distinct (id_related) from MetsError where ID_TESTSET = ?";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

   if(rc == SQLITE_OK)
   {
	   sqlite3_bind_int(pStmt,1,idTestset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		  {
			int count = sqlite3_data_count(pStmt);	
			    
			for (int i =0;i<  count ;i++)
			{	
				const char *result = safe_sqlite3_column_text(pStmt, i);						
				if (pileMets.find(atoi(result)) == pileMets.end())
				{
					pileMets.insert(atoi(result));
				}
			}
		}
	}else{
		raiseError(conn,selectSql);
	}
   sqlite3_finalize(pStmt);
  
	std::string selectSqlLinked = "select distinct (ID_METS) from MetsError S,LINKEDFILES l where s.ID_TESTSET =? and RELATED_TYPE = 'LINKEDFILES' and s.id_related = l.id ";
	
	rc = sqlite3_prepare_v2(conn->db,selectSqlLinked.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt,1,idTestset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		 {
			int count = sqlite3_data_count(pStmt);		    
			for (int i =0;i<  count ;i++)
			{
				const char *result = safe_sqlite3_column_text(pStmt, i);		
				if (pileMets.find(atoi(result)) == pileMets.end())
				{
					pileMets.insert(atoi(result));
				}
			}
		}
	}else{
		raiseError(conn,selectSqlLinked);
	}
	sqlite3_finalize(pStmt);
	return pileMets.size();
}

/// <summary>count number of titles to check </summary>
/// <param name="id">id testset</param>
/// <returns>int count</returns>
int dbrequest::getcountTitleCheck(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	int count =0;
	const char *zErrMsg= 0; 
	std::string selectSql = "select count(a.id) from article a, mets m where mustcheck = 1 and a.id_mets = m.id_mets and m.id_testset = ? "; 
	
	std::vector<ErrorType> v;
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorType et;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id_testset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{						
			count = sqlite3_column_int(	pStmt, 0);
		}		
    }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return count;
}

void dbrequest::getAllvError(std::map<int,std::vector<MetsError>*>& ret, int idTestset){

	ConnectionDB* conn = g_pool.getConnection(databaseName);
	MetsError es;
	sqlite3_stmt *pStmt;
	int category;
	
	const char *zErrMsg= 0; 	
	std::string selectSql = "select s.ID,s.ID_RELATED,s.RELATED_TYPE,s.FILE_PART,s.ERRORLINE,s.ERRORCOLUMN,"
		"s.MESSAGE,s.ID_ERRORTYPE,esv.GRAVITY,e.ERROR,s.id_search,s.HASHKEY,b.VALUE,l.FILENAME,mm.PATH,mm.year,mm.FILENAME,e.ID_CATEGORY "
		"from MetsError s,ERRORTYPE e,ERRORSEVERITY esv, METS  mm"
		" LEFT JOIN ACCEPTEDERROR b ON b.HASHKEY = s.HASHKEY"
		" LEFT JOIN LINKEDFILES l ON s.ID_RELATED = l.ID_METS AND s.FILE_PART = l.FILEID"
		" where s.ID_ERRORTYPE = e.ID_TYPE and s.ID_TESTSET = ? "
		" AND s.ID_RELATED = mm.ID_METS"
		" AND e.ID_SEVERITY=esv.ID_SEVERITY";
	

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

    if(rc == SQLITE_OK)
    {	
		sqlite3_bind_int(pStmt,1,idTestset);

      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		  int col = 0;
		  es.id = sqlite3_column_int(pStmt,col++);
 							
			es.idRelatedFile = sqlite3_column_int(pStmt,col++);
			es.relatedType = safe_sqlite3_column_text(pStmt,col++);
			es.filePart =safe_sqlite3_column_text(pStmt,col++);
			es.errorLine = sqlite3_column_int(pStmt,col++);
			es.errorColumn = sqlite3_column_int(pStmt,col++);
			es.message = safe_sqlite3_column_text(pStmt,col++);
			es.errorType.id_type = sqlite3_column_int(pStmt,col++);
			es.errorType.severity.gravity = safe_sqlite3_column_text(pStmt,col++);
			es.errorType.error = safe_sqlite3_column_text(pStmt,col++);
			es.id_search = safe_sqlite3_column_text(pStmt,col++);
			es.hashkey = safe_sqlite3_column_text(pStmt,col++);
			es.accepted = sqlite3_column_int(pStmt,col++);
			es.filenameShort = safe_sqlite3_column_text(pStmt,col++);
			es.filenameFullPath = std::string(safe_sqlite3_column_text(pStmt,col++)) + es.filenameShort;
			es.mets.year = sqlite3_column_int(pStmt,col++);
			es.mets.fileName = safe_sqlite3_column_text(pStmt,col++);
			category= sqlite3_column_int(pStmt,col++);

			if (ret.find(category) == ret.end()){
				std::vector<MetsError> *v = new std::vector<MetsError>();
				v->push_back(es);
				ret[category]=v;
			}else{
				(*ret.find(category)).second->push_back(es);
			};
	  }
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
}

/// <summary>get a vector of Mets errors </summary>
/// <param name="id_cat">id category</param>
/// <param name="id_testset">id testset</param>
/// <param name="year">year</param>
/// <returns>vector of mets error</returns>
std::vector<MetsError> dbrequest::getvErrorPerCategory(int id_cat, int idTestset){
	std::vector<MetsError> v;
	getvErrorPerCategory_METS(v,id_cat,idTestset);
	getvErrorPerCategory_LINKED(v,id_cat,idTestset);
	return v;
}
void dbrequest::getvErrorPerCategory_LINKED(std::vector<MetsError>& v, int id_cat, int idTestset){
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	MetsError es;
	sqlite3_stmt *pStmt;	
	
	const char *zErrMsg= 0; 	
	std::string selectSql = "select s.ID,s.ID_RELATED,s.RELATED_TYPE,s.FILE_PART,s.ERRORLINE,s.ERRORCOLUMN,"
		"s.MESSAGE,s.ID_ERRORTYPE,s.id_search,s.HASHKEY,b.VALUE,l.FILENAME,mm.PATH,mm.year,mm.FILENAME "
		"from MetsError s,ERRORTYPE e, LINKEDFILES l, METS  mm"
		" LEFT JOIN ACCEPTEDERROR b ON b.HASHKEY = s.HASHKEY"
		" where s.ID_ERRORTYPE = e.ID_TYPE and e.ID_CATEGORY = ? and s.ID_TESTSET = ? "
		" AND s.RELATED_TYPE != 'METS' "
		" AND s.ID_RELATED = l.ID_METS "
		" AND s.FILE_PART = l.FILEID"
		" AND s.ID_RELATED = mm.ID_METS";
	

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

    if(rc == SQLITE_OK)
    {	
		sqlite3_bind_int(pStmt,1,id_cat);
		sqlite3_bind_int(pStmt,2,idTestset);

      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		  int col = 0;
		  es.id = sqlite3_column_int(pStmt,col++);
 							
			es.idRelatedFile = sqlite3_column_int(pStmt,col++);
			es.relatedType = safe_sqlite3_column_text(pStmt,col++);
			es.filePart =safe_sqlite3_column_text(pStmt,col++);
			es.errorLine = sqlite3_column_int(pStmt,col++);
			es.errorColumn = sqlite3_column_int(pStmt,col++);
			es.message = safe_sqlite3_column_text(pStmt,col++);
			es.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			es.id_search = safe_sqlite3_column_text(pStmt,col++);
			es.hashkey = safe_sqlite3_column_text(pStmt,col++);
			es.accepted = sqlite3_column_int(pStmt,col++);
			es.filenameShort = safe_sqlite3_column_text(pStmt,col++);
			es.filenameFullPath = std::string(safe_sqlite3_column_text(pStmt,col++)) + es.filenameShort;
			es.mets.year = sqlite3_column_int(pStmt,col++);
			es.mets.fileName = safe_sqlite3_column_text(pStmt,col++);

		 v.push_back(es);		 
	  }
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);

}
void dbrequest::getvErrorPerCategory_METS(std::vector<MetsError>& v, int id_cat, int idTestset)
{
	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	MetsError es;
	sqlite3_stmt *pStmt;	
	
	const char *zErrMsg= 0; 	
	std::string selectSql = "select s.ID,s.ID_RELATED,s.RELATED_TYPE,s.FILE_PART,s.ERRORLINE,"
		"s.ERRORCOLUMN,s.MESSAGE,s.ID_ERRORTYPE,s.id_search,s.HASHKEY,b.VALUE,a.filename,a.path,a.year,a.FILENAME "
		"from MetsError s,ERRORTYPE e, METS a"
		" LEFT JOIN ACCEPTEDERROR b ON b.HASHKEY = s.HASHKEY"
		" where s.ID_ERRORTYPE = e.ID_TYPE and e.ID_CATEGORY = ? and s.ID_TESTSET = ?"
		" AND s.RELATED_TYPE = 'METS' "
		" AND a.ID_METS = s.ID_RELATED ";
	

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

    if(rc == SQLITE_OK)
    {	
		sqlite3_bind_int(pStmt,1,id_cat);
		sqlite3_bind_int(pStmt,2,idTestset);

		while(sqlite3_step(pStmt) == SQLITE_ROW){
			int col = 0;
			es.id = sqlite3_column_int(pStmt,col++);

			es.idRelatedFile = sqlite3_column_int(pStmt,col++);
			es.relatedType = safe_sqlite3_column_text(pStmt,col++);
			es.filePart =safe_sqlite3_column_text(pStmt,col++);
			es.errorLine = sqlite3_column_int(pStmt,col++);
			es.errorColumn = sqlite3_column_int(pStmt,col++);
			es.message = safe_sqlite3_column_text(pStmt,col++);
			es.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			es.id_search = safe_sqlite3_column_text(pStmt,col++);
			es.hashkey = safe_sqlite3_column_text(pStmt,col++);
			es.accepted = sqlite3_column_int(pStmt,col++);
			es.filenameShort = safe_sqlite3_column_text(pStmt,col++);
			es.filenameFullPath = std::string(safe_sqlite3_column_text(pStmt,col++)) + '/' + es.filenameShort;
			es.mets.year = sqlite3_column_int(pStmt,col++);
			es.mets.fileName = safe_sqlite3_column_text(pStmt,col++);

			v.push_back(es);		 
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);

}

/// <summary>get a vector of all the errortype found for a specific category of the testset </summary>
/// <param name="id_cat">id_cat</param>
/// <param name="id_testset">id testset</param>
/// <returns>int count</returns>
std::vector<ErrorType> dbrequest::getDistinctErrorType(int id_cat,int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::stringstream oid,oidcat;
	std::vector<ErrorType> v;
	oid << id_testset;	
	oidcat << id_cat;
	sqlite3_stmt *pStmt;	
	const char *zErrMsg= 0;    
	std::string selectSql = "select distinct(ID_TYPE) from ERRORTYPE e,MetsError s where s.ID_ERRORTYPE = e.id_type and id_testset ='" + oid.str() + "'and e.ID_CATEGORY = '" + oidcat.str() + "'";  
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	std::map<int,int> mapYearCount;
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{				
			const char *result = safe_sqlite3_column_text(pStmt, 0);			
			v.push_back(getErrorTypeWithId(atoi(result)));		 
		}
   } else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
   return v;
}

/// <summary>get a vector of Errorcategory</summary>	
/// <returns>vector of Errorcategory</returns>	
std::map<int,ErrorCategory> dbrequest::getErrorCategory()
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::map<int,ErrorCategory> ret;
	ErrorCategory ec;	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= "";    
    std::string selectSql = "select ID,ID_CATEGORY,NAME from ERRORCATEGORY";  

    int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

    if(rc == SQLITE_OK)
    {	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{				
				int col = 0;
				ec.id= sqlite3_column_int(pStmt, col++);	 							
				ec.id_category = sqlite3_column_int(pStmt, col++);
				ec.name = safe_sqlite3_column_text(pStmt, col++);					
			
			ret[ec.id]=ec;		 
		}
   } else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
   return ret;
}

/// <summary>get DateError found by the metsverfier</summary>
/// <param name="id_testset">id testset</param>
/// <returns>vector of DateError</returns>
std::vector<DateError> dbrequest::getDateError(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	
	DateError de;
	std::vector<DateError> v;
    sqlite3_stmt *pStmt;
	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT de.id,de.id_testset,de.date_begin,de.date_end, de.issues,de.comment,de.id_errortype,de.hashkey,b.value "
		                    "FROM DATEERROR de "
							"LEFT JOIN ACCEPTEDERROR b ON b.HASHKEY = de.HASHKEY "
	                        "where de.ID_testset =?";

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK){

		sqlite3_bind_int(pStmt,1,id_testset);
		
		while(sqlite3_step(pStmt) == SQLITE_ROW){

			  int col=0;
			  de.id = sqlite3_column_int(pStmt,col++);
			  de.dateComment = getDateCommentid(de.id);
			  de.id_testset = sqlite3_column_int(pStmt,col++); // silly it was the condition
			  de.dateBegin = safe_sqlite3_column_text(pStmt, col++);
			  de.dateEnd = safe_sqlite3_column_text(pStmt, col++);
			  de.issues = safe_sqlite3_column_text(pStmt, col++);
			  de.comment = safe_sqlite3_column_text(pStmt, col++);
			  de.errortype = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			  de.hashkey = safe_sqlite3_column_text(pStmt, col++);
			  de.accepted = sqlite3_column_int(pStmt,col++);
	
			  v.push_back(de);
		}
	}else{
			raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

/// <summary>fill combobox of type of date category</summary>
/// <param name="id_cat">id cat</param>
/// <param name="id_testset">id_testset</param>
/// <returns>vector of DateError</returns>
std::vector<std::string> dbrequest::getDistinctErrorTypeDateError(int id_cat,int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::stringstream oid,oidcat;
	std::vector<std::string> v;
	oid << id_testset;	
	oidcat << id_cat;	
    sqlite3_stmt *pStmt;	
	const char *zErrMsg= 0;    
    std::string selectSql = "select distinct(Error) from ERRORTYPE e,DATEERROR s where s.ID_ERRORTYPE = e.id_type ";  
	DEBUG_ME
    int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	std::map<int,int> mapYearCount;
    if(rc == SQLITE_OK)
    {	  
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {			
		const char *result = safe_sqlite3_column_text(pStmt, 0);			
		v.push_back(result);		 
	  }
   } else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
   return v;
}

/// <summary>get comment of a date of the selected date</summary>
/// <param name="idError">idError</param>
/// <returns>vector of DateComment of the current DateError</returns>

std::vector<DateComment> dbrequest::getDateCommentid(int idError)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	DateComment dc;
	std::vector<DateComment> v;
    sqlite3_stmt *pStmt;	
	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT dc.id, dc.id_dateerror, dc.date, dc.comment FROM DATECOMMENT dc where id_dateerror =?" ;

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK)
	{	  
	  sqlite3_bind_int(pStmt,1,idError);
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		  int col = 0;
		  dc.id = sqlite3_column_int(pStmt, col++);	
		  dc.id_DateError = sqlite3_column_int(pStmt, col++);
		  dc.date = dc.date.fromString(safe_sqlite3_column_text(pStmt, col++),"yyyy-MM-dd");
		  dc.comment = safe_sqlite3_column_text(pStmt, col++);	
	
		  v.push_back(dc);	 
	  }
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}


void dbrequest::deleteStructureErrorId(int id)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	const char *zErrMsg= 0; 
	std::string selectSql = "DELETE FROM STRUCTUREERROR where ID = ?"; 

	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id);

		if ( sqlite3_step(pStmt)!= SQLITE_DONE ) raiseError(conn,selectSql);
	
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);

}



StructureError dbrequest::getStructureErrorId(int id)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT ID,ID_METS,IMAGEPATH,MESSAGE,ID_ERRORTYPE,FILEID,CUSTOM,PAGENB FROM STRUCTUREERROR where ID = ?"; 

	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	StructureError se;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{   int col=0;
			se.id = sqlite3_column_int(pStmt,col++);
			se.id_mets = sqlite3_column_int(pStmt,col++);
			se.pathImage = safe_sqlite3_column_text(pStmt, col++);
			se.message = safe_sqlite3_column_text(pStmt, col++);
			se.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			se.fileid = safe_sqlite3_column_text(pStmt, col++);
			se.custom = safe_sqlite3_column_text(pStmt, col++);
			se.pagenb = sqlite3_column_int(pStmt, col++);		 
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return se;
}

int dbrequest::getStructureErrorMaxId(int id_Mets)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT MAX(ID) FROM STRUCTUREERROR where ID_METS = ?"; 
	
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	int ret;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id_Mets);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{   int col=0;
			ret = sqlite3_column_int(pStmt,col++);
			
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return ret;
}

std::vector<StructureError> dbrequest::getStructureError(int id_Mets)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT ID,ID_METS,IMAGEPATH,MESSAGE,ID_ERRORTYPE,FILEID,CUSTOM,PAGENB FROM STRUCTUREERROR where ID_METS = ?"; 
	std::vector<StructureError> v;
	DEBUG_ME
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	StructureError se;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id_Mets);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{   int col=0;
			se.id = sqlite3_column_int(pStmt,col++);
			se.id_mets = sqlite3_column_int(pStmt,col++);
			se.pathImage = safe_sqlite3_column_text(pStmt, col++);
			se.message = safe_sqlite3_column_text(pStmt, col++);
			se.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			se.fileid = safe_sqlite3_column_text(pStmt, col++);
			se.custom = safe_sqlite3_column_text(pStmt, col++);
			se.pagenb = sqlite3_column_int(pStmt, col++);

			v.push_back(se); 			 
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

std::map<std::string,std::vector<StructureError> > dbrequest::getBatchStructureError(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	int rc;	
	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT a.ID,a.ID_METS,a.IMAGEPATH,a.MESSAGE,a.ID_ERRORTYPE,a.FILEID,a.CUSTOM,"
		                           "a.PAGENB,b.FILENAME FROM STRUCTUREERROR a, METS b where ID_TESTSET = ? AND a.ID_METS = b.ID_METS"; 
	std::map<std::string,std::vector<StructureError> > v;
	
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	StructureError se;  
	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_int(pStmt,1,id_testset);

		

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{   
		    int col=0;
			se.id = sqlite3_column_int(pStmt,col++);
			se.id_mets = sqlite3_column_int(pStmt,col++);
			se.pathImage = safe_sqlite3_column_text(pStmt, col++);
			se.message = safe_sqlite3_column_text(pStmt, col++);
			se.errorType = getErrorTypeWithId(sqlite3_column_int(pStmt,col++));
			se.fileid = safe_sqlite3_column_text(pStmt, col++);
			se.custom = safe_sqlite3_column_text(pStmt, col++);
			se.pagenb = sqlite3_column_int(pStmt, col++); 	
			std::string ptr =  std::string ( (char*)safe_sqlite3_column_text(pStmt, col++));
			v[ ptr ].push_back(se);			 
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}



std::vector<Sampling_Structure> dbrequest::getListSamplingStructure(int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
    sqlite3_stmt *pStmt;
		
	const char *zErrMsg= ""; 

	static std::string selectSql = "select ss.ID,ss.ID_METS,aa.ID_STATE from SAMPLING_STRUCTURE ss, METS m "
		                           " LEFT JOIN PROGRESSION_STATE aa"
								   " ON m.ID_METS = aa.ID_METS"
		                           " where ss.ID_METS = m.ID_METS and m.ID_TESTSET  =?";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	
	std::vector<Sampling_Structure>  ret;

	if(rc == SQLITE_OK){
		Sampling_Structure ss;
		sqlite3_bind_int(pStmt,1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW){
			int col=0;
			ss.id = sqlite3_column_int (pStmt,col++);
			ss.id_Mets = sqlite3_column_int (pStmt,col++);
			ss.checked = sqlite3_column_int (pStmt,col++);
			ss.checked = ss.checked == 2 ? 1 : 0;
			ss.Mets = getMets(ss.id_Mets);	
			ret.push_back(ss);
		}
	}else{
		raiseError(conn,selectSql);
	};
	sqlite3_finalize(pStmt);
	return ret;   
}

void dbrequest::updateSamplingStructure(int id,int checked)
{
	insertupdateProgress(id,checked==1?2:0);
}


///@Deprecated : only for old base
//
std::vector<ErrorType> dbrequest::_getErrorTypeCatStructure(std::string docType)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT * FROM ERRORTYPE where ID_CATEGORY = 6"; 
	std::vector<ErrorType> v;
	DEBUG_ME
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorType et;  
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			
			int count = sqlite3_data_count(pStmt);		
			for (int i =0;i<  count ;i++)
			{
				const char *result = safe_sqlite3_column_text(pStmt, i);			
				if (i==0) et.id = atoi(result);	
				else if (i==1) et.id_type = atoi(result);						
				else if (i==2) et.error = result;
				else if (i==3) et.details = result;
				else if (i==4) 
				{
					et.severity = getErrorSeverityWithId(atoi(result));
					et.id_severity = atoi(result);		
				}	
				else if (i==5) et.category = getErrorCategoryWithId(atoi(result));
						
			}
			v.push_back(et);				 
		}		
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

std::vector<ErrorType> dbrequest::getErrorTypeCatStructure(std::string docType)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT a.ID,a.ID_TYPE,a.ERROR,a.DETAILS,a.ID_SEVERITY,a.ID_CATEGORY"
		                    " FROM ERRORTYPE a ,ERRORSCREENSHOT b"
							" where a.ID_TYPE = b.ID_TYPE and a.ID_CATEGORY = 6 and b.DOCTYPE=?"; 
	std::vector<ErrorType> v;
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorType et;  
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_text(pStmt,1,docType.c_str(),docType.length(),SQLITE_STATIC);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;
			
			et.id =  sqlite3_column_int(pStmt, col++);	
			et.id_type = sqlite3_column_int(pStmt, col++);				
			et.error = std::string((const char*)sqlite3_column_text(pStmt, col++));
			et.details = std::string((const char*)sqlite3_column_text(pStmt, col++));
			et.id_severity = sqlite3_column_int(pStmt, col++);
			et.severity = getErrorSeverityWithId(et.id_severity);
			et.category = getErrorCategoryWithId(sqlite3_column_int(pStmt, col++));
						
			v.push_back(et);				 
		}		
   }else{
	   raiseError(conn,selectSql); // only for debug
	   return _getErrorTypeCatStructure(docType);
	}
	sqlite3_finalize(pStmt);
	return v;
}

bool dbrequest::saveStructError(int id_mets,std::string message,int idErrorType,std::string path, std::string fileID, std::string custom,int pagenb)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);

	const char *zErrMsg=0;
	sqlite3_stmt *pStmt;
	
	std::string sql = "INSERT INTO STRUCTUREERROR ('ID_METS','IMAGEPATH', 'MESSAGE','ID_ERRORTYPE','FILEID','CUSTOM','PAGENB') \
					  VALUES  (?,?,?,?,?,?,?)"; 							

	
	int rc = sqlite3_prepare_v2(conn->db,sql.c_str(),-1, &pStmt,&zErrMsg);
	if( rc != SQLITE_OK )
	{
		raiseError(conn,sql);
			return false;
	}
    
	sqlite3_bind_int(pStmt, 1,id_mets);
	sqlite3_bind_text(pStmt,2,path.c_str(),path.length(),SQLITE_STATIC);
	sqlite3_bind_text(pStmt,3,message.c_str(),message.length(),SQLITE_STATIC);
	sqlite3_bind_int(pStmt, 4,idErrorType);
	sqlite3_bind_text(pStmt, 5,fileID.c_str(),fileID.length(),SQLITE_STATIC);
	sqlite3_bind_text(pStmt, 6,custom.c_str(),custom.length(),SQLITE_STATIC);
	sqlite3_bind_int(pStmt, 7,pagenb);

	
	if (sqlite3_step(pStmt) != SQLITE_DONE) {
			raiseError(conn,sql);
			return false;
	}
	return true; 
}


std::vector<Title> dbrequest::getvTitle(int id_testset)
{	
	ConnectionDB* conn = g_pool.getConnection(databaseName);
	std::stringstream oIdTestset,oYear;
	oIdTestset << id_testset;		
    sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	Title title;
	std::vector<Title>  vTitle;
	std::string selectSql ="select a.id, m.id_mets,a.countcaracter,a.title from ARTICLE a,METS m "
		                   "where  m.id_mets = a.id_mets and m.id_testset = ? order by a.id ";
	DEBUG_ME
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  

		sqlite3_bind_int(pStmt,1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
		
			std::pair<int,DateError> p;
		
			int col = 0;

			title.id_article = sqlite3_column_int(pStmt, col++);
			title.article = getArticle(title.id_article);

			title.id_mets = sqlite3_column_int(pStmt, col++);
			title.mets = getMets(title.id_mets);

			title.countString = sqlite3_column_int(pStmt, col++);
			//title.countError = sqlite3_column_int(pStmt, col++);
			title.title = safe_sqlite3_column_text(pStmt, col++);

			vTitle.push_back(title);
		 
	  }
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
   return vTitle;
}


Article dbrequest::getArticle(int id)
{
Article a;

	ConnectionDB* conn = g_pool.getConnection(databaseName);

	std::stringstream oId;
	oId << id;	
	//oYear << year;
    sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	Article article;	
	std::string selectSql = "SELECT * FROM Article where ID ='" + oId.str() + "'";
	DEBUG_ME
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	  
      while(sqlite3_step(pStmt) == SQLITE_ROW)
      {
		int count = sqlite3_data_count(pStmt);
		std::pair<int,DateError> p;
		 for (int i =0;i<  count ;i++)
		 {
			const char *result = safe_sqlite3_column_text(pStmt, i);
			
			if (i==0)
			{ 
				article.id = atoi(result);				
			}
							
			else if (i==1) article.id_mets = atoi(result);
			else if (i==2) article.id_article = result;
			else if (i==3) article.div = result;
			else if (i==4)	article.check = atoi(result);		
			else if (i==5) article.title = result;
			else if (i==6) article.countcaracter = atoi(result);		
	//		else if (i==7) article.number = atoi(result);		
		 }		
		 
	  }
   }else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return article;
}

std::vector<std::vector<QVariant> > dbrequest::getAllMets(int id_testset,bool sampling){
	ConnectionDB* conn = g_pool.getConnection(BatchDetail::getBatchDetail().database);	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0;
	static QIcon icon("iconLoadThumb.bmp");
	static bool flagones = true;
	
	
	//std::string selectSql = "SELECT a.ID_METS, a.PATH, a.FILENAME, b.PAGENB FROM METS a LEFT JOIN STRUCTUREERROR b ON a.ID_METS = b.ID_METS WHERE a.ID_TESTSET=?";
	std::string selectSql = "SELECT ll.LABEL, a.ID_METS, a.PATH, a.FILENAME, c.UNIQUEBUILDKEY," 
		                    " c.TITLE,c.TYPE,c.UNIQUEBUILDKEY, c.UNIQUEBUILDKEY/*,c.CHECKED*/ FROM METS a"
							" LEFT JOIN (SELECT * FROM INVENTORY h, METSINVENTORY b WHERE b.UNIQUEBUILDKEY = h.UNIQUEBUILDKEY) c "
							" ON a.ID_METS = c.ID_METS"
							" LEFT JOIN PROGRESSION_STATE aa"
							" ON a.ID_METS = aa.ID_METS"
							" LEFT JOIN LABEL_STATE ll"
							" ON ll.ID_STATE = aa.ID_STATE"
							" WHERE a.ID_TESTSET=?";

	if ( sampling ) {

		selectSql = "SELECT ll.LABEL, a.ID_METS, a.PATH, a.FILENAME, c.UNIQUEBUILDKEY," 
		                    " c.TITLE,c.TYPE,c.UNIQUEBUILDKEY, c.UNIQUEBUILDKEY/*,c.CHECKED*/ FROM METS a, SAMPLING_STRUCTURE ss"
							" LEFT JOIN (SELECT * FROM INVENTORY h, METSINVENTORY b WHERE b.UNIQUEBUILDKEY = h.UNIQUEBUILDKEY) c "
							" ON a.ID_METS = c.ID_METS"
							" LEFT JOIN PROGRESSION_STATE aa"
							" ON a.ID_METS = aa.ID_METS"
							" LEFT JOIN LABEL_STATE ll"
							" ON ll.ID_STATE = aa.ID_STATE"
							" WHERE a.ID_TESTSET=? AND ss.ID_METS = a.ID_METS";
		
	}
	
	std::vector<std::vector<QVariant> > v;
retry:
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	

	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt, 1,id_testset);
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			std::vector<QVariant> row;
			int col=0;
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QVariant( sqlite3_column_int(pStmt, col++) ) );
			row.push_back(QString( (char*)sqlite3_column_text(pStmt, col++) ) );
			row.push_back(QString( (char*)sqlite3_column_text(pStmt, col++) ) );
			row.push_back( icon );
			//row.push_back(QString( (char*)sqlite3_column_text(pStmt, 3) ) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			//row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			//row.push_back(QString( (char*)sqlite3_column_text(pStmt, col++) ));
			//row.push_back(QString( (char*)sqlite3_column_text(pStmt, col++) ));
			//row.push_back(QString( (char*)sqlite3_column_text(pStmt, col++) ));


			v.push_back(row);
		}
	}else {
			if ( flagones ){ // probably table non existing so retry without compatibility mode with previous database
				flagones = false;
				selectSql = "SELECT a.ID_METS, a.PATH, a.FILENAME, 'XXXXX'," 
		                    " 'YYYYY', 'ZZZZZZZ','BBBBBB', 'SSSSSS' FROM METS a"
							" WHERE a.ID_TESTSET=?";
				sqlite3_finalize(pStmt);
				goto retry; // yes I know :-)

			}else{
				raiseError(conn,selectSql);
			}
		}
	sqlite3_finalize(pStmt);
	return v;
}


std::vector<std::vector<QVariant> > dbrequest::getAllBooks(int id_testset){
	ConnectionDB* conn = g_pool.getConnection(BatchDetail::getBatchDetail().database);	
	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0;
	
	
	static std::string selectSql = "SELECT  c.CHECKED, a.ID_TESTSET, c.LANGUAGES, c.UNIQUEBUILDKEY,c.TITLE, c.TYPE," 
		                    " c.FORMALDATE, c.UNIQUEBUILDKEY,c.UNIQUEBUILDKEY FROM "
							" INVENTORY c"
							" LEFT JOIN ( SELECT ID_TESTSET, UNIQUEBUILDKEY  FROM METS a, METSINVENTORY b WHERE a.ID_METS = b.ID_METS AND a.ID_TESTSET=? ) a"
							" ON a.UNIQUEBUILDKEY = c.UNIQUEBUILDKEY";
	
	std::vector<std::vector<QVariant> > v;

	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	

	if(rc == SQLITE_OK)
	{	  
		sqlite3_bind_int(pStmt, 1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			std::vector<QVariant> row;
			int col=0;
			row.push_back(QString( sqlite3_column_int(pStmt, col++)==1  ?"Checked":"" ));
			row.push_back(QString( id_testset == sqlite3_column_int(pStmt, col++) ? "Current Batch" : "Not" ) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );
			row.push_back(QString::fromUtf8((char*)sqlite3_column_text(pStmt, col++)) );


			v.push_back(row);
		}
	}else {
			
			raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;

}

std::vector<int> dbrequest::getMetsIdInPeriod(int id_testset, int year_from, int month_from, int year_to, int month_to)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "' AND DATE>='" << year_from << "-" << month_from << "01' AND DATE<'" << year_to << "-" << month_to << "01';";
	std::string selectSql = "SELECT ID_METS FROM METS WHERE " + clause.str();
	std::vector<int> v;
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	

	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int result = sqlite3_column_int(pStmt, 0);
			v.push_back(result);
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

std::vector<QDate> dbrequest::getMetsDateInPeriod(int id_testset, int year_from, int month_from, int year_to, int month_to)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "' AND DATE>='" << year_from << "-" << std::setw(2) << std::setfill('0') << month_from << "-01' AND DATE<'" << year_to << "-" << std::setw(2) << std::setfill('0') << month_to << "-01';";
	std::string selectSql = "SELECT DATE FROM METS WHERE " + clause.str();
	std::vector<QDate> v;
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	QDate temp;

	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			const char * result = safe_sqlite3_column_text(pStmt, 0);
			v.push_back(temp.fromString(result, "yyyy-MM-dd"));
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

std::vector<QDate> dbrequest::getMetsDateInMonth(int id_testset, int year, int month)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0;
	int year_to;
	int month_to;
	if (month == 12) {
		month_to = 1;
		year_to = year + 1;
	} else {
		month_to = month + 1;
		year_to = year;
	}
	
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "' AND DATE>='" << year << "-" << std::setw(2) << std::setfill('0') << month << "-01' AND DATE<'" << year_to << "-" << std::setw(2) << std::setfill('0') << month_to << "-01';";
	std::string selectSql = "SELECT DATE FROM METS WHERE " + clause.str();
	std::vector<QDate> v;
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	QDate temp;

	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			const char * result = safe_sqlite3_column_text(pStmt, 0);
			v.push_back(temp.fromString(result, "yyyy-MM-dd"));
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}
std::vector<QDate> dbrequest::getMetsDates(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "'";
	std::string selectSql = "SELECT DATE FROM METS WHERE " + clause.str();
	std::vector<QDate> v;
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	
	QDate temp;
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			const char * result = safe_sqlite3_column_text(pStmt, 0);
			v.push_back(temp.fromString(result, "yyyy-MM-dd"));
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return v;
}

// Get the number of (METS, supplements) per year
std::map<int ,std::pair<int, int> > dbrequest::GetYearSummary(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;
	
	const char *zErrMsg= 0; 
	
	// First get the number of METS files per year
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "' GROUP BY YEAR";
	std::string selectSql = "SELECT YEAR, COUNT(ID_METS) FROM METS WHERE " + clause.str();
	std::map<int ,std::pair<int, int> > v;
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	
	
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int resYear = sqlite3_column_int(pStmt, 0);
			int resCountMets = sqlite3_column_int(pStmt, 1);
			v[resYear].first = resCountMets;
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	
	// Now get the number of supplements per year
	clause.clear();
	clause.str(std::string());
	clause << "ID_TESTSET=" << id_testset << " AND S.ID_METS=M.ID_METS GROUP BY YEAR";
	selectSql = "SELECT M.YEAR, COUNT(S.ID_SUPPLEMENT) FROM METS M, SUPPLEMENTS S WHERE " + clause.str();
	DEBUG_ME
	rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	
	
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int resYear = sqlite3_column_int(pStmt, 0);
			int resCountSuppl = sqlite3_column_int(pStmt, 1);
			v[resYear].second = resCountSuppl;
		}
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);

	return v;
}

std::vector<MetsFile> *dbrequest::getMetsByDate(int id_testset, QDate date)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	// First get the number of METS files per year
	std::stringstream clause;
	clause << "ID_TESTSET='" << id_testset << "' AND DATE='" << qPrintable(date.toString("yyyy-MM-dd")) << "'";
	std::string selectSql = "SELECT ID_METS FROM METS WHERE " + clause.str();
	DEBUG_ME
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);	
	
	if(rc == SQLITE_OK)
	{	  
		std::vector<MetsFile> *result = new std::vector<MetsFile>;
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int id_mets = sqlite3_column_int(pStmt, 0);
			result->push_back(getMets(id_mets));
		}
		sqlite3_finalize(pStmt);
		return result;
	} else { // TODO analyse error
		raiseError(conn,selectSql);
		sqlite3_finalize(pStmt);
		return 0;
	}
}

std::vector<int> dbrequest::getReport(){
	ConnectionDB* conn = g_pool.getConnection(databaseName);
    sqlite3_stmt *pStmt;
		
	const char *zErrMsg= ""; 

	static std::string selectSql = "select checked , count() from inventory group by checked order by checked desc";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	
	std::vector<int>  ret;

	if(rc == SQLITE_OK){

		while(sqlite3_step(pStmt) == SQLITE_ROW){
			int col=0;
			col++; 
			int count = sqlite3_column_int (pStmt,col++);
				
			ret.push_back(count);
		}
	}else{
		raiseError(conn,selectSql);
	};
	sqlite3_finalize(pStmt);
	return ret;
}

std::string dbrequest::getFirstMetsFilename(int id_testset)
{
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT PATH, FILENAME FROM METS WHERE ID_TESTSET=? LIMIT 1";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	std::string res;
	
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_int(pStmt,1,id_testset);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			const char *path = safe_sqlite3_column_text(pStmt, 0);
			const char *filename = safe_sqlite3_column_text(pStmt, 1);
			res = path + std::string("/") + filename;
		}
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return res;
}

void dbrequest::insertupdateProgress(int metsid, int value){
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	static std::string selectSql = "UPDATE PROGRESSION_STATE SET ID_STATE = ? WHERE ID_METS = ? ";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_int(pStmt,1,value);
		sqlite3_bind_int(pStmt,2,metsid);

		rc = sqlite3_step(pStmt) ;  

		if ( rc == SQLITE_DONE ) {
			int count = sqlite3_changes(conn->db);

			if ( count == 0 ){
				sqlite3_finalize(pStmt);
				static std::string selectSql2 = "INSERT INTO PROGRESSION_STATE ('ID_STATE','ID_METS')  VALUES  (?,?);";
				rc = sqlite3_prepare_v2(conn->db,selectSql2.c_str(),-1, &pStmt,&zErrMsg);
					if(rc == SQLITE_OK)
					{	
						sqlite3_bind_int(pStmt,1,value);
						sqlite3_bind_int(pStmt,2,metsid);

						if ( sqlite3_step(pStmt)!=SQLITE_DONE ) {
							raiseError(conn,selectSql2);
						}

					}else{
						raiseError(conn,selectSql2);
					}
			}
		}
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return;

}

void dbrequest::insertAccepted (std::string hashkey){
	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	static std::string selectSql = "INSERT INTO ACCEPTEDERROR ( HASHKEY, VALUE ) VALUES (?,?) ";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_text(pStmt,1,hashkey.c_str(),hashkey.length(),SQLITE_STATIC);
		sqlite3_bind_int(pStmt,2,1);

		if ( sqlite3_step(pStmt)!=SQLITE_DONE ) raiseError(conn,selectSql);
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return;
}

void dbrequest::deleteAccepted (std::string hashkey){
		ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	static std::string selectSql = "DELETE FROM ACCEPTEDERROR WHERE HASHKEY = ?";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_text(pStmt,1,hashkey.c_str(),hashkey.length(),SQLITE_STATIC);

		if ( sqlite3_step(pStmt)!= SQLITE_DONE ) raiseError(conn,selectSql);
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
	return;
}

std::vector <std::pair < string , int > > & dbrequest::loadLabel(){

	static std::vector <std::pair < string , int > > cache;

	if ( cache.size() != 0 ) return cache;

	ConnectionDB* conn = g_pool.getConnection(databaseName);
    sqlite3_stmt *pStmt;
		
	const char *zErrMsg= ""; 

	static std::string selectSql = "select LABEL,ID_STATE from LABEL_STATE order by RANK";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK){

		while(sqlite3_step(pStmt) == SQLITE_ROW){
			int col=0;
			std::pair<std::string,int > p;
			p.first= safe_sqlite3_column_text (pStmt,col++);
			p.second = sqlite3_column_int (pStmt,col++);
			cache.push_back(p);
		}
	}else{
		raiseError(conn,selectSql);
	};
	sqlite3_finalize(pStmt);
	return cache;

}

std::map < string , QColor >& dbrequest::loadColor(std::string schma){

	static std::map < string , QColor > empty;
	static std::map < string , std::map < string , QColor > > cache;

	if ( cache.size() == 0 ) _loadColor ( cache );

	std::map < string , std::map < string , QColor > > ::iterator it =  cache.find(schma);

	if ( it == cache.end() ) { return empty; }
	return it->second;
}

void  dbrequest::_loadColor(std::map < string , std::map < string , QColor > >& toFill){


	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT RCOLOR,GCOLOR,BCOLOR,ENTITYNAME, DOCTYPE FROM ENTITYCONFIGURATION WHERE USEDNAMALYS=1";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;
			int r =  sqlite3_column_int(pStmt, col++);
			int g =  sqlite3_column_int(pStmt, col++);
			int b =  sqlite3_column_int(pStmt, col++);

			std::string entity = safe_sqlite3_column_text(pStmt, col++);
			std::string doctype = safe_sqlite3_column_text(pStmt, col++);

			toFill [doctype] [entity] = QColor(r,g,b);
		}
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
}

void  dbrequest::loadEntityCount(std::map<std::string, std::map < string , int > >& toFill, std::map< string,int>& headerData){


	ConnectionDB* conn = g_pool.getConnection(databaseName);	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT b.FILENAME,a.ENTITY,a.TOT FROM METS b, METSDIVCOUNT a  WHERE a.ID_METS = b.ID_METS and b.ID_TESTSET = ?";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		
		sqlite3_bind_int(pStmt, 1, BatchDetail::getBatchDetail().idTestSet);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;
			std::string filename =   safe_sqlite3_column_text(pStmt, col++);

			std::string entity = safe_sqlite3_column_text(pStmt, col++);
			int count =  sqlite3_column_int(pStmt, col++);
			

			toFill [filename] [entity] = count;
			headerData[entity] += count;
		}
		
	}else{
		raiseError(conn,selectSql);
	}
	sqlite3_finalize(pStmt);
}

void  dbrequest::_loadEntityTitleCorrection(std::map < std::string , std::map < std::string , std::map < std::string ,int > > > & toFill){
	sqlite3_stmt *pStmt;
	ConnectionDB* conn = g_pool.getConnection(databaseName);

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT a.ENTITYNAME, a.DOCTYPE, b.ENTITYNAMELINK FROM ENTITYCONFIGURATION a LEFT JOIN ENTITYCONFIGURATIONLINK b on a.ID_ENTITY = b.ID_ENTITY"
		                    " WHERE USEFORTITLECORRECTION=1";
	
	int rc = sqlite3_prepare_v2(conn->db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;

			std::string entity = safe_sqlite3_column_text(pStmt, col++);
			std::string doctype = safe_sqlite3_column_text(pStmt, col++);
			std::string entitylink = safe_sqlite3_column_text(pStmt, col++);

			toFill [doctype] [entity] [entitylink] = 1;
		}
		
	}else{
		raiseError(conn,selectSql);	
	}
	sqlite3_finalize(pStmt);
}


static std::map < std::string , std::map < std::string , std::map < std::string ,int > > > cacheEntityToTitleCorrection;

bool dbrequest::isEntityToTitleCorrection(std::string type,std::string entity){

	if ( cacheEntityToTitleCorrection.size() == 0 ) _loadEntityTitleCorrection ( cacheEntityToTitleCorrection );


	std::map < std::string , std::map < std::string ,int > >::iterator it = cacheEntityToTitleCorrection[type].find(entity);

	if ( it != cacheEntityToTitleCorrection[type].end() ) return true;

	return false;
}

bool dbrequest::isEntityToTitleCorrectionLink(std::string type,std::string entity,std::string entityLink){

	if ( cacheEntityToTitleCorrection.size() == 0 ) _loadEntityTitleCorrection ( cacheEntityToTitleCorrection );


	std::map < std::string , std::map < std::string ,int > >::iterator it = cacheEntityToTitleCorrection[type].find(entity);

	if ( it == cacheEntityToTitleCorrection[type].end() ) return false;

	std::map < std::string ,int >::iterator itt = it->second.find(entityLink);

	if (itt != it->second.end() ) return true;

	return false;

}