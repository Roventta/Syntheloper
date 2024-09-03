#pragma once
#include "QPCGInstruction.h"
#include "QPaudioBus.h"
#include "QPBusChannelGrain.h"
#include <array>
#include <cstddef>

#define MaxListener 5

class Ugen;
class BusMsger{
    public:
    BusMsger(){};
    float* Get(Ugen* u, std::size_t i);
    float* Get(QPCGInstruction* i);
    void Set(Ugen* u, std::size_t i, float v);
};

class Ugen {
friend BusMsger;
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
