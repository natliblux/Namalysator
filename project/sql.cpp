#include "sql.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "../common_files/utilities.h"
#include "datehelper.h"
#include "metsverifier.h"
#include "errorhandler.h"
#include "metsparser.h"
#include "md5wrapper.h"

#pragma warning(disable:4503)
#pragma warning(disable:4244)

typedef std::map<std::string,std::vector<std::string> > my_map_type;
md5wrapper g_md5;

database::database(const std::string &batch,const std::string &dataBase,const std::string &logFile)
{
	batchName = batch;
	databaseName= dataBase;	
	logFilePath = logFile;
}

//! open Db
bool database::openDB()
{	
	int rc = sqlite3_open(databaseName.c_str(), &db);
	if( rc )
	{
		fprintf(stderr, "Can not open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}	
	return true;
}

void database::closeDB()
{
	sqlite3_close(db);
	db = 0;
}

//! set current id from Testset
void database::setIdTestSet()
{
	idTestset =  getIdTestSet(batchName);
	std::stringstream o;
	o << idTestset;
	stringIdTestSet = o.str();	
}

//! functon to create database with the path as input
bool database::createTable(const std::string &filePath)
{
	std::vector<std::string> vect;
	ifstream myfile;
	string::size_type found;
	char *zErrMsg = 0;
	openDB();
	string sql="";
	string value = ");";	
	string line;
	myfile.open (filePath.c_str());
	const char *sqlCreate;

	if(myfile.fail())
	{			
		myfile.close(); 
		return false;
	}
	else
	{
		while (!myfile.eof())
		{	
			getline (myfile,line);		
			found = line.find(value);
			sql = sql + line; 
			if (found != string::npos)
			{
				vect.push_back(sql);				
				sql ="";
			}	
		}
		myfile.close(); 

		for (size_t i=0;i < vect.size();i++)
		{
			sqlCreate = vect[i].c_str();
			int rc = sqlite3_exec(db, sqlCreate, NULL, 0, &zErrMsg);

			if (rc!=SQLITE_OK )
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);			
				sqlite3_free(zErrMsg);		
				return false;
			}		
		}
		insertLog("New database created");	
	}
	closeDB();
	return true;
}

//! insert into the table TestSet the name of batch and the current datetime
//! \param batchName name of the Batch
//! \param datetime current time of Test
//! \return true if there's no errors, otherwise FALSE  the program will exit
bool database::insertTestSet(const std::string &batchName,const std::string &subbatchName,const std::string &datetime)
{	
	char *zErrMsg=0;
	std::string sql = "INSERT INTO TESTSET ('BATCHNAME','SUBBATCHNAME', 'DATE') \
					  VALUES  ('" + batchName + "','"+ subbatchName+ "','"+ datetime + "')"; 							

	int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); //null because no calll back needed	
	if( rc )
	{		
		std::stringstream ss;
		ss <<  "Can not insert data into table TESTSET: " << zErrMsg ;		
		return false;
	}
	if ( zErrMsg ) { sqlite3_free(zErrMsg);};
	//set the current id from the Testset
	setIdTestSet();		
	return true;
}
//! get MAX id from testset
int database::getIdTestSet(const std::string &batchName)
{		
	const char *zErrMsg =0;
	int id =0;
	sqlite3_stmt *pStmt;     
	std::string selectSql = "SELECT max(ID_TESTSET) FROM TESTSET WHERE BATCHNAME =?";
	int  rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);

	if(rc == SQLITE_OK)
	{	
		sqlite3_bind_text(pStmt, 1, batchName.c_str(), batchName.length(), SQLITE_STATIC);
		sqlite3_step(pStmt);	
		id = sqlite3_column_int(pStmt, 0);	 
	}  
	else
	{
		std::stringstream ss;
		ss <<  "Can not select id from Testset: " << selectSql ;		
		insertLog(ss.str());		
	}
	sqlite3_finalize(pStmt);
	return id;
}

