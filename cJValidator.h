#pragma once
#include "cJSONPP.h"

namespace cjsonpp {

class Validator {
public:
	Validator() {}
	~Validator() {}

	bool validate(const Json& data, const Json& schema);
private:
	bool verifyValue(const Json& value,const char* opt);
	std::string error_;
};


}//namespace
