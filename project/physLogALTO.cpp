#include "physLogALTO.h"
#include <iostream>

PhysicalLogicalAltoFilter::PhysicalLogicalAltoFilter():donothing(false){
}

void PhysicalLogicalAltoFilter::appendPhysical( std::string key ){
	mapKey[key]=false;
	listKey.push_back(key);
}

void PhysicalLogicalAltoFilter::checkLogical( std::string key , std::string type ){
	if ( isCheckedType ( type )) {
		mapKey[key]=true;
	}else{
		mapKey[key]=false;
	}
}

void PhysicalLogicalAltoFilter::build(){
	
	//std::cout << "BUILD BUILD:" <<std::endl;
	
	while ( listKey.size() > 0 ){

		std::string key = listKey.back() ;
		//std::cout << "BUILD KEY:" << key <<std::endl;
		if ( mapKey[key] ){

			bool flag = false;
			for( std::list<std::string>::iterator it = listKey.begin(); it != listKey.end();++it){
				//std::cout << "BUILD KEY:" << key << " SECOND: "<< *it <<std::endl;
				if ( ! key.compare(*it)){ break; }
				if ( mapKey[*it] ) {
					flag = true;
				}
				mapKey[*it]=flag;
			}
			break;
		}
		listKey.pop_back();
	}
}

bool PhysicalLogicalAltoFilter::isAltoToCheck(std::string key){
	if ( donothing ) return true;

	std::map<std::string,bool>::iterator it = mapKey.find(key);
	if ( it == mapKey.end () ) return false;
	return it->second;
}

void PhysicalLogicalAltoFilter::setDoNothing(bool flag){
	donothing = flag; 
}

bool PhysicalLogicalAltoFilter::isCheckedType(std::string type){
	if ( type.find(".MAIN.") != std::string::npos ) {
		return true;
	}
	return false;
}