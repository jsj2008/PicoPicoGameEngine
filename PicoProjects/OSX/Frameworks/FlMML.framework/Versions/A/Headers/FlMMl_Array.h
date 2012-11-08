#pragma once

#include <iostream>
#include <cstddef>
#include <stdexcept>
using namespace std;

namespace FlMML {

template<class T> class Array {
public:
	size_t array_size;
	int array_use_count;
	int stack;
	bool nofree;
	T** array;
	Array(bool _nofree=false) {
		array = NULL;
		array_size = 0;
		stack = -1;
		nofree = _nofree;
		array_use_count = 0;
	}
	virtual ~Array() {
		clear();
		if (array) free(array);
	}
	void clear() {
		if (array) 
		{
			for (int i=0;i<array_size;i++) {
				if (nofree == false) {
					if (array[i] != NULL) delete array[i];
				}
				array[i] = NULL;
			}
		}
		stack = -1;
		//		array_size = 0;
		array_use_count = 0;
	}
	size_t size() {
		return array_use_count;
	}
	void reserve(int size) {
		if (array_size < size) {
			if (array) {
				array = (T**)realloc(array,sizeof(T*)*(size));
				for (size_t i=array_size;i<size;i++) array[i] = NULL;
			} else {
				array = (T**)calloc(1,sizeof(T*)*(size));
			}
			array_size = size;
		}
	}
	void push(T* track) {
		stack ++;
		reserve(stack+1);
		if (nofree == false) {
			if (array[stack] != NULL && array[stack] != track) delete array[stack];
		}
		array[stack] = track;
		if (array_use_count <= stack) array_use_count = stack+1;
	}
	void popWithDelete() {
		if (stack >= 0) {
			if (nofree == false) {
				if (array[stack] != NULL) delete array[stack];
			}
			array[stack] = NULL;
			stack --;
			array_use_count = stack+1;
		}
	}
	T* pop() {
		popWithDelete();
		return NULL;
	}
	//	T& operator[](size_t index) {
	//		if (index < array_use_count) return *array[index];
	//		return *array[0];
	//	}
	T* operator[](size_t index) {
		if (index < array_use_count) return array[index];
		return array[0];
	}
	void assign(int index,T* track) {
		reserve(index+1);
		if (nofree == false) {
			if (array[index] != NULL && array[index] != track) delete array[index];
		}
		array[index] = track;
		if (array_use_count <= index) array_use_count = index+1;
		stack = array_use_count-1;
	}
};

}