//! insert Mets into database
//! \param batchName as string
//! \param path as string
//! \param fileName as string
void database::insertMets(const std::string &batchName,const std::string &path ,const std::string &fileName)
{	
	char *zErrMsg =0;
	current_id_mets = -1;
	std::string pathDb = path.substr(3,path.length());
	std::string sql = "INSERT INTO METS ('ID_TESTSET','PATH','FILENAME') \
					  VALUES   ('" + stringIdTestSet  + "',  \
					  '" + slash_path(pathDb) + "' ,  \
					  '" + fileName + "')"; 			
	int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); 
	if (rc)
	{
		std::stringstream ss;
		ss <<  "Can not insert new mets into table: " << zErrMsg ;	
		insertLog(ss.str());
	}else{
		current_id_mets = sqlite3_last_insert_rowid(db);
	}
	if ( zErrMsg ) {
		sqlite3_free(zErrMsg);
	}

	
}

void database::dberror(std::string sql){
	std::string error ( sqlite3_errmsg(db) );
	std::stringstream ss;
	ss << "ERROR DB :" << error << std::endl << sql ;
	std::cerr << ss.str() << std::endl;
	insertLog(ss.str());
}

bool database::getInventory(std::string _sysnum, inventory& _inventory){
	bool ret = false;
	static std::string sql = "SELECT UNIQUEBUILDKEY,AUTHOR,PAPERID,"
		              "  LANGUAGES,FORMALDATE,TYPE,TITLE,TITLECOLLECTION,SUBTITLE,CALLNUMBER,ISSUENUMBER,YEARNUMBER,SYSTEMNUMBER,PAGES,"
					  "  CHECKED"
					  "  from INVENTORY where UNIQUEBUILDKEY=?";

	const char *szErrMsg =0;
	sqlite3_stmt *pStmt =0;

	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);

	if( rc == SQLITE_OK){
		sqlite3_bind_text(pStmt, 1, _sysnum.c_str(), _sysnum.length(), SQLITE_STATIC);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col=0;
			
			_inventory.uniquebuildkey = safe_sqlite3_column_text(pStmt, col++);
			_inventory.author = safe_sqlite3_column_text(pStmt, col++);
			_inventory.paperid = safe_sqlite3_column_text(pStmt, col++);
			
			std::string lang = safe_sqlite3_column_text(pStmt, col++);
			std::stringstream ss(lang);
			std::string item;
			while (std::getline(ss, item, ',')) {
				_inventory.languageTerm[item]=item;
			}
			_inventory.formaldate = safe_sqlite3_column_text(pStmt, col++);
			_inventory.type = safe_sqlite3_column_text(pStmt, col++);
			_inventory.title = safe_sqlite3_column_text(pStmt, col++);
			_inventory.titlecollection = safe_sqlite3_column_text(pStmt, col++);
			_inventory.subtitle = safe_sqlite3_column_text(pStmt, col++);
			_inventory.callnumber = safe_sqlite3_column_text(pStmt, col++);
			_inventory.issuenumber = safe_sqlite3_column_text(pStmt, col++);
			_inventory.yearnumber = safe_sqlite3_column_text(pStmt, col++);
			_inventory.systemnumber = safe_sqlite3_column_text(pStmt, col++);
			_inventory.pages = safe_sqlite3_column_text(pStmt, col++);
			
			_inventory.checked = sqlite3_column_int(pStmt, col++);

			//std::cerr << _inventory.toString("DATABASE") << std::endl;
			ret = true;
		}
		
		
	}else{ // problem
		dberror(sql);
	}
	sqlite3_finalize(pStmt);

	return ret;
}

bool database::insertMetsInventory(int id_mets, std::string _sysnum){
	bool ret = false;
	static std::string sql= "INSERT INTO METSINVENTORY (ID_METS,UNIQUEBUILDKEY)"
							" VALUES(?,?)";

	const char *szErrMsg =0;
	sqlite3_stmt *pStmt =0;

	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);

	if( rc == SQLITE_OK){
		sqlite3_bind_int(pStmt,1,id_mets);
		sqlite3_bind_text(pStmt,2,_sysnum.c_str(), _sysnum.length(), SQLITE_STATIC);

		if ( sqlite3_step(pStmt) == SQLITE_DONE ) {
			ret = true;
		}else{
			dberror(std::string("INSERT NOT DONE:")+sql);
		}	
	}else{ 
		dberror(std::string("PREPARE PROBLEM:")+sql);	
	}
	sqlite3_finalize(pStmt);

	return ret;
}

