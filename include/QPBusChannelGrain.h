#include "QPaudioBus.h"
#include <cstddef>
#include <stdexcept>

// a compact chunk of memeory on the bus
// assume that every elements there are floats
class QPBusChannelGrain{
  size_t mSize = 0;
  float* mChannelStart = nullptr;
  QPaudioBus* mBus = nullptr;
  public:
  QPBusChannelGrain(QPaudioBus* bus, size_t size){
      mBus = bus;
      mSize = size;
      mChannelStart = (float*)mBus->Alloc(mSize*sizeof(float));
      if(mChannelStart==nullptr){
          throw std::invalid_argument("QPbus failed during initing a bus channel");
      }
  }
  ~QPBusChannelGrain(){
     mBus->Free((unsigned char*) mChannelStart);
  }

  float Get(size_t index){
      if(mChannelStart && index < mSize){
          return mChannelStart[index];
      }
      return 0;
  }
  void Set(size_t index, float v){
      if(mChannelStart && index < mSize){
          mChannelStart[index]=v;
      }
  }
};
