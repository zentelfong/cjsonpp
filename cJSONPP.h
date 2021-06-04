#pragma once
#include "cJSON.h"
#include <string>
#include <iterator>

namespace cjsonpp {

class Json {
public:
	enum Type {
		kFalse,
		kTrue,
		kNull,
		kNumber,
		kString,
		kArray,
		kObject,
	};

	Json()
		: json_(cJSON_CreateNull()) {
	}

	Json(bool b)
		: json_(cJSON_CreateBool(b)) {
	}

	Json(int n)
		: json_(cJSON_CreateNumber(n)) {
	}

	Json(double n)
		: json_(cJSON_CreateNumber(n)) {
	}

	Json(const char* s)
		: json_(cJSON_CreateString(s)) {
	}

	Json(const std::string& s)
		: json_(cJSON_CreateString(s.c_str())) {
	}

	Json(cJSON* j,bool ref = true)
		: json_(j) {
		if(ref)
			cJSON_Ref(json_);
	}

	Json(const Json& j)
		: json_(j.json_) {
		cJSON_Ref(json_);
	}

	~Json() {
		cJSON_Release(json_);
	}

	Json& operator=(const Json& j) {
		cJSON_Ref(j.json_);
		cJSON_Release(json_);
		json_ = j.json_;
		return *this;
	}

	static Json object() {
		return Json(cJSON_CreateObject(),false);
	}

	static Json array() {
		return Json(cJSON_CreateArray(), false);
	}

	Type type() {
		if (json_) {
			return (Type)(json_->type & 0xff);
		}
		else {
			return kNull;
		}
	}

	Json clone() {
		cJSON *j = cJSON_Duplicate(json_, true);
		return Json(j, false);
	}

	template<class T> T to();

	static Json parse(const char* str) {
		cJSON* j = cJSON_Parse(str);
		return Json(j, false);
	}

	static Json parse(const std::string& str) {
		cJSON* j = cJSON_Parse(str.c_str());
		return Json(j, false);
	}

	std::string print() {
		char* s = cJSON_Print(json_);
		std::string s2(s);
		cJSON_Free(s);
		return std::move(s2);
	}

	std::string dump() {
		char* s = cJSON_PrintUnformatted(json_);
		std::string s2(s);
		cJSON_Free(s);
		return std::move(s2);
	}

	int size() {
		return cJSON_GetArraySize(json_);
	}

	Json at(int idx) {
		return Json(cJSON_GetArrayItem(json_, idx));
	}

	Json at(const char* key) {
		return Json(cJSON_GetObjectItem(json_, key));
	}

	void add(const char* key, const Json& item) {
		cJSON_AddItemToObject(json_, key, item.json_);
	}

	void add(const Json& item) {
		cJSON_AddItemToArray(json_, item.json_);
	}

	void insert(int which,const Json& item) {
		cJSON_InsertItemInArray(json_,which, item.json_);
	}

	void replace(int which, const Json& item) {
		cJSON_ReplaceItemInArray(json_, which, item.json_);
	}

	void replace(const char* key, const Json& item) {
		cJSON_ReplaceItemInObject(json_, key, item.json_);
	}

	class Iterator:public std::iterator<std::input_iterator_tag,cJSON> {
	public:
		Iterator(cJSON* j)
			:json_(j) {
		}

		Iterator(const Iterator& j)
			:json_(j.json_) {
		}

		const char* key() {
			return json_->string;
		}

		Json operator*() {
			return Json(json_);
		}

		Iterator& operator++() {
			json_ = json_->next;
			return *this;
		}

		Iterator operator++(int) {
			cJSON* json = json_;
			json_ = json_->next;
			return Iterator(json);
		}
		
		bool operator==(const Iterator& rhs) const { return json_ == rhs.json_; }
		bool operator!=(const Iterator& rhs) const { return json_ != rhs.json_; }
	private:
		cJSON* json_;
	};

	Iterator begin() {
		return Iterator(json_->child);
	}

	Iterator end() {
		return Iterator(nullptr);
	}
private:
	cJSON* json_;
};


template<> double Json::to() {
	if (type() == kNumber) {
		return json_->valuedouble;
	} else {
		return 0.0;
	}
}

template<> int Json::to() {
	if (type() == kNumber) {
		return json_->valueint;
	}
	else {
		return 0;
	}
}

template<> const char* Json::to() {
	if (type() == kString) {
		return json_->string;
	}
	else {
		return "";
	}
}

template<> std::string Json::to() {
	if (type() == kString) {
		return json_->string;
	}
	else {
		return "";
	}
}

template<> bool Json::to() {
	switch (type()) {
	case kTrue:
		return true;
	case kFalse:
		return false;
	case kString:
		return strcmp(json_->valuestring,"1")==0;
	case kNumber:
		return json_->valueint != 0;
	default:
		return false;
	}
}

}//namespace
