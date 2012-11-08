//
//  FlMML_Regex.h
//  PicoPicoGames
//

#ifndef __FlMML_Regex_h__
#define __FlMML_Regex_h__

//#define USE_BOOST_REGEX
#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
using namespace boost;
#else

#if TARGET_OS_MAC
#include <regex.h>
#else
#include "regex.h"
#endif
#include <string>
#include <vector>

namespace FlMML {

namespace flmml {

	namespace regex_constants {
		const int match_not_dot_newline = 0;
	}
	
	class regex {
	public:
		regex(std::string& form) {
			cflag = REG_EXTENDED;
			const char* s = form.c_str();
			if (s[0] == '^' && s[form.length()-1] == '$') {
				cflag |= REG_NEWLINE;
			}
			regcomp(&reg,form.c_str(),cflag);
			format = form;
		}
		regex(const char* str) {
			std::string form = str;
			cflag = REG_EXTENDED;
			const char* s = str;
			if (s[0] == '^' && s[form.length()-1] == '$') {
				cflag |= REG_NEWLINE;
			}
			regcomp(&reg,form.c_str(),cflag);
			format = form;
		}
		virtual ~regex() {
			regfree(&reg);
		}
		regex_t reg;
		std::string format;
		int cflag;
	};
	
	class sub_match {
	public:
		std::string::const_iterator second;
	};

	const int max_match = 10;
	
	class smatch {
	public:
		smatch() {
		}
		virtual ~smatch() {
		}
		std::string str(int a) {
			return match_str[a];
		}
		int size() {
			return 0;
		}
		sub_match operator[](int sub) const {
			return match_sub[sub];
		}
		
		sub_match match_sub[max_match];
		std::string match_str[max_match];
	};

}

bool regex_search(std::string& str,flmml::regex& reg,int param=0);
bool regex_search(std::string::const_iterator start,std::string::const_iterator end,flmml::smatch& result,flmml::regex& reg,int param=0);
std::string regex_replace(std::string& cppstr,regex_t* reg,const char* replace,int param=0);
std::string regex_replace(std::string& str,flmml::regex& reg,const char* replaced,int param=0);
std::string regex_replace(std::string& str,flmml::regex& reg,std::string& replaced,int param=0);

using namespace flmml;

}

#endif

#endif
