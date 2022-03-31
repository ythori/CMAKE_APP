/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Constants

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


================================ */


#include "pch.h"
#include "ConstantParams.h"
#include "DataProc.h"

CParams::CParams() {
	std::vector<std::string> ad = readTxt("_address.txt");
	for (auto t : ad) {
		if (t.size() > 0) {
			if (t.substr(0, 1) == "1") db_folder_path = t.substr(2);
			else if (t.substr(0, 1) == "2") music_db_folder_path = t.substr(2);
			else if (t.substr(0, 1) == "3") crestmuse_db_name = t.substr(2);
			else if (t.substr(0, 1) == "4") crestmuse_music_name = t.substr(2);
			else if (t.substr(0, 1) == "5") crestmuse_xml_name = t.substr(2);
		}
	}
	echoes(db_folder_path, music_db_folder_path, crestmuse_db_name, crestmuse_music_name, crestmuse_xml_name);
}