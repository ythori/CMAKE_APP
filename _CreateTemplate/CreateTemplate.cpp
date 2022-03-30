

#include "CreateTemplate.h"
#include "pch.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"

CreateTemplate::CreateTemplate() {
	is = false;
}


void CreateTemplate::execute() {
	// Folder check
	bool is_tf = createDirectory(top_path + folder_name);
	//echo(is_tf);

	createClassFile();
	createTestFile();
	createPch();
	createCMakeLists();
}

std::string CreateTemplate::convertName(const std::string att) {
	std::string ret;
	if (att == "test") {
		ret += "_";
		ret += std::tolower(class_name[0]);
		for (int i = 1; i < class_name.size(); ++i) {
			ret += class_name[i];
		}
		ret += "Test";
	}
	else ret = class_name;

	return ret;
}

void CreateTemplate::createClassFile() {
	std::string header_file_name = class_name + ".h";
	std::string cpp_file_name = class_name + ".cpp";
	std::string header_txt = "";
	std::string cpp_txt = "";

	std::string template_header_path = template_path + "_cls.h";
	std::string template_cpp_path = template_path + "_cls.cpp";

	auto template_header_vec = readFileStr(template_header_path);
	auto template_cpp_vec = readFileStr(template_cpp_path);

	// header
	for (auto line : template_header_vec) {
		std::string temp = "";
		//echoes("line", line, line.length());

		if (line.length() == 0) temp = "";

		else if (line.find("program") != std::string::npos) {
			int i = 0;
			temp = tab;
			for (auto s : class_name) {
				//echo(s);
				if (i == 0) temp += s;
				else {
					if (std::isupper(s)) {
						temp += " ";
						temp += s;
					}
					else temp += s;
				}
				++i;
			}
			//echoes("program", line, temp);
		}
		else if (line.find("_C_H") != std::string::npos) {
			std::string cap_str;
			int i = 0;
			for (auto s : class_name) {
				if (i == 0) cap_str += s;
				else {
					if (std::isupper(s)) {
						cap_str += "_";
						cap_str += s;
					}
					else cap_str += std::toupper(s);
				}
				++i;
			}
			cap_str += "_H";
			temp = std::regex_replace(line, std::regex("_C_H"), cap_str);
			//echoes("_H", line, temp);
		}
		else if (line.find("CLS") != std::string::npos) {
			temp = std::regex_replace(line, std::regex("CLS"), class_name);
			//echoes("CLS", temp);
		}
		else {
			temp = line;
			//echoes("none", line, temp);
		}
		header_txt += (temp + "\n");
	}

	// cpp
	for (auto line : template_cpp_vec) {
		if (line.find("_cls") != std::string::npos) {
			cpp_txt += std::regex_replace(line, std::regex("_cls"), class_name);
		}
		else if (line.find("CLS") != std::string::npos) {
			cpp_txt += std::regex_replace(line, std::regex("CLS"), class_name);
		}
		else cpp_txt += line;
		cpp_txt += "\n";
	}

	createFileUsingStr(folder_path, header_file_name, header_txt);
	createFileUsingStr(folder_path, cpp_file_name, cpp_txt);
}

