#pragma once
#include <math.h>

#ifndef MS_HEADER
#define MS_HEADER

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

static double midiToFreq(double midi) {
	return pow(2, (midi - 69) / 12) * 440;
}

static double freqToMidi(double freq) {
	return 12 * log(freq / 440.0) + 69;
}

#endif