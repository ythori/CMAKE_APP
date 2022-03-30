/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Constants

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

For use:
	Usage
		1. DataAddress DA
		2. DA.resetSaveFolder()		// set result folder (just in case)
		3.1. Set save folder
			: DA.setSaveFolder(savefolder)		// savefolder = Results/savefolder/
		3.2. Get data from DB folder
			: 1. db_folder = DA. get____DB() + "db_folder_name/"
			  2. data_path = db_folder + "file_name.__"

	1. DataAddress
		Defaultly set:
			DBFolder:		"C:/Users/brume/Documents/Project/DB/Research/"
			ResultFolder:	DBFolder + "Results/"
			TestSampleDB:	DBFolder + "TestSampleDB/"
			MusicDB:		DBFolder + "MusicDB/"
			MiscFolder:		DBFolder + "Misc/"

	Optional:
		bool setSaveFolder(std::string sfname):
			Make save folder at (savefolder + sfname)
			savefolder is updated as "savefolder + sfname"
		void resetSaveFolder(std::string sfname):
			sfname: "testsample": savefolder = CommonDB
				  : "music": savefolder = MusicDB
				  : else: savefolder = ResultFolder
		string getFilePath(std::string DB, std::string fname):
			DB: "testsample" or "music" or "result"
			return DB + fname
		string getSavePath(std::string fname):
			return savefolder + fname
		string concatFileForWindows(std::string bef, std::string aft)
			return bef + aft

================================ */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "pch.h"


class DataAddress {
public:
	DataAddress();

	std::string getDBFolder() const { return DBFolder; }

	//std::string getTestSampleDB() const { return TestSampleDB; }
	std::string getMusicDB() const { return MusicDB; }
	std::string getResultFolder() const { return ResultFolder; }
	std::string getMiscFolder() const { return MiscFolder; }

	void resetSaveFolder() { savefolder = ResultFolder; }
	void resetSaveFolder(const std::string sfname) { savefolder = DBFolder + sfname; }
	bool setSaveFolder(const std::string sfname);	// Create save folder if not existing (where savefolder + sfname)
	std::string getStandardDBFilePath(const std::string DB, const std::string fname) const;

	std::string getSaveFolderPath() const { return savefolder; }
	std::string getSavePath(const std::string fname) const;	// return savefolder + fname


private:
	const std::string DBFolder, ResultFolder, TestSampleDB, MusicDB, MiscFolder;
	std::string savefolder;

};

class Timer {
public:
	Timer() { restart(); elapsed_msec = 0; }
	void restart() {
		start = std::chrono::system_clock::now();
	}
	void elapsed() {
		auto end = std::chrono::system_clock::now();
		auto dur = end - start;
		elapsed_msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
	}
	void show() { std::cout << elapsed_msec << "[msec]" << std::endl; }

private:
	std::chrono::system_clock::time_point start;    //  vJn
	double elapsed_msec;
};

#endif //CONSTANTS_H
