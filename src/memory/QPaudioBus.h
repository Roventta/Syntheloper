#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

//#define POOLSIZE 280

struct ChunkH {
	//contain size and free status, read them via bit mask
	size_t data;
	ChunkH* previousFree;
	ChunkH* nextFree;
};

struct ChunkF {
	size_t size;
};

class QPaudioBus
{
private: 
	unsigned char* mPool;
	unsigned char* mPoolBuoy;
	size_t mPoolSize;
	size_t mMDsize;
	ChunkH* mFreeListStart;

	size_t POOLSIZE;

	// chunk header and footer data size
	size_t mCHsize;
	size_t mCFsize;

	size_t readHeaderSize(size_t data) {
		std::uint32_t mask = (uint32_t)1 << 31;
		return (size_t)((uint32_t)data & (~mask));
	}

	size_t synthHeaderData(size_t size, size_t alloc) {
		return size | alloc << 31;
	}

	size_t readHeaderBool(size_t data) {
		return (size_t)(uint32_t)data >> 31;
	}

	ChunkF* H2F(ChunkH* h) {
		return reinterpret_cast<ChunkF*> ((char*)h + sizeof(ChunkH) + readHeaderSize (h->data));
	}

	void freeListRemove(ChunkH* h) {
		if (h->previousFree)
			h->previousFree->nextFree = h->nextFree;
		if (h->nextFree)
			h->nextFree->previousFree = h->previousFree;

		//if removing first, set the linked list start to be its next
		if (mFreeListStart == h) {
			mFreeListStart = h->nextFree;
		}
	}

	void freeListAppend(ChunkH* header) {
		//append to the start of the list

		//if it already is the start, do nothing
		if (mFreeListStart == header) { return; }

		mFreeListStart->previousFree = header;
		header->nextFree = mFreeListStart;
		header->previousFree = nullptr;
		mFreeListStart = header;
	}

	int writeChunk(unsigned char* target, ChunkH h, ChunkF f) {
		//write header
		memcpy(target, &h, sizeof(ChunkH));
		//move pointer one chunkH up, and one data size up
		char* ptr = (char*)target + sizeof(ChunkH) + readHeaderSize(h.data);
		memcpy(ptr, &f, sizeof(ChunkF));
		return 0;
	}

	unsigned char* getFence(int which) {
		if(which == 0) return mPool; 
		
		if (which == 1) {
			return mPool + POOLSIZE - mMDsize;
		}
	}

public:

	void* Alloc(size_t size);
	int Free(unsigned char* ptr);
	int Init(size_t size);

	int PoolWalk() {
		int size = 0;
		std::cout << "sequential walk\n";
		unsigned char* current = getFence(0);
		while (current <= getFence(1)) {
			ChunkH* temp = reinterpret_cast<ChunkH*>(current);
			std::cout << "one chunk, size is " << readHeaderSize(temp->data) << " alloc as " << readHeaderBool(temp->data) << "\n";
			std::cout << "corresponding foot is " << H2F(temp)->size << "\n";
			std::cout << "--------\n";
			current += mMDsize + readHeaderSize(temp->data);
			size += mMDsize + readHeaderSize(temp->data);
		}
		std::cout << std::endl;
		return size;
	}

	int FreeWalk() {
		std::cout << "free linked list walk\n";
		int size = 0;
		ChunkH* current = mFreeListStart;
		while (current!= nullptr) {
			ChunkH* temp = reinterpret_cast<ChunkH*>(current);
			std::cout << "one chunk, size is " << readHeaderSize(temp->data) << " alloc as " << readHeaderBool(temp->data) << "\n";
			std::cout << "corresponding footer think size is " << H2F(temp)->size << "\n";
			std::cout << "--------\n";
			current = current->nextFree;

			size += readHeaderSize(temp->data);
		}
		std::cout << std::endl;
		return size;

	}

	size_t getMDsize() { return mMDsize; }

	void BusDestroy() { free(mPool); }

};




