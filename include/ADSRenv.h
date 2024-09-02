#pragma once
#include "ugen.h"
#include <array>
#include "Math_.h"

class ADSRenv : protected Ugen
{
private:
	// asume that the segements are linear functions
	// time domain of the Segements are opened at start, closed at end, [a, b)
public:
};
