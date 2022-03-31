/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Constants

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


================================ */

#ifndef CONSTANT_PARAMS_H
#define CONSTANT_PARAMS_H

#include "pch.h"

class CParams {
public:
	/* ================================
		Music XML data address:
			ex) "C:/Users/brume/Documents/Project/DB/Research/MusicDB/crestmuse/PEDBv2.2/bac-inv001-schif-g/bac-inv001-wiener-p012-013.xml"

			-> Set as follows
				db_folder_path:			"C:/Users/brume/Documents/Project/DB/Research/"
				music_db_folder_path:	"MusicDB"
				crestmuse_db_name:		"PEDBv2.2";
				crestmuse_music_name:	"bac-inv001-schif-g";
				crestmuse_xml_name:		"bac-inv001-wiener-p012-013.xml";

	================================ */


	CParams();


	// Functions
	std::string getDBFolderPath() const { return db_folder_path; }
	std::string getMusicDBFolderPath() const { return music_db_folder_path; }
	std::vector<std::string> getCrestmusePath() const {
		std::vector<std::string> ret;
		ret.emplace_back(crestmuse_db_name);
		ret.emplace_back(crestmuse_music_name);
		ret.emplace_back(crestmuse_xml_name);
		return ret;
	};

	std::string db_folder_path, music_db_folder_path, crestmuse_db_name, crestmuse_music_name, crestmuse_xml_name;


};

#endif //CONSTANT_PARAMS_H
