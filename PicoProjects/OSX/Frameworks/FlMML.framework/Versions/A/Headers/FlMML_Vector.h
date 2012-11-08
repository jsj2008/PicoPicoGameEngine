#pragma once

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
using namespace std;

namespace FlMML {

template<class T> class Vector {
public:
	int size_of_array;
	size_t count;
	bool nofree;
	T *array;

	Vector(int size=0) : array(NULL),size_of_array(0),count(-1),nofree(false) {
		if (size > 0) {
			reserve(size);
		}
	}
	virtual ~Vector() {
		if (!nofree) {
			if (array) free(array);
		}
	}
	T& operator[](size_t index) {
		reserve(index+1);
		return array[index];
	}
	void clear() {
		count = -1;
		if (!nofree) {
			if (array) free(array);
			size_of_array = 0;
			array = NULL;
		}
	}
	size_t capacity() {return size_of_array;}
	size_t size() {return count+1;}
	void setSize(size_t size) {
		reserve(size);
		count = size-1;
	};

	void reserve(size_t size) {
		if (!nofree) {
			if (size_of_array < size) {
				if (array) {
					array = (T*)realloc(array,sizeof(T)*(size));
					memset(&array[size_of_array],0,sizeof(T)*(size-size_of_array));
				} else {
					array = (T*)calloc(1,sizeof(T)*(size));
				}
				size_of_array = (int)size;
			}
		}
	}
	
	void push(T& b) {
		count++;
		reserve(count+1);
		array[count] = b;
	}
	
	T& pop() {
		if (count >= 0) {
			--count;
			return array[count+1];
		}
		T a;
		return a;
	}

	void push(const T& b) {
		count++;
		reserve(count+1);
		array[count] = b;
	}
};

}
