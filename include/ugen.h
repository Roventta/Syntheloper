#pragma once
#include "QPaudioBus.h"
#include "QPBusChannelGrain.h"
#include <array>

#define MaxListener 5

class Ugen {

public:
	virtual void tick(float* buf) {};
	virtual float calcCurrentData() { return 0; };
	Ugen(QPaudioBus* bus, size_t numOfParameters)
	:mChannelGrain(bus, numOfParameters) {
		mQPbus = bus;
	};
	~Ugen() {};

protected:
	QPBusChannelGrain mChannelGrain;
	QPaudioBus* mQPbus;
};
