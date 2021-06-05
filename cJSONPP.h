#pragma once
#include "cJSON.h"
#include <string>
#include <iterator>

namespace cjsonpp {

class Json {
public:
	enum Type {
		kInvalid = cJSON_Invalid,
		kFalse = cJSON_False,
		kTrue = cJSON_True,
		kNull = cJSON_NULL,
		kNumber = cJSON_Number,
		kString = cJSON_String,
		kArray = cJSON_Array,
		kObject = cJSON_Object,
		kRaw = cJSON_Raw,
	};

	Json()
		: json_(cJSON_CreateNull()) {
	}

	Json(nullptr_t)
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

	Json(Json&& j)
		: json_(j.json_) {
		j.json_ = nullptr;
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

	Type type() const {
		if (json_) {
			return (Type)(json_->type & 0xff);
		}
		else {
			return kInvalid;
		}
	}

	Json clone() const {
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

	std::string print() const {
		char* s = cJSON_Print(json_);
		if(!s){
			return "";
		}
		std::string s2(s);
		cJSON_free(s);
		return std::move(s2);
	}

	std::string dump() const  {
		char* s = cJSON_PrintUnformatted(json_);
		if (!s) {
			return "";
		}
		std::string s2(s);
		cJSON_free(s);
		return std::move(s2);
	}

	bool isNumber() const  {
		return cJSON_IsNumber(json_);
	}

	bool isString() const  {
		return cJSON_IsString(json_);
	}

	bool isTrue() const  {
		return cJSON_IsTrue(json_);
	}

	bool isFalse() const  {
		return cJSON_IsFalse(json_);
	}

	bool isArray() const {
		return cJSON_IsArray(json_);
	}

	bool isObject() const {
		return cJSON_IsObject(json_);
	}

	bool isNull() const {
		return cJSON_IsNull(json_);
	}

	int size() const  {
		return cJSON_GetArraySize(json_);
	}

	Json at(int idx) const {
		return Json(cJSON_GetArrayItem(json_, idx));
	}

	Json at(const char* key) const  {
		return Json(cJSON_GetObjectItem(json_, key));
	}

	Json operator[](int idx) const {
		return Json(cJSON_GetArrayItem(json_, idx));
	}

	Json operator[](const char* key) const {
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

	bool replace(int which, const Json& item) {
		return cJSON_ReplaceItemInArray(json_, which, item.json_);
	}

	bool replace(const char* key, const Json& item) {
		return cJSON_ReplaceItemInObject(json_, key, item.json_);
	}

	void remove(int which) {
		cJSON_DeleteItemFromArray(json_, which);
	}

	void remove(const char* key) {
		cJSON_DeleteItemFromObject(json_, key);
	}

	void removeCs(const char* key) {
		cJSON_DeleteItemFromObjectCaseSensitive(json_, key);
	}

	void removeAll() {
		cJSON_DeleteAllItem(json_);
	}

	bool operator==(const Json& ref) const {
		return cJSON_Compare(json_, ref.json_, true);
	}

	bool operator!=(const Json& ref) const {
		return !cJSON_Compare(json_, ref.json_, true);
	}

	operator bool() const {
		switch (type()) {
		case kInvalid:
		case kNull:
		case kFalse:
			return false;
		default:
			return true;
		}
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
			if (json_)
				return json_->string;
			else
				return "";
		}

		Json operator*() {
			return Json(json_);
		}

		Iterator& operator++() {
			if(json_)
				json_ = json_->next;
			return *this;
		}

		Iterator operator++(int) {
			cJSON* json = json_;
			if(json)
				json_ = json->next;
			return Iterator(json);
		}
		
		bool operator==(const Iterator& rhs) const { return json_ == rhs.json_; }
		bool operator!=(const Iterator& rhs) const { return json_ != rhs.json_; }
	private:
		cJSON* json_;
	};

	Iterator begin() {
		if (json_)
			return Iterator(json_->child);
		else
			return Iterator(nullptr);
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

template<> float Json::to() {
	if (type() == kNumber) {
		return (float)json_->valuedouble;
	}
	else {
		return 0.0f;
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
		return json_->valuestring[0] != '\0';
	case kNumber:
		return json_->valueint != 0;
	default:
		return false;
	}
}

}//namespace
