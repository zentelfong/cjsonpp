#include "cJValidator.h"

namespace cjsonpp {

bool isOptional(const char* opt) {
	return strstr(opt, "Opt") != nullptr;
}

bool Validator::verifyValue(const Json& value, const char* opt) {

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
