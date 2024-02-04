#include "ADSRenv.h"
#include "Math_.h"

void ADSRenv::supress() {
	for (size_t i=0; i < 3; i++) {
		mTurns[i].Set(0, 0);
	}
}

// time needs to be multiplied with samplerate, unit of time is second
ADSRenv::ADSRenv(Vector2 a, Vector2 b, Vector2 c, float totalDuration) {
	//set up first turnning point
	//check sanity, supress and return if fail
	Vector2 temp;
	if (a.x < 0 || a.y < 0) goto sanityCheckFail;
	mTurns[0] = a;
	//second 
	if (b.x < a.x || b.y < 0) goto sanityCheckFail;
	mTurns[1] = b;
	//third
	if (c.x < b.x || c.y<0 || c.x>totalDuration) goto sanityCheckFail;
	mTurns[2] = c;

	printf("one ASDR enveloper inited, three turnning points <t,a> are:\n");
	for (size_t i = 0; i < 3; i++) { Vector2::Print(mTurns[i]); printf("\n"); }
	//temp = Vector2::Lerp(mTurns[0], mTurns[1], 0.5);
	//Vector2::Print(temp);

	mPhase = -1.0;
	mDuration = totalDuration;

	return;

sanityCheckFail:
	supress();
	return;

}

void ADSRenv::tick(float* buf) {
	//env deactivated
	if (mPhase < 0) { return; }
	//env elapsed
	if (mPhase >= mDuration) { mPhase = -1.0; return; }

	//fine just tick
	mPhase += 1.0f;

	//debug
	//printf("env <t=%f, y=%f>\n", mPhase , readCurrentData());
}

float ADSRenv::readCurrentData() {
	if (mPhase < 0 || mPhase >= mDuration) { return 0.0; }
	
	std::array<Vector2, 2> activatedSegment;

	//determine which segements currently
	for (size_t i = 0; i < 4; i++) {
		//set start point
		if (i == 0) { activatedSegment[0].Set(0, 0); }
		if (i != 0) { activatedSegment[0] = mTurns[i - 1]; }

		//set end point
		if (i == 3) { activatedSegment[1].Set(mDuration, 0);}
		if (i != 3) { activatedSegment[1] = mTurns[i]; }

		if (mPhase >= activatedSegment[0].x && mPhase <= activatedSegment[1].x) { goto foundSegment; }
	}
	//no suitable segments
	return 0.0;

foundSegment:
	//lerp between segments by f:=mphase-seg_start
	Vector2 interp = Vector2::Lerp(activatedSegment[0], activatedSegment[1], 
								  (mPhase-activatedSegment[0].x)/(activatedSegment[1].x-activatedSegment[0].x));
	return interp.y;
}

void ADSRenv::Strike()
{
	mPhase = 0;
}
