#include "pch.h"
#include "Constants.h"

DataAddress::DataAddress() :
	DBFolder("C:/Users/brume/Documents/Project/DB/Research/"),
	ResultFolder(DBFolder + "Results/"),
	//TestSampleDB(DBFolder + "TestSampleDB/"),
	MusicDB(DBFolder + "MusicDB/"),
	MiscFolder(DBFolder + "Misc/")
{
	savefolder = ResultFolder;
}

bool DataAddress::setSaveFolder(const std::string sfname) {
	savefolder += sfname;
	if (_mkdir(savefolder.c_str()) == 0) {
		std::cout << "Create save folder: " << savefolder << std::endl;
		savefolder += "/";
		return true;
	}
	else {
		savefolder += "/";
		return false;
	}
}

std::string DataAddress::getStandardDBFilePath(const std::string DB, const std::string fname) const {
	std::string attDB;
	//if (DB == "testsample") attDB = TestSampleDB;
	if (DB == "music") attDB = MusicDB;
	else if (DB == "result") attDB = ResultFolder;
	else std::cout << "DB specification error" << std::endl;
	return attDB + fname;
}

std::string DataAddress::getSavePath(const std::string fname) const {
	return savefolder + fname;
}

