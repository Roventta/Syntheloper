#include "../include/ugen.h"
#include <cstddef>

float* BusMsger::Get(Ugen* u, std::size_t i){
    return u->mChannelGrain.Get(i);
}
float* BusMsger::Get(QPCGInstruction* i){
    return i->readRegi();
}
void BusMsger::Set(Ugen* u, std::size_t i, float v){
    u->mChannelGrain.Set(i, v);
}
