#include "gtest/gtest.h"
#include "cJSONPP.h"
#include "cJRPC.h"
#include "cJValidator.h"
#include <iostream>
#include <vector>
#include <stdio.h>

using namespace cjsonpp;

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

TEST(CJSONPP, TestJson){
	Json j1;
	{
		Json j2("hello world");
		 j1 = j2;
		EXPECT_EQ(j2, j1);
	}

	//j1数据依然有效
	EXPECT_TRUE(j1.isString());

	Json null;
	EXPECT_TRUE(null.isNull());

	Json root = Json::object();

	root.add("num", 1);
	EXPECT_EQ(root["num"].to<int>(), 1);

	Json num = root.detach("num");
	EXPECT_FALSE(root["num"].valied());


	//dump
	root.add("num", 1);
	root.add("str", "hello");
	root.add("b", true);
	root.add("array", {1,2,3,4,5});
	std::cout << "dump:" << std::endl;
	for (auto it = root.begin(); it != root.end(); ++it) {
		std::cout << it.key() << ":" << (*it).dump() << std::endl;
	}

	Json child = Json::array();

	for (int i = 0; i < 10; ++i) {
		child.add(i);
	}
	root.add("child", child);

	Json array = { 1,2,3,4,5,6,7 };
	root.add("array", array);

	std::cout << root.print() << std::endl;

	EXPECT_FALSE(root.empty());
	root.removeAll();

	EXPECT_TRUE(root.empty());
}

TEST(CJSONPP, TestParseValue) {
	Json j = Json::parse(R"({
		"a":"a",
		"b":"b"
	})");
	EXPECT_TRUE(j.isObject());
	ASSERT_EQ(j["a"].to<std::string>(), "a");
	ASSERT_EQ(j["b"].to<std::string>(), "b");

	j = Json::parse("[1,2,3,4,5,6,7,8,9,0]");
	EXPECT_TRUE(j.isArray());
	ASSERT_EQ(j.size(), 10);

	j = Json::parse("123456");
	ASSERT_EQ(j.to<int>(), 123456);

	j = Json::parse("null");
	EXPECT_TRUE(j.isNull());

	j = Json::parse("\"hello world\"");
	EXPECT_TRUE(j.isString());

	j = Json::parse("true");
	EXPECT_TRUE(j.isTrue());

	j = Json::parse("false");
	EXPECT_TRUE(j.isFalse());
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


TEST(CJSONPP, TestModel) {
	UserListModel model(R"([
	{"id":1,"name":"user1"},
	{"id":2,"name":"user2"},
	{"id":3,"name":"user3"}])");

	auto users = model.toJson();
	EXPECT_EQ(users.size(),3);
}

class TestSender :public RpcSender {
public:
	TestSender(Rpc* rpc)
		:rpc_(rpc)
	{
	}
	void send(Json& js) override {
		rpc_->recv(js);
	}
private:
	Rpc* rpc_;
};


TEST(CJSONPP, TestRPC) {
	Rpc rpc;
	TestSender sender(&rpc);
	rpc.registerSender(&sender);
	rpc.registerHandler("add", [](Request& req, Response& resp) {
		int a = req.param("a").to<int>();
		int b = req.param("b").to<int>();
		resp.ok(Json(a + b));
	});

	rpc.registerHandler("mul", [](Request& req, Response& resp) {
		int a = req.param("a").to<int>();
		int b = req.param("b").to<int>();
		resp.ok(Json(a * b));
	});

	Json param = Json::object();
	param.add("a", 1);
	param.add("b", 2);
	rpc.call(Request("add", param), [](Response& resp) {
		EXPECT_EQ(resp.result().to<int>(), 3);
	});

	rpc.call(Request("mul", param), [](Response& resp) {
		EXPECT_EQ(resp.result().to<int>(), 2);
	});
}


TEST(CJSONPP, ValidateNumber) {
	Validator validator;
	Json schema = Json::parse("{\"num\":\"optinal int [1,100)\"}");
	EXPECT_TRUE(validator.validate(Json::parse(R"({})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"num":0})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"num":1})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"num":100})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"num":101})"), schema));


	schema = Json::parse("{\"num\":\"int {1,2,3,4}\"}");
	EXPECT_TRUE(validator.validate(Json::parse(R"({"num":1})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"num":2})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"num":3})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"num":4})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"num":5})"), schema));
}


TEST(CJSONPP, ValidateString) {
	Validator validator;
	Json schema = Json::parse("{\"str\":\"string [1,10)\"}");
	EXPECT_FALSE(validator.validate(Json::parse(R"({})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"str":""})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"str":"1"})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"str":"helloworld"})"), schema));
}

TEST(CJSONPP, ValidateBool) {
	Validator validator;
	Json schema = Json::parse("{\"b\":\"bool\"}");
	EXPECT_FALSE(validator.validate(Json::parse(R"({"b":123})"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"({"b":null})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"b":true})"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"({"b":false})"), schema));
}

TEST(CJSONPP, ValidateArray) {
	Validator validator;
	Json schema = Json::parse(R"(["int"])");
	EXPECT_FALSE(validator.validate(Json::parse(R"({})"), schema));

	//数组不限制长度，故空数组也能通过验证
	EXPECT_TRUE(validator.validate(Json::parse(R"([])"), schema));
	EXPECT_TRUE(validator.validate(Json::parse(R"([1,2,3])"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"(["abc"])"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"([true])"), schema));

	//验证对象数组
	schema = Json::parse(R"([{"a":"string"}])");
	EXPECT_TRUE(validator.validate(Json::parse(R"([{"a":"11"}])"), schema));
	EXPECT_FALSE(validator.validate(Json::parse(R"([{"a":123}])"), schema));
}


TEST(CJSONPP, TestValidate) {
	Validator validator;
	bool valied;
	
	Json schema = Json::parse(R"({
			"num":"int [1,20000]",
			"num2":"int >0",
			"num3":"{1,2,3,4,5}",
			"array":["int"],
			"opt":"optinal float",
			"obj":{
				"item":"string"
			}
	})");

	valied = validator.validate(Json::parse(R"({})"), schema);
	EXPECT_FALSE(valied);


	valied = validator.validate(Json::parse(R"({
			"num":12344,
			"num2":1,
			"num3":2,
			"array":[],
			"obj":{
				"item":""
			}
		})"), schema);
	EXPECT_TRUE(valied);
}

