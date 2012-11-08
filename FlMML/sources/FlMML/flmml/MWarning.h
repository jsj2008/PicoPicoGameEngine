#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace FlMML {

class MWarning {
public:
	static const int UNKNOWN_COMMAND=0;
	static const int UNCLOSED_REPEAT=1;
	static const int UNOPENED_COMMENT=2;
	static const int UNCLOSED_COMMENT=3;
	static const int RECURSIVE_MACRO=4;
	static const int UNCLOSED_ARGQUOTE=5;
	static const int UNCLOSED_GROUPNOTES=6;
	static const int UNOPENED_GROUPNOTES=7;
	static const string s_string[];
	static string returnstring;
	static string getString(int warnId,string str) {
		char t[1024];
		::sprintf(t,s_string[warnId].c_str(),str.c_str());
		returnstring=t;
		return returnstring;
	}
};

}
