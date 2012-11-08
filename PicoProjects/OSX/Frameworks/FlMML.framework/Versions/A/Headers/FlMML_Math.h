#pragma once
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace FlMML {

class Math {
public:
	static double pow(double a,double b) {return ::pow(a,b);};
	static int max(int a,int b) {return ::max(a,b);}
	static int min(int a,int b) {return ::min(a,b);}
	static double sin(double v) {return ::sin(v);};
	static double ceil(double v) {return ::ceil(v);};
	static double floor(double v) {return ::floor(v);};
	static double log(double v) {return ::log(v);};
	static double round(double v) {return ::round(v);};
	static double random() {return ((double)::rand())/RAND_MAX;};
	static const double PI;
	static const double LOG2E;
};

}
