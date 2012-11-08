#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace FlMML {

class EGPhase {
public:
	static const int next=0;
	static const int attack=1;
	static const int decay=2;
	static const int sustain=3;
	static const int release=4;
	static const int off=5;
};

}
