#pragma once

#ifndef SYN_H
#define SYN_H

#include <vector>
#include "portaudio.h"
#include "unordered_map"

#include "ugen.h"
#include "wavetable.h"
#include "Syntheloper.fwd.h"

//memory
#include "QPaudioBus.h"
#include "QPCGInstruction.fwd.h"

#define BUS_SIZE (4096)

class Syntheloper
{
    friend QPBusChannelGrain;
    public:
    struct CallBackVectors {
			std::vector<Ugen*>* Ugens_ptr;
			std::vector<QPCGInstruction*>* Ulinks_ptr;
		};

	private:
		QPaudioBus mBus;
		BusMsger mBusMsger;
		std::vector<Ugen*> mUgens;
		std::vector<QPCGInstruction*> mUlinks;
		CallBackVectors mCallBackVectors;
		PaStreamParameters outputParameters;
		PaStream* stream;

		PaError err;
		std::unordered_map<std::string, Wavetable*> mWvTbls;

		static void StreamFinished(void* userData)
		{
			printf("Stream Completed\n");
		}


	protected:
		const int mSampleRate = 44100;
		const int mFramesPerBuffer = 1024;
		const int mWaveTableSize = 1024;


	public:
		Syntheloper();
		~Syntheloper() {};

		int Terminate();
		int ErrHandle();

		void AddUgen(Ugen* u) {
			mCallBackVectors.Ugens_ptr->push_back(u);
		}
		void AddUgenLink(QPCGInstruction* ul) {
			mCallBackVectors.Ulinks_ptr->push_back(ul);
		}

		int openStream();
		void customSetUp();
		QPaudioBus* getBus() { return &mBus; }
		Wavetable* getWvTble(const std::string tablename,
			float(*writer)(double));

};

#endif
