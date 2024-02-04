#pragma once

#ifndef OSC_H
#define OSC_H

#include "Syntheloper.h"
#include "ugen.h"
#include "wavetable.h"
#include "math_synth.h"
#include "memory/QPaudioBus.h"

//[0]:out, [1]:freq, [2]:amp, [3]:phase; 

class Osc : public Ugen {
private:
	Wavetable* mWavetable;
	float mFrequency;

	float mPhase;
	float mAmplitude=0;

	Syntheloper* mSystem;

public:
	Osc();
	Osc(float freq, float amp);
	Osc(Syntheloper* sys, float freq, float amp,
		std::string tblname, float(*writer)(double),
		bool onbus, bool alone);
	~Osc() {};
	void tick(float* buf) override;

	float readCurrentData() override{
		float tempF, tempA, tempP;

		if (mChanneled) {
			//struct ChannelGrain *temp = (ChannelGrain*)mBusChannel;
			readFromBus(&tempF, mCGPointers[1], sizeof(float));
			readFromBus(&tempA, mCGPointers[2], sizeof(float));
			readFromBus(&tempP, mCGPointers[3], sizeof(float));
		}

		else {
			tempF = mFrequency;
			tempA = mAmplitude;
			tempP = mPhase;
		}

		float data_floor = mWavetable->readTable((int)floor(tempP));
		float data_ceiling = mWavetable->readTable((int)ceil(tempP));
		double temp;
		float intered_tmp = data_floor + (data_ceiling - data_floor) * (float)modf(tempP, &temp);
		return tempA * intered_tmp; 
	}

	void setWavetable(Wavetable* wavetable) { mWavetable = wavetable;};

	void setAmplitude(float amp) { if (amp >= 0) mAmplitude = amp;};

	struct ChannelGrain {
		float out;
		float frequency;
		float amplitude;
		float phase;
	};

	void loadDefault2Bus() {
		if (mBusChannel == nullptr) { return; }
		struct ChannelGrain temp = {0, mFrequency, mAmplitude, mPhase};
		writeToBus(mBusChannel, &temp, sizeof(ChannelGrain));
	}

//private:
	void initBusChannel(QPaudioBus* bus_ptr) override{
		if (bus_ptr == nullptr) { return; }
		mBusChannel = bus_ptr->Alloc(sizeof(ChannelGrain));
		if (mBusChannel == nullptr) { return; }
		loadDefault2Bus();
		BusOn();
		// regist channel grains' memory locations
		mCGPointers[0] = &((ChannelGrain*)mBusChannel)->out;
		mCGPointers[1] = &((ChannelGrain*)mBusChannel)->frequency;
		mCGPointers[2] = &((ChannelGrain*)mBusChannel)->amplitude;
		mCGPointers[3] = &((ChannelGrain*)mBusChannel)->phase;
		
	}

protected:
	void writeToBus(void* target, void* payload, size_t size) override{
		if (mBusChannel == nullptr) { return; }
		memcpy(target, payload, size);
	}
	void readFromBus(void* target, void* src, size_t size) override {
		if (mBusChannel == nullptr) { return; }
		memcpy(target, src, size);
	}

};

#endif