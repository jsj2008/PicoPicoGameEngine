#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "FlMML_Number.h"

namespace FlMML {

class MEnvelopePoint {
public:
	MEnvelopePoint() {
		next=NULL;
		time = 0;
		level = 0;
	}
	virtual ~MEnvelopePoint() {
		if (next) {
			if (next != this) {
				delete next;
			}
		}
	}
	int time;
	Number level;
	MEnvelopePoint* next;
};

}
