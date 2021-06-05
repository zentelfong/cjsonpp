#include "cJSONPP.h"
#include <iostream>
#include <stdio.h>

using cjsonpp::Json;

std::string readFile(const char* filename) {
	FILE* file = fopen(filename, "rb");
	std::string str;
	if (file) {
		fseek(file, 0, SEEK_END);
		auto size = ftell(file);
		str.resize(size);
		fseek(file, 0, SEEK_SET);

		if (fread(&str[0], size, 1, file)) {
			fclose(file);
			return str;
		}
		else {
			fclose(file);
			return "";
		}
	}
	return str;
}

void testRef() {
	Json j1("hello world");
	Json j2 = j1;

	std::cout << j2.to<const char*>() << std::endl;
}



int main(int argc, const char** argv) {
	Json null;

	Json root = Json::object();

	root.add("num1", Json(1));
	root.add("num2", Json(2.1234567));
	root.add("str", Json("test string"));
	root.add("null", null);
	root.add("null2", null.clone());

	std::cout << "dump:" << std::endl;
	for (auto it = root.begin(); it != root.end(); ++it) {
		std::cout << it.key() << ":" << (*it).to<const char*>() << std::endl;
	}

	std::cout<< "num1:" << root["num1"].to<int>() << std::endl;

	Json child = Json::array();

	for (int i = 0; i < 10; ++i) {
		child.add(Json(i));
	}

	root.add("child", child);

	auto out22 = root.print();
	std::cout << out22 << std::endl;

	root.removeAll();

	//std::string data = readFile("../data/canada.json");
	//Json pased = Json::parse(data);
	//auto out = pased.dump();

	getchar();
	return 0;
}