bool database::InventoryChecked(std::string _sysnum){
	bool ret = false;
	static std::string sql = "UPDATE INVENTORY SET CHECKED=1"
		              " where UNIQUEBUILDKEY=?";

	const char *szErrMsg =0;
	sqlite3_stmt *pStmt =0;

	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);

	if( rc == SQLITE_OK){
		sqlite3_bind_text(pStmt, 1, _sysnum.c_str(), _sysnum.length(), SQLITE_STATIC);

		if ( sqlite3_step(pStmt) == SQLITE_DONE ) {
			ret = true;
		}else{
			dberror(std::string("UPDATE NOT DONE:")+sql);
		}	
	}else{ 
		dberror(std::string("PREPARE PROBLEM:")+sql);	
	}
	sqlite3_finalize(pStmt);

	return ret;
}

//update details of Mets
void database::updateMets(int id_mets, datafactory *df)
{	
	const char *szErrMsg =0;
	sqlite3_stmt *pStmt =0;

	std::string issue_number;
	std::string title,date, ausgabe,titleSupplement, papertype;		
		

	Mets *mets = 0;
	mets = df->get<Mets>("METS");		
	title = mets->title;
	date = mets->date;
	issue_number = mets->issueNumber;
	papertype = mets->papertype;

	std::stringstream date_string, year_string;
	int year,day,month;
	// correct conversion of the date
	convert_date(date, "", year, month, day);
	date_string << std::setw(4) << std::setfill('0') << year << "-" 
		<< std::setw(2) << std::setfill('0') << month << "-"
		<< std::setw(2) << std::setfill('0') << day;
	year_string << std::setw(4) << std::setfill('0') << year;

	std::string sql = "UPDATE METS SET ISSUE_NUMBER=?, TITLE=?, PAGES=?, PAPERTYPE=?, ";
	sql = sql + "DATE='" + date_string.str() + "', YEAR='" + year_string.str() + "'  WHERE ID_METS=?";
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);
	if( rc == SQLITE_OK)
	{
		sqlite3_bind_text(pStmt, 1, issue_number.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(pStmt, 2, title.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(pStmt, 3, mets->pages);
		sqlite3_bind_text(pStmt, 4, papertype.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(pStmt, 5, id_mets);
		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			std::stringstream ss;
			ss <<  "Can not update data METS (Database Busy) ";	
			insertLog(ss.str());
		}
	} else {
		std::stringstream ss;
		ss <<  "Can not update data METS: " << szErrMsg ;	
		insertLog(ss.str());
	}
	sqlite3_finalize(pStmt);
	// Insert titles of supplements
	// First delete any existing ones for this ID_METS
	sql = "DELETE FROM SUPPLEMENTS WHERE ID_METS=?";
	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);
	if( rc == SQLITE_OK) {
		sqlite3_bind_int(pStmt, 1, id_mets);
		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			std::stringstream ss;
			ss <<  "Can not update data METS for supplements - delete (Database Busy) ";	
			insertLog(ss.str());
		}
	} else {
		std::stringstream ss;
		ss <<  "Can not update data for supplements for METS - delete: " << szErrMsg ;	
		insertLog(ss.str());
	}
	sqlite3_finalize(pStmt);

	if (mets->supplements.size() > 0) {
		sql = "INSERT INTO SUPPLEMENTS (ID_METS, TITLE_SUPPLEMENT) VALUES(?,?)";
		rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, &szErrMsg);
		if( rc == SQLITE_OK) {
			sqlite3_bind_int(pStmt, 1, id_mets);
			for (std::vector<std::string>::const_iterator it = mets->supplements.begin(); it != mets->supplements.end(); ++it) {
				sqlite3_bind_text(pStmt, 2, it->c_str(), it->length(), SQLITE_STATIC);
				if (sqlite3_step(pStmt) != SQLITE_DONE) {
					std::stringstream ss;
					ss <<  "Can not update data METS for supplements - insert (Database Busy) ";	
					insertLog(ss.str());
				}
				sqlite3_reset(pStmt);
			}
		} else {
			std::stringstream ss;
			ss <<  "Can not update data for supplements for METS - insert: " << szErrMsg ;	
			insertLog(ss.str());
		}
		sqlite3_finalize(pStmt);
	}
}


