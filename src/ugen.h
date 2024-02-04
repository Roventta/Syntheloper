#pragma once
#include "memory/QPaudioBus.h"
#include <array>

#define MaxListener 5

class Ugen {

public:
	virtual void tick(float* buf) {};
	virtual float readCurrentData() { return 0; };
	Ugen() { 
		mBusChannel = nullptr; 
		mChanneled = 0;
	};
	~Ugen() {};

	void* mBusChannel;
	virtual void initBusChannel(QPaudioBus* bus_ptr) {};
	std::array<void*, 15> mCGPointers = {};
	void getScoped() { mScoped = true; }
	void deScoped() { mScoped = false; }

protected:
	virtual void readFromBus(void* source, void* target, size_t size) {};
	virtual void writeToBus(void* target, void* payload, size_t size) {};
	bool mChanneled;
	void BusOn() { mChanneled = 1; }
	void BusOff() { mChanneled = 0; }

	bool mScoped = false;
};