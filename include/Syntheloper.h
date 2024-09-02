#pragma once

#include <vector>
#include "portaudio.h"
#include "unordered_map"

#include "ugen.h"
#include "UgenLink.h"
#include "wavetable.h"

//memory
#include "QPaudioBus.h"


#define BUS_SIZE (4096)

class Syntheloper
{
	private:
		QPaudioBus mBus;
		std::vector<Ugen*> mUgens;
		std::vector<UgenLink*> mUlinks;
		struct CallBackVectors {
			std::vector<Ugen*>* Ugens_ptr;
			std::vector<UgenLink*>* Ulinks_ptr;
		};
		CallBackVectors mCallBackVectors;

		PaStreamParameters outputParameters;
		PaStream* stream;

		PaError err;

		static int CallBack(const void* inputBuffer, void* outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void* userData) {

			float* out = (float*)outputBuffer;
			unsigned long i;

			CallBackVectors* objVectors = reinterpret_cast<CallBackVectors*>(userData);

			std::vector<Ugen*>* ugens = objVectors->Ugens_ptr;
			std::vector<UgenLink*>* ugenLinks = objVectors->Ulinks_ptr;

			for (i = 0; i < framesPerBuffer; i++)
			{

				for (UgenLink* l : *ugenLinks) { l->commune(); }
				for (Ugen* u : *ugens) { u->tick(out); }
				out += 2;

			}
			return paContinue;
		}

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

		void AddUgenLink(UgenLink* ul) {
			mCallBackVectors.Ulinks_ptr->push_back(ul);
		}

		int openStream() {
			err = Pa_OpenStream(
				&stream,
				NULL, /* no input */
				&outputParameters,
				mSampleRate,
				mFramesPerBuffer,
				paClipOff,/* we won't output out of range samples so don't bother clipping them */
				CallBack,
				&mCallBackVectors);
			if (err != paNoError) ErrHandle();

			err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);
			if (err != paNoError) ErrHandle();

			err = Pa_StartStream(stream);
			if (err != paNoError) ErrHandle();

			return err;
		}

		void customSetUp();

		QPaudioBus* getBus() { return &mBus; }

		Wavetable* getWvTble(const std::string tablename,
			float(*writer)(double));

};