//! insert Linked files of mets (pdf,alto,img)
void database::insertLinkedFiles(int id_mets,datafactory *df)
{
	const char *zErrMsg = 0;
	sqlite3_stmt *pStmt = 0;

	datafactory_set<File_Group> dftypefile = df->get_set<File_Group>();
	datafactory_set<AmdSecMix> dfamdsec = df->get_set<AmdSecMix>();

	const char *sql = "INSERT INTO LINKEDFILES ('ID_METS', 'TYPE', 'GROUPID', 'CHECKSUM','SIZE','FILENAME','FILEID','DPI') VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
	if (sqlite3_prepare_v2(db, sql, -1, &pStmt, &zErrMsg) == SQLITE_OK) {
		for (datafactory_set<File_Group>::iterator it = dftypefile.begin(); it != dftypefile.end(); ++it)
		{		
			for (size_t i=0;i < it->vect.size();i++)
			{
				const Type_File &tf = it->vect[i];
				int dpi =0;
				if (it.key() == "IMGGRP")
				{
					AmdSecMix *s =	df->get<AmdSecMix>(tf.admid); // get the dpi of image
					if ( s!= NULL) {
						dpi = s->dpi;	
					}else{
						dpi=0;
						std::stringstream ss;
						ss <<  "database::insertLinkedFiles -> Error with Non admid:" << tf.admid ;               
						insertLog(ss.str());   
					}
				}
				else
				{			
					dpi =0;
				}
				sqlite3_bind_int(pStmt, 1, id_mets);
				sqlite3_bind_text(pStmt, 2, it.key().c_str(), it.key().length(), SQLITE_STATIC);
				sqlite3_bind_text(pStmt, 3, tf.groupId.c_str(), tf.groupId.length(), SQLITE_STATIC);
				sqlite3_bind_text(pStmt, 4, tf.checksum.c_str(), tf.checksum.length(), SQLITE_STATIC);
				sqlite3_bind_int(pStmt, 5, tf.size);
				sqlite3_bind_text(pStmt, 6, tf.ref.c_str(), tf.ref.length(), SQLITE_STATIC);
				sqlite3_bind_text(pStmt, 7, tf.id.c_str(), tf.id.length(), SQLITE_STATIC);
				sqlite3_bind_int(pStmt, 8, dpi);
				if (sqlite3_step(pStmt) != SQLITE_DONE) {
					insertLog("ERROR: Cannot insert data into LINKEDFILES");
				}
				sqlite3_clear_bindings(pStmt);
				sqlite3_reset(pStmt);
			}
		}
	} else {
		insertLog("ERROR: insertLinkedFiles - cannot prepare statement");
	}
}
//! insert Articles into database
void database::insertArticle(int id_mets, datafactory_set<Article> dfarticle,int number)
{
	const char *zErrMsg = 0;
	sqlite3_stmt *pStmt = 0;

	const char *sql = "INSERT INTO ARTICLE ('ID_METS','ID_ARTICLE','DIV','MUSTCHECK','COUNTCARACTER','TITLE', 'NUMBER_BLOCKS') VALUES (?,?,?,?,?,?,?)";
	if (sqlite3_prepare_v2(db, sql, -1, &pStmt, &zErrMsg) == SQLITE_OK) {
		for (datafactory_set<Article>::iterator it = dfarticle.begin(); it != dfarticle.end(); ++it)
		{	
			if (it->check == 1){
				int rr = rand();
				double al = ( 1000.0 * rr ) / ( 1.0 + RAND_MAX );
				if ( al > number ) continue ; // skip the insert

				int col = 1;

				sqlite3_bind_int(pStmt, col++, id_mets);
				sqlite3_bind_text(pStmt, col++, it->id.c_str(), it->id.length(), SQLITE_STATIC);
				sqlite3_bind_text(pStmt, col++, it->div.c_str(), it->div.length(), SQLITE_STATIC);
				sqlite3_bind_int(pStmt, col++, it->check);
				sqlite3_bind_int(pStmt, col++, it->title.length());
				sqlite3_bind_text(pStmt, col++, it->title.c_str(), it->title.length(), SQLITE_STATIC);
				sqlite3_bind_int(pStmt, col++, it->vectArticle.size());
				if (sqlite3_step(pStmt) != SQLITE_DONE) {
					insertLog("ERROR: Can not insert data into table Article");
				}
				sqlite3_clear_bindings(pStmt);
				sqlite3_reset(pStmt);
			}
		}
	} else {
		insertLog("ERROR: insertArticle - cannot prepare statement");
	}
}


