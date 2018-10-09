// COMP1521 18s1 Assignment 2
// Implementation of heap management system

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myHeap.h"
#include <assert.h>

// minimum total space for heap
#define MIN_HEAP  4096
// minimum amount of space for a free Chunk (excludes Header)
#define MIN_CHUNK 32

#define ALLOC     0x55555555
#define FREE      0xAAAAAAAA

typedef unsigned int uint;   // counters, bit-strings, ...

typedef void *Addr;          // addresses

typedef struct {             // headers for Chunks
	uint  status;             // status (ALLOC or FREE)
	uint  size;               // #bytes, including header
} Header;

static Addr  heapMem;        // space allocated for Heap
static int   heapSize;       // number of bytes in heapMem
static Addr *freeList;       // array of pointers to free chunks
static int   freeElems;      // number of elements in freeList[]
static int   nFree;          // number of free chunks

// initialise heap
int initHeap(int size)
{
    //make sure size is correct
    //if size < 4096 change it to 4096
    if (size < MIN_HEAP){
        size = 4096;
    //make to sure size is a multiple fo 4
    } else { 
        size = size & 3 ?(size | 3) + 1 : size;
    }
    //make heapMem point to the first byte region of the malloc'd region
    heapMem = malloc(size);
    heapSize = size;

    //check to make sure heapMem is valid
    if(heapMem == NULL){
        return -1;
    }

    heapSize = size;

    //initialise the region to be a single large free-space chunk
    Header *freeChunk = heapMem;
    //set up size of the block
    freeChunk->size = size;
    //set the block to be free
    freeChunk->status = FREE;

    //allocate freeList of size = size/MIN_CHUNK
    freeList = calloc(size / MIN_CHUNK, sizeof(Addr));

    //check to make sure FreeList is valid
    if(freeList == NULL){
        return -1;
    }
    //make the first item in freeList = heapMem
    freeList[0] = heapMem;

    //we have one massive element in freeList and one big free chunk
    freeElems = size / MIN_CHUNK;
    //freeElems = 1;
    nFree = 1;

    return 0; 
}

// clean heap
void freeHeap()
{
	free(heapMem);
	free(freeList);
}

// allocate a chunk of memory
void *myMalloc(int size)
{
    //if size < 1 return NULL
    if(size < 1 || nFree <= 0){
        return NULL;
    }
        
	
    size = size & 3 ?(size | 3) + 1 + sizeof(Header): size + sizeof(Header);

    //now try and find the best chunk in freelist to allocate the size
    int i = 0;
    int track = 0; //used to keep track of i
    int bestSize = 0;

    while (i < nFree){
        Header *curr = (Header *)freeList[i];

        //if the current block is larger than size
        //we could potentially use it
        if(curr->size >= size){

            //if bestSize hasn't been intialised
            //bestSize can be curr->size
            if(bestSize == 0){
                track = i;
                bestSize = curr->size;

            //if bestSize has been used and we find a 
            //smaller chunk that works we use that instead
            } else if(curr->size < bestSize && bestSize != 0) {
                track = i;
                bestSize = curr->size;
            }
        }
        i++;
    }

    if(track == 0 && bestSize == 0){
        return NULL;
    }

    //check to see if the bestSize to allocate is less than
    //N + headerSize + MIN_CHUNK
    //so lets make it
    int freeChunkSizeCheck = size + MIN_CHUNK;

	Header* toAlloc = freeList[track];

	//if bestSize is smaller than freeChunkSizeCheck then
	//allocate everything
	if(bestSize <= freeChunkSizeCheck){
            toAlloc->status = ALLOC;
            if(nFree == 1){
                freeList[0] = NULL;
            } else {
                 for(int j = track; j< nFree; j++){
                 freeList[j] = freeList[j + 1];
            }
        }
        nFree--;
        //but if bestSize is > freeChunkSizeCheck we need to split
        } else {
            Header* freeChunk = (Header*)((void*)toAlloc + size);
            freeChunk->status=FREE;
            int freeSize = bestSize - size;
            freeChunk->size = freeSize;

            toAlloc->size = size;
            toAlloc->status = ALLOC;
            freeList[track] = (Addr)freeChunk;    
        } 

        return (void*)toAlloc + sizeof(Header);
}

// free a chunk of memory
void myFree(void *block)
{
     
    // check block is valid
    void* endHeap = (void*)heapMem + heapSize;
    if ((void*)block >= endHeap || (void*) block < (void*)heapMem){
        fprintf(stderr,"Attempt to free unallocated chunk\n");
        exit(1);
    }
    // then check header is valid
    // free the block
    Header* chunk = (Header*)((void*)block - sizeof(Header));
    chunk->status = FREE;
    // set status to freee
    // add it to the freeList array
    for (int i = 0; i < nFree; i++){
        Header* curr = (Header*)freeList[i];
        // freeblock before chunk
        if ((void*)curr + curr->size == (void*)chunk){
            if(i + 1 < nFree){
                Header *next = (Header *)freeList[i + 1];
                // and after
                    if((void*)next == (void*)chunk + chunk->size){
                    //merge together chunk and next and make it free
                        int mergeSize = next->size + chunk->size;
                        chunk->size = mergeSize;
                        //deleteFreeList(i + 1);
                        if(nFree == 1){
                            freeList[0] = NULL;
                        } else {
                            for(int j = i + 1; j < nFree; j++) freeList[j] = freeList[j + 1];
                        }
                    nFree--;
                    }
            }
            int mergeSize = curr->size + chunk->size;
            curr->size = mergeSize;
            return;

            // block free after 
            }else if((void*)curr == (void*)chunk + chunk->size){
                int mergeSize = chunk->size + curr->size;
                chunk->size = mergeSize;
                freeList[i] = chunk;
                return;
            //no adjacent blocks!!
            } else if((void*)curr > (void*)chunk + chunk->size){
                //first case
                if(nFree == 0){
                    freeList[0] = chunk;
                //last case
                } else if(i == nFree){
                    freeList[nFree] = chunk;
                //middle
                } else {
                    for(int j = nFree - 1; j >= i; j--) freeList[j+1] = freeList[j];
                }
                freeList[i] = chunk;
                nFree++;
                return;

            }

       }
   
}

// convert pointer to offset in heapMem
int  heapOffset(void *p)
{
	Addr heapTop = (Addr)((char *)heapMem + heapSize);
	if (p == NULL || p < heapMem || p >= heapTop)
		return -1;
	else
		return p - heapMem;
}

// dump contents of heap (for testing/debugging)
void dumpHeap()
{
	Addr    curr;
	Header *chunk;
	Addr    endHeap = (Addr)((char *)heapMem + heapSize);
	int     onRow = 0;

	curr = heapMem;
	while (curr < endHeap) {
		char stat;
		chunk = (Header *)curr;
		switch (chunk->status) {
			case FREE:  stat = 'F'; break;
			case ALLOC: stat = 'A'; break;
			default:    fprintf(stderr,"Corrupted heap %08x\n",chunk->status); exit(1); break;
		}
		printf("+%05d (%c,%5d) ", heapOffset(curr), stat, chunk->size);
		onRow++;
		if (onRow%5 == 0) printf("\n");
		curr = (Addr)((char *)curr + chunk->size);
	}
	if (onRow > 0) printf("\n");
}
