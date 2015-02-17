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


#ifndef AIVERTEX_H_
#define AIVERTEX_H_


// In our graph, a vertex is a (cell, direction) couple.
// The vertices are number first by row, then by column, then by direction.
class AIVertex {
public:
	// cost (in number of arrows to place) to go to directions
	// by convention, same dir as current vertex = next cell,
	// otherwise: same cell but rotate
	int right, up, down, left;
	int index;
	int distance;
	AIVertex(int r, int u, int l, int d, int i) {
		right = r;
		up = u;
		left = l;
		down = d;
		index = i;
		distance = 0;

	}
	void zeroAllDirs() {
		right = up = left = down = 0;
	}
	int getWeight(int dir) {
		switch (dir) {
			case 0:
				return right;
			case 1:
				return up;
			case 2:
				return left;
			case 3:
				return down;
			default:
				Functions::fatalError("getWeight: invalid dir");
				return 0; // never reached
		}
	}
	int *weight(int dir) {
			switch (dir) {
				case 0:
					return &right;
				case 1:
					return &up;
				case 2:
					return &left;
				case 3:
					return &down;
				default:
					Functions::fatalError("getWeight: invalid dir");
					return NULL; // never reached
			}
		}
};



#endif /* AIVERTEX_H_ */