//! insert message into log
void database::insertLog(std::string message)
{
	errorHandler h;
	h.setlogFilePath(logFilePath);	
	h.begin(message);	

#ifdef DEBUGPARAMETERS
	std::cout << message << std::endl;
#endif // DEBUGPARAMETERS		

}
//! insert Dateerror 
void database::insertDateError(int category,std::string dateBegin,std::string dateEnd,std::string issues,std::string comment){	

	sqlite3_stmt *pStmt;
	const char *zErrMsg= 0; 

	std::string sql = "INSERT INTO DATEERROR ('ID_TESTSET','DATE_BEGIN', 'DATE_END', 'ISSUES', 'COMMENT','ID_ERRORTYPE','HASHKEY') "
					  "VALUES   (?,  ?,  ?,  ?,  ?,  ?,  ?)";	

	int rc = sqlite3_prepare_v2(db,sql.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK){

		int pos = 1;

		sqlite3_bind_int(pStmt,pos++,idTestset);
		sqlite3_bind_text(pStmt,pos++,dateBegin.c_str(),dateBegin.length(),SQLITE_STATIC);
		sqlite3_bind_text(pStmt,pos++,dateEnd.c_str(),dateEnd.length(),SQLITE_STATIC);
		sqlite3_bind_text(pStmt,pos++,issues.c_str(),issues.length(),SQLITE_STATIC);
		sqlite3_bind_text(pStmt,pos++,comment.c_str(),comment.length(),SQLITE_STATIC);
		sqlite3_bind_int(pStmt,pos++,category);
		
		std::stringstream ss;
		ss << dateBegin << dateEnd << issues << category << comment ;
		std::string md5 = g_md5.getHashFromString(ss.str());

		sqlite3_bind_text(pStmt,pos++,md5.c_str(),md5.length(),SQLITE_STATIC);

		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			dberror(sql);
		};
	
	}else{
		std::stringstream ss;
		ss <<  "Can not insert data into DATEERROR: " << &zErrMsg ;	
		insertLog(ss.str());		
	} 

}
bool database::insertALLData(datafactory *df,metsparserContext& ctx,int number)
{	
	char *sErrMsg=0;
	// Use a transaction to speed things up
	startTransaction();
		updateMets(current_id_mets,df);
		if ( ctx.inventory.isActif() ){ 
			//TODO Call unique function
			insertMetsInventory(current_id_mets,ctx.inventory.inventoryMODSMD_COLLECTION.uniquebuildkey); 
		};
		insertLinkedFiles(current_id_mets,df);
		datafactory_set<Article> dfarticle = df->get_set<Article>();
		insertArticle(current_id_mets,dfarticle,number);	
	endTransaction();
	return true;
}

void database::startTransaction(){
	char *sErrMsg=0;
	
	int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
	
	if ( rc != SQLITE_OK ){
		std::string error (sErrMsg);
		std::cerr << "ERROR BEGIN TRANSATCTION" << error << std::endl;
		sqlite3_free(sErrMsg);
	}
}

void database::endTransaction(){
	char *sErrMsg=0;
	
	int rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);

	if ( rc != SQLITE_OK ){
		std::string error (sErrMsg);
		std::cerr << "ERROR END TRANSATCTION" << error << std::endl;
		sqlite3_free(sErrMsg);
	}

}

//! get all years of the current Mets
std::vector<int> database::getAllYears()
{	
	int year;
	std::pair<int,int> pair;
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 

	std::string selectSql = "select distinct(year) from Mets where id_testset ='" + stringIdTestSet + "'";  
	rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	std::vector<int> v;
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int count = sqlite3_data_count(pStmt);		
			for (int i =0;i<  count ;i++)
		 {
			 const char *result = (const char *)sqlite3_column_text(pStmt, i);			
			 if (i==0) year = atoi(result);
			}			
			v.push_back(year);
		}
	}
	sqlite3_finalize(pStmt);
	return v;
}

