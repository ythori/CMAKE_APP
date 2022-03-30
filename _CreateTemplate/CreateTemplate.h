
/* ================================
Created by Hori on 2021//

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Create template

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Input: Folder_name and class_name
			ex) (DTW, DynamicTimeWarping)

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef CREATE_TEMPLATE_C_H
#define CREATE_TEMPLATE_C_H

#include "pch.h"

class CreateTemplate {
public:
	CreateTemplate();
	CreateTemplate(std::string folder_n, std::string file_n) : folder_name(folder_n), class_name(file_n) {
		top_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/";
		template_path = top_path + "_Template/";
		folder_path = top_path + folder_n + "/";
		tab = "    ";	// 4 space
	}

	void execute();


private:
	std::string convertName(const std::string att);
	void createClassFile();
	void createTestFile();
	void createPch();
	void createCMakeLists();

	bool is;
	std::string top_path, template_path, folder_path, tab, folder_name, class_name;


};



#endif //CREATE_TEMPLATE_C_H