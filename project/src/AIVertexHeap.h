// Copyright (c) 2012, River CHAMPEIMONT
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef AIVERTEXHEAP_H_
#define AIVERTEXHEAP_H_

#include "Functions.h"
#include "AIVertex.h"

class AIVertexHeap {
	vector<AIVertex*> A;
	vector<int> vertexIndexToHeapIndex;
	int maxHeapSize; // allocate heap size (max currentSize possible)
	int currentSize; // current size = # of vertices currently in the heap
	void heapify(int k);
	bool exists(int k) {
		return k >= 0 && k < currentSize;
	}
	void updateVertexIndexToHeapIndex(int heapIndex) {
		Functions::verify(exists(heapIndex), "AIVertexHeap updateVertexIndexToHeapIndex error");
		vertexIndexToHeapIndex[A[heapIndex]->index] = heapIndex;
	}
	void swap(int a, int b) {
		Functions::verify(exists(a) && exists(b), "AIVertexHeap swap error");
		AIVertex *tmp;
		tmp = A[a];
		A[a] = A[b];
		A[b] = tmp;
		updateVertexIndexToHeapIndex(a);
		updateVertexIndexToHeapIndex(b);
	}
public:
	void reset(vector<AIVertex>*);
	AIVertex *extractMin();
	bool empty() {
		return currentSize == 0;
	}
	void decreaseKey(AIVertex*);
	AIVertexHeap(int);
};


#endif /* AIVERTEXHEAP_H_ */