//! get Mets of the selected years
std::vector<std::pair<int,Mets>> database::vMetsYear(int year)
{	
	std::stringstream oIdTestset,oYear;
	oYear << year;
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	Mets mets;
	std::vector<std::pair<int,Mets>>  vMets;
	std::string selectSql = "SELECT ID_METS,ID_TESTSET,PATH, FILENAME,\
							ISSUE_NUMBER,TITLE,DATE,YEAR FROM METS where ID_TESTSET ='" + stringIdTestSet + "' and YEAR ='" + oYear.str() + "'";
	rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			std::pair<int,Mets> p;
			int resInt = sqlite3_column_int(pStmt, 0);
			mets.idMets = resInt;
			p.first = resInt;
			
			mets.idTestSet = sqlite3_column_int(pStmt, 1);
			mets.path = (const char *) sqlite3_column_text(pStmt, 2);
			mets.fileName = (const char *) sqlite3_column_text(pStmt, 3);
			mets.issueNumber = (const char *) sqlite3_column_text(pStmt, 4);
			mets.vectIssueNumber = getIssueNumber(mets.issueNumber);
			mets.title = (const char *) sqlite3_column_text(pStmt, 5);
			mets.date = (const char *) sqlite3_column_text(pStmt, 6);
			mets.year = atoi((const char *) sqlite3_column_text(pStmt,7));
			FillSupplements(mets.idMets, mets.supplements);
			p.second = mets;
			vMets.push_back(p);
		}
	}
	sqlite3_finalize(pStmt);
	return vMets;
}

//� insert mets for the sample test of the structure
void database::insertRandomMets(int number,int totalMets)
{
	float percentage = float(totalMets) /float (1000) * number;
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0; 
	int random= int(percentage);
	if (random ==0)
		random =1;

	std::stringstream ss;
	ss << random;
	std::string selectSql = "SELECT ID_METS FROM METS WHERE \
							ID_TESTSET ='"+ stringIdTestSet + "' ORDER BY RANDOM () LIMIT " + ss.str() ;

	rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	if(rc == SQLITE_OK)
	{   
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{		
			const char *result = (const char *)sqlite3_column_text(pStmt, 0);			
			int id  = atoi(result);				
			std::stringstream oId;
			oId << id;
			char *zErrMsg2=0;
			std::string sql2 = "INSERT INTO SAMPLING_STRUCTURE ('ID_mets') \
							   VALUES  ('" + oId.str()  + "')"; 							

			int rc = sqlite3_exec(db, sql2.c_str(), NULL, 0, &zErrMsg2);
			if( rc )
			{
				std::stringstream ss;
				ss <<  "Can not insert data SAMPLING_STRUCTURE: " << zErrMsg ;	
				insertLog(ss.str());				
			}
			if ( zErrMsg2 ) { sqlite3_free(zErrMsg2);};
		}
	}
	sqlite3_finalize(pStmt);
}


ErrorTypeMets database::getErrorTypeWithId(int id)
{
	sqlite3_stmt *pStmt=0;
	std::stringstream ss;
	ss<<id;	
	const char *zErrMsg= 0; 
	std::string selectSql = "SELECT ID,ID_TYPE,ERROR,DETAILS FROM ERRORTYPE where id_type ='" + ss.str()+ "'"; 
	std::vector<ErrorTypeMets> v;
	int rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	ErrorTypeMets et;  
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{

			int count = sqlite3_data_count(pStmt);		
			for (int i =0;i<  count ;i++)
			{
				const char *result = (const char *)sqlite3_column_text(pStmt, i);			
				if (i==0) et.id = atoi(result);		
				else if (i==1) et.id_type = atoi(result);					
				else if (i==2) et.error = result;
				else if (i==3) et.details = result;		
			}
		}		
	}
	sqlite3_finalize(pStmt);
	return et;
}


void database::deletePARAMTESTSET(){

	static std::string sql = "DELETE FROM PARAMTESTSET WHERE ID_TESTSET = ? ";
	const char *zErrMsg=0;
	sqlite3_stmt *pStmt;

	int rc = sqlite3_prepare_v2(db,sql.c_str(),-1, &pStmt,&zErrMsg);

	if( rc != SQLITE_OK  )
	{		
		
		dberror(sql);
		return;	
	};

	int col = 1;
	sqlite3_bind_int(pStmt,col++,idTestset);

	if (sqlite3_step(pStmt) != SQLITE_DONE) {
			dberror(sql);
	};

	sqlite3_finalize(pStmt);

}

