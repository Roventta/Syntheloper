#pragma once

#include "QPaudioBus.h"
#include <cstddef>
#include <cstdlib>

void* QPaudioBus::Alloc(size_t size)
{
	size_t chunkSize = size + mMDsize;
	
	//check whether can find a slot in free chunk list
	//first fit algorithm
	if (mFreeListStart != nullptr) {
		//ChunkH* candidate;
		ChunkH* cur = mFreeListStart;
		while (cur->nextFree != nullptr) {
			//check can fit
			if (readHeaderSize(cur->data) == size) {
				freeListRemove(cur);
				//candidate = cur;
				return (char*)cur+sizeof(ChunkH);
				break;
			}
			//split
			if (readHeaderSize(cur->data) > size) {
				freeListRemove(cur);
				//candidate = cur;
				//create new free chunk
				//left over size
				size_t losize = readHeaderSize(cur->data) - size - mMDsize;
				ChunkH leftoverH;
				leftoverH.nextFree = nullptr; leftoverH.previousFree = nullptr;
				leftoverH.data = synthHeaderData(losize, (size_t)0);
				ChunkF leftoverF;
				leftoverF.size = losize;
				writeChunk((char*)cur+mMDsize+size, leftoverH, leftoverF);
				//freelist insert
				freeListAppend((ChunkH*) ((char*)cur+mMDsize+size));

				//reuse register as the allocated
				leftoverH.data = synthHeaderData(size, (size_t)1);
				leftoverF.size = size;
				writeChunk((char*)cur, leftoverH, leftoverF);
				return (char*)cur+sizeof(ChunkH);
			
			}
		}
	
	}

	// free list no good, check whether can fit
	if (mPoolBuoy + chunkSize > (mPool + POOLSIZE))
	{
		return nullptr;
	}


	//create new chunk
	ChunkH header;
	header.data = synthHeaderData(size, (size_t)1);
	header.nextFree = nullptr;
	header.previousFree = nullptr;

	ChunkF footer;
	footer.size = size;

	writeChunk(mPoolBuoy, header, footer);
	char* out = mPoolBuoy + sizeof(ChunkH);
	mPoolBuoy += chunkSize;
	return out;
}

int QPaudioBus::Free(char* ptr)
{
	ChunkH *leftChunk, *rightChunk;
	ChunkF *leftFoot, *rightFoot;
	size_t newLeftSize;

	bool mergeFlag = 0;
	bool mergedLeft = 0;

	//check whether is a QPB pointer
	if (ptr > getFence(1) || ptr < getFence(0)) { return -1; }

	ChunkH* header = reinterpret_cast<ChunkH*>(ptr - sizeof(ChunkH));

	//check whether is a QPB free pointer
	if (readHeaderBool(header -> data) == 0) { return -1; }

	//supress the alloc bit
	std::uint32_t mask = ~( (uint32_t)1 << 31);
	(header->data) = header->data & mask;

	//coalesce
	//left Chunk by footer

mergeLeft:
	leftFoot = reinterpret_cast<ChunkF*> ((char*)header - sizeof(ChunkF));
	leftChunk = reinterpret_cast<ChunkH*> ((char*) leftFoot - leftFoot->size - sizeof(ChunkH));
	if (readHeaderBool(leftChunk->data) == 1) { 
		goto mergeRight; }
	//merge
	newLeftSize = readHeaderSize(leftChunk->data);
	newLeftSize += mMDsize + readHeaderSize(header->data);
	leftChunk->data = synthHeaderData(newLeftSize, (size_t)0);
	leftFoot = H2F(leftChunk);
	leftFoot->size = newLeftSize;
	header = leftChunk;

	mergeFlag = 1;
	mergedLeft = 1;

mergeRight:
	rightChunk = reinterpret_cast<ChunkH*> ((char*) header + mMDsize + readHeaderSize(header->data));
	rightFoot = H2F(rightChunk);
	if (readHeaderBool(rightChunk->data) == 1) { goto FLappend; }
	//merge
	newLeftSize = readHeaderSize(rightChunk->data);
	newLeftSize += mMDsize + readHeaderSize(header->data);
	header->data = synthHeaderData(newLeftSize, (size_t)0);
	H2F(header)->size = newLeftSize;
	if (mergedLeft) {
		//l & r double merge, remove left and right from list and stick merged to start;
		freeListRemove(rightChunk);
		freeListRemove(header);
		
		goto appendstart;
	}
	//trivial merge
	header->nextFree = rightChunk->nextFree;
	header->previousFree = rightChunk->previousFree;
	header->nextFree->previousFree = header;
	header->previousFree->nextFree = header;

	mergeFlag = 1;

FLappend:
	std::cout << mFreeListStart << "\n" << std::endl;
	if (mFreeListStart == nullptr) { mFreeListStart = header; goto end; }
	if (mFreeListStart == header) { goto end; }
	if (mergeFlag == 1) { goto end; }
appendstart:
	//append to the start of the list
	freeListAppend(header);
	
end:
	return 0;
}

int QPaudioBus::Init(size_t size)
{
	// inir free list as non
	mFreeListStart = nullptr;

	POOLSIZE = size;
	mMDsize = sizeof(ChunkF) + sizeof(ChunkH);
	mPool = (char*)malloc(POOLSIZE);
	//place pool fences
	ChunkH header;
	header.data = synthHeaderData((size_t)0, (size_t)1);
	ChunkF footer;
	footer.size = 0;
	//write starting fence
	writeChunk(mPool, header, footer);
	mPoolBuoy = mPool + mMDsize;

	//write ending fence
	writeChunk(mPool + POOLSIZE - mMDsize, header, footer);

	return 0;
}
