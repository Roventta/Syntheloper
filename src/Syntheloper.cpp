#include "../include/osc.h"
#include "../include/math_synth.h"
#include "../include/QPCGInstruction.h"
#include "../include/Syntheloper.h"

Syntheloper::Syntheloper():mBusMsger() {
	err = 0;
	mBus.Init((size_t)BUS_SIZE);
	mCallBackVectors.Ugens_ptr = &mUgens;
	mCallBackVectors.Ulinks_ptr = &mUlinks;

	err = Pa_Initialize();
	if (err != paNoError) ErrHandle();

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		ErrHandle();
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	return;
};

static int CallBack(const void* inputBuffer, void* outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void* userData) {
			float* out = (float*)outputBuffer;
			unsigned long i;
			Syntheloper::CallBackVectors* objVectors = reinterpret_cast<Syntheloper::CallBackVectors*>(userData);
			std::vector<Ugen*>* ugens = objVectors->Ugens_ptr;
			std::vector<QPCGInstruction*>* ugenLinks = objVectors->Ulinks_ptr;
			for (i = 0; i < framesPerBuffer; i++)
			{
				for (QPCGInstruction* l : *ugenLinks) { l->commune(); }
				for (Ugen* u : *ugens) { u->tick(out); }
				out += 2;
			}
			return paContinue;
		}

int Syntheloper::openStream(){
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

int Syntheloper::Terminate() {
	err = Pa_StopStream(stream);
	if (err != paNoError) ErrHandle();

	err = Pa_CloseStream(stream);
	if (err != paNoError) ErrHandle();

	Pa_Terminate();
	printf("Test finished.\n");

	for (Ugen* u : mUgens) { free(u); }
	for (QPCGInstruction* l : mUlinks) { free(l); }
	//release wavetables
	for (auto wtPair : mWvTbls) {
		wtPair.second->releaseRawData();
		free(wtPair.second); }

	return err;
}

int Syntheloper::ErrHandle() {
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return err;
}

void Syntheloper::customSetUp() {
	//carrier osc
	Osc* carrier = new Osc(this, midiToFreq(60), 0.5f,
		"sin", [](double i)->float {return (float)sin(i * 2 * M_PI); }
	);
	carrier->Scoped=true;
	Osc* modulator = new Osc(this, 8, 1.0f, "sin", nullptr);
	QPCGInstruction* mod_raise = new QPCGInstruction(this, this->mBusMsger.Get(modulator, 0),
	this->mBusMsger.Get(modulator, 2), this->mBusMsger.Get(modulator, 0),
	[](float l, float r)->float{return (l+r)/2;});

	QPCGInstruction* add1 = new QPCGInstruction(this, this->mBusMsger.Get(carrier, 2),
	this->mBusMsger.Get(modulator, 0), this->mBusMsger.Get(carrier, 2),
	[](float l, float r)->float{return r;});
}

Wavetable* Syntheloper::getWvTble(const std::string tablename,
	float(*writer)(double))
{
	//search the map for existing wave table
	Wavetable* wt = nullptr;
	auto iter = mWvTbls.find(tablename);
	if (iter != mWvTbls.end())
	{
		wt = iter->second;
	}
	//add new wvtable
	else
	{
		wt = new Wavetable(mWaveTableSize, writer);
		std::cout << "new wave table alloced, nametag:" << tablename<< std::endl;
		mWvTbls.emplace(tablename, wt);
	}

	return wt;
}
