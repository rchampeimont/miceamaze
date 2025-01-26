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


#ifndef AI_H_
#define AI_H_

#include "Functions.h"

#include "AIVertex.h"
#include "AIVertexHeap.h"
#include "AnimalWithDistance.h"

using namespace std;

class AI {
public:
	Maze *maze;
	Game *game;
	int player;
	vector<AIVertex> graph;
	int homeI, homeJ;
	bool snakeMode;

	// Dijkstra's algo stuff
	vector<int> pathToHome;
	vector<bool> dijkstraVisited;
	AIVertexHeap dijkstraQ;
	vector<AnimalWithDistance> sortedAnimals;
	AI(int, Maze*, Game*);
	~AI();
	void init();
	void play();
	void updateGraph();
	void computeDistances();

	// Edge weights for the following things
	static int forwardCost() {
		return 1;
	}
	static int arrowCost() {
		return 10;
	}
	static int infinity() {
		return 1000000;
	}

	int getDistance(int vertexIndex) {
		return graph[vertexIndex].distance;
	}

	int vertexIndexFromCoords(int i, int j, int dir) {
		Functions::verify(i >= 0 && i < maze->height, "vertexIndexFromCoords: invalid i:", i);
		Functions::verify(j >= 0 && j < maze->width, "vertexIndexFromCoords: invalid j:", j);
		Functions::verify(dir >= 0 && dir < 4, "vertexIndexFromCoords: invalid dir", dir);
		return i*4*maze->width + 4*j + dir;
	}

	AIVertex *vertexFromCoords(int i, int j, int dir) {
		return &graph[vertexIndexFromCoords(i, j, dir)];
	}

	int vertexI(int vertexIndex) {
		return vertexIndex / (4*maze->width);
	}

	int vertexJ(int vertexIndex) {
		return (vertexIndex % (4*maze->width))/4;
	}

	int vertexDir(int vertexIndex) {
		return vertexIndex % 4;
	}

	void vertexCoords(int vertexIndex, int *i, int *j, int *dir) {
		Functions::verify(vertexIndex >= 0 && vertexIndex < (int) graph.size(), "vertexCoords: invalid vertex index", vertexIndex);
		if (i != NULL) *i = vertexI(vertexIndex);
		if (j != NULL) *j = vertexJ(vertexIndex);
		if (dir != NULL) *dir = vertexDir(vertexIndex);
	}

	string vertexString(int vertexIndex) {
		int i, j, dir;
		vertexCoords(vertexIndex, &i, &j, &dir);
		return string("(") + Functions::toString(i) + "," + Functions::toString(j) + "," + Functions::toString(dir) + ")";
	}


	int nextVertex(int vertexIndex, int direction);
	int edgeWeight(int u, int v);
	void getPreviousVertices(int vertexIndex, int previousVertices[4]);
};


#endif /* AI_H_ */
