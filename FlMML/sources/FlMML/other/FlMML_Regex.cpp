#include <string>
#include "FlMML_Regex.h"
#include <string.h>
#include <stdlib.h>

namespace FlMML {

#ifdef USE_BOOST_REGEX
#else

static char *regmatch_replace(char *whole_string, const regmatch_t *match, const char *replaced)
{
	const size_t src_len = (size_t)(match->rm_eo - match->rm_so);
	const size_t dst_len = strlen(replaced);
	
	if(src_len < dst_len) {
		whole_string = (char*)realloc(whole_string, strlen(whole_string) + (dst_len - src_len) + 1);
		if(!whole_string) return NULL;
	}
	
	if(src_len != dst_len) {
		memmove(whole_string + match->rm_so + dst_len, whole_string + match->rm_eo, strlen(whole_string + match->rm_eo) + 1);
	}
	strncpy(whole_string + match->rm_so, replaced, dst_len);
	
	return whole_string;
}

static size_t regmatch_count(const char *s, const regex_t *reg,int param=0)
{
	size_t match_count = 0;
	regmatch_t pmatch;
	
	if(!reg) return 0;
	
	for(/* */;regexec(reg,s,1,&pmatch,param) != REG_NOMATCH;s+=pmatch.rm_eo) {
		match_count++;
	}
	
	return match_count;
}

bool regex_search(std::string& str,flmml::regex& reg,int param)
{
	if (str.length() > 0) {
		regmatch_t match;
		regmatch_t* pmatch = &match;
		long long t=0;
		const char *s = str.c_str();
		if (*s) {
			while (1) {
		//printf("--\n%s\n",&s[t]);
				bool match = false;
				if (regexec(&reg.reg,&s[t],1,pmatch,0) != REG_NOMATCH) {
		//printf("match %d,%d\n",(int)pmatch[0].rm_so+t,(int)pmatch[0].rm_eo+t);
					regmatch_t m = pmatch[0];
					m.rm_so += t;
					m.rm_eo += t;
					t = pmatch[0].rm_so+t;
					if (m.rm_so != m.rm_eo) {
						return true;
					}
				}
				if (!match) {
					if (reg.cflag & REG_NEWLINE) {
						break;
					}
					while (s[t]!=0) {
						if (s[t] == '\n' || s[t] == '\r') {
							t ++;
							break;
						}
						t ++;
					}
					if (s[t] == 0) break;
				}
			}
		}
	}
	return false;
}

bool regex_search(std::string::const_iterator start,std::string::const_iterator end,flmml::smatch& result,flmml::regex& reg,int param)
{
	regmatch_t pmatch[max_match];
	std::string str(start,end);
	if (str.length() > 0) {
		const char* s = str.c_str();
		long long t=0;
		if (*s) {
			while (1) {
				bool match = false;
				if (regexec(&reg.reg,&s[t],max_match,pmatch,0) != REG_NOMATCH) {
					for (int i=0;i<max_match;i++) {
						if (pmatch[i].rm_so != pmatch[i].rm_eo) {
							result.match_str[i] = str.substr((unsigned long)(pmatch[i].rm_so+t),(unsigned long)(pmatch[i].rm_eo+t-(pmatch[i].rm_so+t)));
							result.match_sub[i].second = start+(int)(pmatch[i].rm_eo+t);
							match = true;
						} else {
							result.match_str[i] = "";
							result.match_sub[i].second = end;
						}
					}
					return true;
				}
				if (!match) {
					if (reg.cflag & REG_NEWLINE) {
						break;
					}
					while (s[t]!=0) {
						if (s[t] == '\n' || s[t] == '\r') {
							t ++;
							break;
						}
						t ++;
					}
					if (s[t] == 0) break;
				}
			}
		}
		for (int i=0;i<max_match;i++) {
			result.match_str[i] = "";
			result.match_sub[i].second = end;
		}
	}
	return false;
}

std::string regex_replace(std::string& str,regex_t* reg,const char* replace,int cflag)
{
	regmatch_t match;
	regmatch_t* pmatch = &match;
	long long t=0;
	if (str.length() > 0) {
		char *s = (char*)malloc(str.length()+1);
		strcpy(s,str.c_str());
		if (*s) {
			while (1) {
//printf("--\n%s\n",&s[t]);
				bool match = false;
				if (regexec(reg,&s[t],1,pmatch,0) != REG_NOMATCH) {
//printf("match %d,%d\n",(int)pmatch[0].rm_so+t,(int)pmatch[0].rm_eo+t);
					regmatch_t m = pmatch[0];
					m.rm_so += t;
					m.rm_eo += t;
					if (m.rm_so != m.rm_eo) {
						s = regmatch_replace(s,&m,replace);
						t = pmatch[0].rm_so+strlen(replace)+t;
						match = true;
					}
				}
				if (!match) {
					if (cflag & REG_NEWLINE) {
						break;
					}
					while (s[t]!=0) {
						if (s[t] == '\n' || s[t] == '\r') {
							t ++;
							break;
						}
						t ++;
					}
					if (s[t] == 0) break;
				}
			}
		//printf("==\n%s\n",s);
		}
		str = s;
		free(s);
	}
	return str;
}

std::string regex_replace(std::string& cppstr,flmml::regex& reg,const char* replace,int param)
{
	return regex_replace(cppstr,&reg.reg,replace,reg.cflag);
}

std::string regex_replace(std::string& str,flmml::regex& reg,std::string& replaced,int param)
{
	return regex_replace(str,reg,replaced.c_str(),reg.cflag);
}

#endif

}
