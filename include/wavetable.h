#pragma once

#include <stdlib.h>
#include <stdio.h>

#ifndef WT_HEADER
#define WT_HEADER

class Wavetable {
private:
	int mTableSize;
	float* mRawdata;

public:
	Wavetable(int tableSize, float(*writer)(double)) { 
		mTableSize = tableSize; 
		mRawdata = (float*)malloc(sizeof(float) * tableSize);
		wavetableFactory(writer);
	};
	
	~Wavetable() {};

	void wavetableFactory(float (*writer)(double)) {
		if (mRawdata == nullptr) { return; };

		for (int i = 0; i < mTableSize; i++) {
			mRawdata[i] = writer((double)i / (double)mTableSize);
		}
	};

	float readTable(int index) { return mRawdata[index]; }
	int getTableSize() { return mTableSize; }
	void releaseRawData() { free(mRawdata); }
};

#endif