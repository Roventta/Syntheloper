#pragma once
#include "ugen.h"
#include <array>
#include "Math_.h"

class ADSRenv : public Ugen
{
private:
	// asume that the segements are linear functions
	// time domain of the Segements are opened at start, closed at end, [a, b)
	std::array<Vector2, 3> mTurns;
	float mPhase;
	float mDuration;

	void supress();

public:
	void tick(float* buf) override;
	float readCurrentData() override;

	void Strike();

	ADSRenv(Vector2 a, Vector2 b, Vector2 c, float totalDuration);
	~ADSRenv() {};
};

