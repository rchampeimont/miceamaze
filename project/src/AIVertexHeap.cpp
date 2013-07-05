// Copyright (c) 2012, Raphael CHAMPEIMONT
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


#include "Functions.h"
#include "AIVertexHeap.h"

AIVertexHeap::AIVertexHeap(int heapSize) {
	maxHeapSize = heapSize;
	for (int i=0; i<heapSize; i++) {
		vertexIndexToHeapIndex.push_back(-1);
		A.push_back(NULL);
	}
}
void AIVertexHeap::reset(vector<AIVertex>* g) {
	for (unsigned i=0; i<g->size(); i++) {
		A[i] = &(*g)[i];
		vertexIndexToHeapIndex[(*g)[i].index] = i;
	}
	currentSize = g->size();
}



AIVertex *AIVertexHeap::extractMin() {
	Functions::verify(currentSize > 0, "tried to extract min from empty heap");
	AIVertex *v = A[0];
	A[0] = A[currentSize-1];
	currentSize--;
	if (currentSize > 0) {
		updateVertexIndexToHeapIndex(0);
		heapify(0);
	}
	return v;
}

void AIVertexHeap::heapify(int k) {
	int left = 2*k+1;
	int right = 2*k+2;
	if ((exists(left) && A[k]->distance > A[left]->distance) || (exists(right) && A[k]->distance > A[right]->distance)) {
		if (A[left]->distance < A[right]->distance) {
			swap(k, left);
			heapify(left);
		} else {
			swap(k, right);
			heapify(right);
		}
	}
}

void AIVertexHeap::decreaseKey(AIVertex* v) {
	int k = vertexIndexToHeapIndex[v->index];
	while (k > 0) {
		int parent = (k-1)/2;
		if (A[k]->distance < A[parent]->distance) {
			swap(k, parent);
			k = parent;
		} else {
			break;
		}
	}
}
