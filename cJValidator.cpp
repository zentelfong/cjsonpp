#include "cJValidator.h"

namespace cjsonpp {


/*
##### 数据类型

​	int,float,bool,string

##### 是否可选，默认为必选

​	optional 可省略为opt

##### 范围区间表示

​	(1,100) 表示 1 < x < 100
​	[1,100) 表示 1 <= x < 100
​	(1,100] 表示 1 < x <= 100
​	[1,100] 表示 1 <= x <= 100
​	{1,2,3,4,5} 表示在集合内

##### 示例如下：

1. int
2. int [1,100]
3. optioal int (1,100)
4. float (0,1)
5. string [10,20]  表示字符串长度范围

*/

enum Type{
	kUnknow = -1,
	kInt = 0,
	kFloat,
	kBool,
	kString,
	kOptinal
};


static const char* keyword[] = {
	"int",
	"float",
	"bool",
	"string",
	"optinal",
	nullptr
};

Type strToType(const char* str) {
	const char** word = keyword;
	int type = 0;
	while (*word) {
		if (strcmp(*word, str) == 0) {
			return (Type)type;
		}
		++word;
		++type;
	}
	return kUnknow;
}

//检测范围
bool checkRange(const Json& value, const char* str) {
	switch (str[0]) {
	case '(':
	case '[':

	case '{':


	default:
		break;
	}

}


bool isOptional(const char* opt) {
	return strstr(opt, "optinal") != nullptr;
}


bool Validator::verifyValue(const Json& value, const char* opt) {

	if (!value) {
		return isOptional(opt);
	}

	//根据空格分割字符串
	//检测类型
	//检测范围


	return true;
}


bool Validator::validate(const Json& data, const Json& schema) {
	if (!data) {
		return false;
	}
	switch (schema.type()) {
	case Json::kArray:
		if (!data.isArray()) {
			error_ = std::string(data.name()) + " must array";
			return false;
		}

		{
			Json s = schema.at(0);
			if (s) {
				for (auto item : data) {
					if (!validate(item, s)) {
						return false;
					}
				}
			}
		}
		break;
	case Json::kObject:
		if (!data.isObject()) {
			error_ = std::string(data.name()) + " must object";
			return false;
		}
		for (auto itr = schema.begin(); itr != schema.end(); ++itr) {
			Json child = data.at(itr.key());
			if (child ) {
				if(!validate(child, *itr))
					return false;
			} else {
				if (!isOptional((*itr).to<const char*>())) {
					error_ = std::string(itr.key()) + " is not exist";
					return false;
				}
			}
		}
		break;
	case Json::kString:
		if (!verifyValue(data, schema.to<const char*>())) {
			return false;
		}
		break;
	default:
		error_ = "invalied schema value";
		return false;
	}
	return true;
}

}//namespace
