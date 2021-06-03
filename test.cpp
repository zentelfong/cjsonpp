#include "cJSONPP.h"
#include <iostream>
using cjsonpp::Json;

int main(int argc, const char** argv) {
	Json null;

	Json root = Json::object();

	root.add("num1", Json(1));
	root.add("num2", Json(2.1234567));
	root.add("str", Json("test string"));
	root.add("null", null);

	std::cout << "iterator root" << std::endl;
	for (auto it = root.begin(); it != root.end(); ++it) {
		std::cout << it.key() << ":" << (*it).to<const char*>() << std::endl;
	}

	Json child = Json::array();

	for (int i = 0; i < 10; ++i) {
		child.add(Json(i));
	}

	root.add("child", child);

	std::cout << root.print() << std::endl;
	//getchar();
	return 0;
}


