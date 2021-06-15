#include "cJSONPP.h"
#include <iostream>
#include <vector>
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

class JsonModel {
public:
	virtual bool fromJson(Json& js) = 0;
	virtual Json toJson() = 0;
};

class UserModel :public JsonModel {
public:
	UserModel(const std::string& name,uint32_t id) 
		:name_(name),id_(id) {
	}

	UserModel()
		:id_(0) {
	}


	bool fromJson(Json& js) override {
		if (!js.isObject())
			return false;
		name_ = js["name"].to<std::string>();
		id_ = js["id"].to<int>();
		return true;
	}

	Json toJson() override {
		Json user = Json::object();
		user.add("name", name_);
		user.add("id",(double)id_);
		return user;
	}

private:
	std::string name_;
	uint32_t   id_;
};

class UserListModel :public JsonModel {
public:
	UserListModel(const std::string& str) {
		Json list = Json::parse(str);
		fromJson(list);
	}

	bool fromJson(Json& js) override {
		if (!js.isArray())
			return false;
		users_.clear();
		for (auto itr : js) {
			UserModel user;
			if (user.fromJson(itr)) {
				users_.push_back(user);
			}
		}
		return true;
	}

	Json toJson() override {
		Json users = Json::array();
		for (auto itr : users_) {
			users.add(itr.toJson());
		}
		return users;
	}
private:
	std::vector<UserModel> users_;
};





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

	UserListModel model(R"([
	{"id":1,"name":"user1"},
	{"id":2,"name":"user2"},
	{"id":3,"name":"user3"}])");

	auto js = model.toJson().dump();
	std::cout <<"dump user list model:"<< js << std::endl;

	getchar();
	return 0;
}


