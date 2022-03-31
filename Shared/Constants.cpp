#include "pch.h"
#include "Constants.h"
#include "ConstantParams.h"

//DataAddress::DataAddress() :
//	DBFolder("C:/Users/brume/Documents/Project/DB/Research/"),
//	ResultFolder(DBFolder + "Results/"),
//	//TestSampleDB(DBFolder + "TestSampleDB/"),
//	MusicDB(DBFolder + "MusicDB/"),
//	MiscFolder(DBFolder + "Misc/")
//{
//	savefolder = ResultFolder;
//}

DataAddress::DataAddress(){
	auto CP = std::make_shared<CParams>();
	DBFolder = CP->getDBFolderPath();
	ResultFolder = DBFolder + "Results/";
	MusicDB = DBFolder + "MusicDB/";
	MiscFolder = DBFolder + "Misc/";
	savefolder = ResultFolder;
}

std::string db_Folder_path = "C:/Users/brume/Documents/Project/DB/Research/";
std::string music_db_folder_path = "MusicDB";
std::string crestmuse_db_name = "PEDBv2.2";
std::string crestmuse_music_name = "bac-inv001-schif-g";
std::string crestmuse_xml_name = "bac-inv001-wiener-p012-013.xml";


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

