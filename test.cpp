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

void testParse() {
	Json j = Json::parse(R"({
		"a":"a",
		"b":"b"
	})");

	std::cout << "object:" << j.print() << std::endl;

	j = Json::parse("[1,2,3,4,5,6,7,8,9,0]");
	std::cout << "array:" << j.print() << std::endl;

	j = Json::parse("123456");
	std::cout << "number:" << j.print() << std::endl;

	j = Json::parse("null");
	std::cout << "null:" << j.print() << std::endl;

	j = Json::parse("\"hello world\"");
	std::cout << "string:" << j.print() << std::endl;

	j = Json::parse("true");
	std::cout << "true:" << j.print() << std::endl;

	j = Json::parse("false");
	std::cout << "false:" << j.print() << std::endl;
}



int main(int argc, const char** argv) {
	testParse();
	
	Json null;

	Json root = Json::object();

	root.add("num1", 1);
	root.add("num2", 2.1234567);
	root.add("str", "test string");
	root.add("null", null);
	root.add("null2", null.clone());
	root.add("null3", nullptr);

	std::cout << "dump:" << std::endl;
	for (auto it = root.begin(); it != root.end(); ++it) {
		std::cout << it.key() << ":" << (*it).to<const char*>() << std::endl;
	}

	std::cout<< "num1:" << root["num1"].to<int>() << std::endl;

	Json child = Json::array();

	for (int i = 0; i < 10; ++i) {
		child.add(i);
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


