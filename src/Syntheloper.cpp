#include "../include/osc.h"
#include "../include/math_synth.h"

#include "../include/Syntheloper.h"

Syntheloper::Syntheloper() {
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


int Syntheloper::Terminate() {
	err = Pa_StopStream(stream);
	if (err != paNoError) ErrHandle();

	err = Pa_CloseStream(stream);
	if (err != paNoError) ErrHandle();

	Pa_Terminate();
	printf("Test finished.\n");

	for (Ugen* u : mUgens) { free(u); }
	for (UgenLink* l : mUlinks) { free(l); }
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
	Osc* sin1 = new Osc(this, midiToFreq(60), 0.5f,
		"sin", [](double i)->float {return (float)sin(i * 2 * M_PI); }
	);
	sin1->Scoped=true;
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