//! insert parameters values into database
void database::insertParameterVerifiers(Parameters *param)
{
	
	deletePARAMTESTSET();
	
	const char *zErrMsg=0;
	sqlite3_stmt *pStmt;


	static std::string sql = "INSERT INTO PARAMTESTSET (ID_TESTSET,KEY,VALUE) VALUES (?,?,?)";

	int rc = sqlite3_prepare_v2(db,sql.c_str(),-1, &pStmt,&zErrMsg);

	if( rc != SQLITE_OK  )
	{		
		
		dberror(sql);
		sqlite3_finalize(pStmt);
		return;	
	};

	std::map<std::string,std::string> map = param->mapFilter();

	for ( std::map<std::string,std::string>::iterator it = map.begin();it != map.end() ; it ++){
		sqlite3_clear_bindings(pStmt);
		sqlite3_reset(pStmt);
		
		int col = 1;
		sqlite3_bind_int(pStmt,col++,idTestset);
		sqlite3_bind_text(pStmt,col++,it->first.c_str(),it->first.length(),SQLITE_STATIC);
		sqlite3_bind_int(pStmt,col++,atoi(it->second.c_str()));

		if (sqlite3_step(pStmt) != SQLITE_DONE) {
			dberror(sql);
		};

	}


	sqlite3_finalize(pStmt);
	
}


void database::insertMetsErrorWithId(int category,const std::string &relatedType,const std::string &filePart,const Error &e,std::string id)
{	
	
	const char *zErrMsg =0;
	sqlite3_stmt *pStmt;

	std::string sql = "INSERT INTO MetsError ('ID_RELATED','RELATED_TYPE','ID_TESTSET', 'FILE_PART',\
					  'ERRORLINE', 'ERRORCOLUMN','ID_ERRORTYPE', 'MESSAGE','ID_SEARCH','HASHKEY') \
					  VALUES   (?,  ?,  ?,  ?,  ?,  ?,  ?,  ?,  ?, ?)";		
					  
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt,&zErrMsg);
	if( rc == SQLITE_OK ){
		sqlite3_bind_int(pStmt,1,current_id_mets);
		sqlite3_bind_text(pStmt,2,relatedType.c_str(),relatedType.length(),SQLITE_STATIC);
		sqlite3_bind_int(pStmt,3,idTestset);
		sqlite3_bind_text(pStmt,4,filePart.c_str(),filePart.length(),SQLITE_STATIC);
		sqlite3_bind_int(pStmt,5,e.errorline);
		sqlite3_bind_int(pStmt,6,e.errorcolumn);
		sqlite3_bind_int(pStmt,7,category);
		sqlite3_bind_text(pStmt,8,e.message.c_str(),e.message.length(),SQLITE_STATIC);
		sqlite3_bind_text(pStmt,9,id.c_str(),id.length()>49?49:id.length(),SQLITE_STATIC);

		std::stringstream ss;
		ss << relatedType << getMetsFilename(current_id_mets) << filePart << category << e.errorline << e.message << id;
		std::string md5 = g_md5.getHashFromString(ss.str());

		sqlite3_bind_text(pStmt,10,md5.c_str(),md5.length(),SQLITE_STATIC);
		
		if ( sqlite3_step(pStmt) != SQLITE_DONE ) {
			dberror(std::string("INSERT NOT DONE:")+sql);
		};
		
	}else{
		dberror(std::string("PREPARE PROBLEM:")+sql);		
	}

	sqlite3_finalize(pStmt);
	
}

bool database::FillSupplements(int idMets, std::vector<string> &supplements)
{
	sqlite3_stmt *pStmt;
	int rc;	
	const char *zErrMsg= 0;

	supplements.clear();

	std::stringstream ostr;
	ostr << "SELECT TITLE_SUPPLEMENT FROM SUPPLEMENTS WHERE ID_METS='" << idMets << "'";

	rc = sqlite3_prepare_v2(db, ostr.str().c_str(), -1, &pStmt,&zErrMsg);	
	if(rc == SQLITE_OK)
	{	  
		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			supplements.push_back((const char *) sqlite3_column_text(pStmt, 0));
		}
		sqlite3_finalize(pStmt);
		return true;
	} else {
		sqlite3_finalize(pStmt);
		return false;
	}
}

bool database::isEntityToCount(std::string type,std::string entity){

	static std::map < string , std::map < string , int > > cache;

	if ( cache.size() == 0 ) _loadEntity ( cache );


	std::map < string , int >::iterator it = cache[type].find(entity);

	if ( it != cache[type].end() ) return true;

	return false;
}

