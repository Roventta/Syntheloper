#pragma once

#include <cstdint>
#include <cstring>

class UgenLink {
private:
	void* source;
	void* target;
	size_t size;
	float (*floatOP)(float, float);

public:
	UgenLink(void* tgt, void* src, size_t sz) {
		source = src; target = tgt; size = sz;
		floatOP = nullptr;
	};
	//only take float assigned memory
	UgenLink(void* tgt, void* src, size_t sz, float(*func)(float,float)) {
		source = src; target = tgt; size = sz;
		floatOP = func;
	};
	~UgenLink() {};

	void setTarget(void* tgt) { target = tgt; }
	void commune() {
		if (source != nullptr && target != nullptr)
		{
			//std::cout << "target " << *(float*)target << " source " << *(float*)source << "\n";

			float rec = *(float*)target;
			float send = *(float*)source;
			if (floatOP != nullptr) {
				send = floatOP(rec, send);
			}
			memcpy(target, &send, size);

			//std::cout << "target after write" << *(float*)target << "\n";
 		}
	}
	void setfloatOP(float (*in)(float, float)) { floatOP = in; }
};