void CreateTemplate::createTestFile() {
	std::string test_file_name = "";
	int i = 0;
	for (auto s : class_name) {
		if (i == 0) test_file_name += std::tolower(s);
		else test_file_name += s;
		++i;
	}
	//echo(test_file_name);

	std::string header_file_name = "_" + test_file_name + "Test.h";
	std::string cpp_file_name = "_" + test_file_name + "Test.cpp";
	std::string header_txt = "";
	std::string cpp_txt = "";

	std::string template_header_path = template_path + "_Test.h";
	std::string template_cpp_path = template_path + "_Test.cpp";

	auto template_header_vec = readFileStr(template_header_path);
	auto template_cpp_vec = readFileStr(template_cpp_path);

	// header
	for (auto line : template_header_vec) {
		std::string temp = "";
		//echoes("line", line, line.length());

		if (line.length() == 0) temp = "";

		else if (line.find("_TEST_H") != std::string::npos) {
			std::string cap_str;
			int i = 0;
			for (auto s : class_name) {
				if (i == 0) cap_str += s;
				else {
					if (std::isupper(s)) {
						cap_str += "_";
						cap_str += s;
					}
					else cap_str += std::toupper(s);
				}
				++i;
			}
			cap_str += "_TEST_H";
			temp = std::regex_replace(line, std::regex("_TEST_H"), cap_str);
		}
		else if (line.find("_Test()") != std::string::npos) {
			temp = std::regex_replace(line, std::regex("_Test"), test_file_name + "Test");	// regex: () is not usable
		}
		else {
			temp = line;
		}
		header_txt += (temp + "\n");
	}

	// cpp
	for (auto line : template_cpp_vec) {
		if (line.find("_Test.h") != std::string::npos) {
			cpp_txt += std::regex_replace(line, std::regex("_Test.h"), "_" + test_file_name + "Test.h");
		}
		else if (line.find("_cls") != std::string::npos) {
			cpp_txt += std::regex_replace(line, std::regex("_cls"), class_name);
		}
		else if (line.find("_Test()") != std::string::npos) {
			cpp_txt += std::regex_replace(line, std::regex("_Test"), test_file_name + "Test");
		}
		else cpp_txt += line;
		cpp_txt += "\n";
	}

	createFileUsingStr(folder_path, header_file_name, header_txt);
	createFileUsingStr(folder_path, cpp_file_name, cpp_txt);

}

void CreateTemplate::createPch() {
	std::string header_file_name = "pch.h";
	std::string cpp_file_name = "pch.cpp";
	std::string header_txt = "";
	std::string cpp_txt = "";

	std::vector<std::string> include_list;
	include_list.emplace_back("windows.h");
	include_list.emplace_back("iostream");
	include_list.emplace_back("vector");
	include_list.emplace_back("string");
	include_list.emplace_back("unordered_map");
	include_list.emplace_back("tuple");
	include_list.emplace_back("fstream");
	include_list.emplace_back("cmath");
	include_list.emplace_back("cmath");
	include_list.emplace_back("Eigen/Core");
	include_list.emplace_back("Eigen/Geometry");


	std::string cap_str;
	int i = 0;
	for (auto s : class_name) {
		if (i == 0) cap_str += s;
		else {
			if (std::isupper(s)) {
				cap_str += "_";
				cap_str += s;
			}
			else cap_str += std::toupper(s);
		}
		++i;
	}

	header_txt += ("#ifndef PCH_" + cap_str + "_H\n");
	header_txt += ("#define PCH_" + cap_str + "_H\n\n");
	header_txt += "#define WIN32_LEAN_AND_MEAN\n\n";

	std::string shared_str = "#include <";
	for (auto s : include_list) {
		header_txt += (shared_str + s + ">\n");
	}
	header_txt += "\n\n#endif //PCH_" + cap_str + "_H\n";


	cpp_txt = "\n#include \"pch.h\"\n";

	createFileUsingStr(folder_path, header_file_name, header_txt);
	createFileUsingStr(folder_path, cpp_file_name, cpp_txt);

}


void CreateTemplate::createCMakeLists() {
	std::string cmake_file_name = "CMakeLists.txt";
	std::string module_name = "";
	std::string test_file_name = "_";
	std::string class_file_name = class_name + ".cpp";

	int i = 0;
	for (auto s : class_name) {
		module_name += std::tolower(s);
		if (i == 0) test_file_name += std::tolower(s);
		else test_file_name += s;
		++i;
	}
	test_file_name += "Test.cpp";

	std::string content_txt = "cmake_minimum_required(VERSION 3.7)\n\ninclude_directories(\n        ../Shared\n)\n\n";

	content_txt += ("add_library(" + module_name + " STATIC\n");
	content_txt += "        pch.cpp\n";
	content_txt += "        " + test_file_name;
	content_txt += "\n        " + class_file_name;
	content_txt += "\n        )\n\ntarget_link_libraries(";
	content_txt += module_name;
	content_txt += "\n        modules\n        )";

	createFileUsingStr(folder_path, cmake_file_name, content_txt);

}