static std::map < std::string , std::map < std::string , std::map < std::string ,int > > > cacheEntityToTitleCorrection;

bool database::isEntityToTitleCorrection(std::string type,std::string entity){

	

	if ( cacheEntityToTitleCorrection.size() == 0 ) _loadEntityTitleCorrection ( cacheEntityToTitleCorrection );


	std::map < std::string , std::map < std::string ,int > >::iterator it = cacheEntityToTitleCorrection[type].find(entity);

	if ( it != cacheEntityToTitleCorrection[type].end() ) return true;

	return false;

}

bool database::isEntityToTitleCorrectionLink(std::string type,std::string entity,std::string entityLink){

	

	if ( cacheEntityToTitleCorrection.size() == 0 ) _loadEntityTitleCorrection ( cacheEntityToTitleCorrection );


	std::map < std::string , std::map < std::string ,int > >::iterator it = cacheEntityToTitleCorrection[type].find(entity);

	if ( it == cacheEntityToTitleCorrection[type].end() ) return false;

	std::map < std::string ,int >::iterator itt = it->second.find(entityLink);

	if (itt != it->second.end() ) return true;

	return false;

}

void  database::_loadEntity(std::map < std::string , std::map < std::string ,int > > & toFill){
	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT ENTITYNAME, DOCTYPE FROM ENTITYCONFIGURATION WHERE USEDMETSVER=1";
	
	int rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;

			std::string entity = safe_sqlite3_column_text(pStmt, col++);
			std::string doctype = safe_sqlite3_column_text(pStmt, col++);
			

			toFill [doctype] [entity] = 1;
		}
		
	}else{
		dberror(std::string("PREPARE PROBLEM:")+selectSql);		
	}
	sqlite3_finalize(pStmt);
}

void  database::_loadEntityTitleCorrection(std::map < std::string , std::map < std::string , std::map < std::string ,int > > > & toFill){
	
	sqlite3_stmt *pStmt;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT a.ENTITYNAME, a.DOCTYPE, b.ENTITYNAMELINK FROM ENTITYCONFIGURATION a LEFT JOIN ENTITYCONFIGURATIONLINK b on a.ID_ENTITY = b.ID_ENTITY"
		                    " WHERE USEFORTITLECORRECTION=1";
	
	int rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
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
		dberror(std::string("PREPARE PROBLEM:")+selectSql);		
	}
	sqlite3_finalize(pStmt);
}

std::string database::getMetsFilename(int id_mets ){

	sqlite3_stmt *pStmt;

	std::string ret;

	const char *zErrMsg= 0; 
	
	std::string selectSql = "SELECT FILENAME FROM METS WHERE id_mets=?";
	
	int rc = sqlite3_prepare_v2(db,selectSql.c_str(),-1, &pStmt,&zErrMsg);
	
	
	if(rc == SQLITE_OK)
	{	 
		sqlite3_bind_int(pStmt,1,id_mets);

		while(sqlite3_step(pStmt) == SQLITE_ROW)
		{
			int col = 0;

			ret = safe_sqlite3_column_text(pStmt, col++);
		}
		
	}else{
		dberror(std::string("PREPARE PROBLEM:")+selectSql);		
	}
	sqlite3_finalize(pStmt);

	return ret;

}


void database::insertDivCount(std::map<std::string,int> & tabl ){

	const char *zErrMsg =0;
	sqlite3_stmt *pStmt;
	startTransaction();

	std::string sql = "INSERT INTO METSDIVCOUNT (ID_METS,ENTITY,TOT)" 
		              " VALUES   (?,  ?,  ?)";		
					  
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt,&zErrMsg);
	if( rc == SQLITE_OK ){
		for ( std::map<std::string,int>::iterator it = tabl.begin(); it != tabl.end() ; it++ ){
			sqlite3_bind_int(pStmt,1,current_id_mets);
			sqlite3_bind_text(pStmt,2,it->first.c_str(),it->first.length(),SQLITE_STATIC);
			sqlite3_bind_int(pStmt,3,it->second);

			if ( sqlite3_step(pStmt) != SQLITE_DONE ) {
				dberror(std::string("INSERT NOT DONE:")+sql);
				break;
			};

			sqlite3_reset(pStmt);
		};
	}else{
		dberror(std::string("PREPARE PROBLEM:")+sql);		
	};

	sqlite3_finalize(pStmt);

	endTransaction();

}

