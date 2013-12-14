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


#include "Program.h"

AI::AI(int p, Maze *m, Game *g) : dijkstraQ(m->width * m->height * 4) {
	player = p;
	maze = m;
	game = g;
	homeI = 0;
	homeJ = 0;
	snakeMode = false;
}

AI::~AI() {

}

void AI::init() {
	int c = 0;
	for (int i=0; i<maze->height; i++) {
		for (int j=0; j<maze->width; j++) {
			for (int dir=0; dir<4; dir++) {
				graph.push_back(AIVertex(0, 0, 0, 0, c));
				Functions::verify(vertexI(graph.size()-1) == i);
				Functions::verify(vertexJ(graph.size()-1) == j);
				Functions::verify(vertexDir(graph.size()-1) == dir);
				c++;
			}
		}
	}

	// Init distances array
	for (int i=0; i<c; i++) {
		pathToHome.push_back(0);
		dijkstraVisited.push_back(false);
	}

	updateGraph();
}

void AI::updateGraph() {
	for (int i=0; i<maze->height; i++) {
		for (int j=0; j<maze->width; j++) {
			for (int dir=0; dir<4; dir++) {
				int v = vertexIndexFromCoords(i, j, dir);
				graph[v].right = (dir != 0) ? arrowCost() : forwardCost();
				graph[v].up    = (dir != 1) ? arrowCost() : forwardCost();
				graph[v].left  = (dir != 2) ? arrowCost() : forwardCost();
				graph[v].down  = (dir != 3) ? arrowCost() : forwardCost();
			}
		}
	}


	if (snakeMode) {
		// choose a victim
		int victim = -1;
		int bestScore = 0;
		for (int k=0; k<game->players; k++) {
			if (k == player) {
				// Do not choose myself as the victim
				continue;
			}
			if (victim < 0 || game->scores[k] > bestScore) {
				victim = k;
				bestScore = game->scores[k];
			}
		}
		// Where is the victim's house?
		for (unsigned k=0; k<maze->houses.size(); k++) {
			if (maze->houses[k].player == victim) {
				homeI = maze->houses[k].i;
				homeJ = maze->houses[k].j;
			}
		}
	} else {
		// Where is the player's house?
		for (unsigned k=0; k<maze->houses.size(); k++) {
			if (maze->houses[k].player == player) {
				homeI = maze->houses[k].i;
				homeJ = maze->houses[k].j;
			}
		}
	}

	// We cannot cross walls
	for (vector<Wall>::iterator wall = maze->vWalls.begin(); wall != maze->vWalls.end(); ++wall) {
		if (wall->j > 0 && wall->j < maze->width) {
			vertexFromCoords(wall->i, wall->j-1, 0)->right = infinity();
			vertexFromCoords(wall->i, wall->j, 2)->left = infinity();
		}
	}
	for (vector<Wall>::iterator wall = maze->hWalls.begin(); wall != maze->hWalls.end(); ++wall) {
		if (wall->i > 0 && wall->i < maze->height) {
			vertexFromCoords(wall->i-1, wall->j, 1)->up = infinity();
			vertexFromCoords(wall->i, wall->j, 3)->down = infinity();
		}
	}

	// Take in account corners: they allow rotation without placing arrow
	for (int direction=0; direction<4; direction++) {
		for (int i=0; i<maze->size; i++) {
			for (int j=0; j<maze->size; j++) {
				bool hasArrow = false;
				for (unsigned k=0; k<maze->arrows.size(); k++) {
					Arrow *arrow = &maze->arrows[k];
					if (arrow->i == i && arrow->j == j) {
						hasArrow = true;
						break;
					}
				}
				if (hasArrow) continue;
				AIVertex *v = vertexFromCoords(i, j, direction);
				int noseFutureJ, noseFutureI;
				int nextVertexIndex = nextVertex(v->index, direction);
				if (nextVertexIndex != -1) {
					vertexCoords(nextVertexIndex, &noseFutureI, &noseFutureJ, NULL);
					if (maze->crossesWall(j, i, noseFutureJ, noseFutureI)) {
						int leftDir = direction == 3 ? 0 : direction + 1;
						int rightDir = direction == 0 ? 3 : direction - 1;
						int udir = leftDir == 3 ? 0 : leftDir + 1;

						IntXY left = maze->nextCell(j, i, leftDir);
						bool leftOK = !maze->crossesWall(j, i, left.x, left.y);
						IntXY right = maze->nextCell(j, i, rightDir);
						bool rightOK = !maze->crossesWall(j, i, right.x, right.y);
						if (leftOK && !rightOK) {
							// animal would go left
							*(v->weight(leftDir)) = 0;
						} else if (rightOK && !leftOK) {
							// animal would go right
							*(v->weight(rightDir)) = 0;
						} else if (rightOK && leftOK) {
							// Can go both left and right, so animal will choose at random
							// so we cannot assume anything.
						} else {
							// Animal will have to U-turn
							*(v->weight(udir)) = 0;
						}
					}
				}
			}
		}
	}

	// Take in account existing arrows
	for (unsigned k=0; k<maze->arrows.size(); k++) {
		Arrow *arrow = &maze->arrows[k];
		int pointedI, pointedJ;
		arrow->pointsTo(&pointedI, &pointedJ);
		// check the arrow actually "works" (not points to a wall)
		if (!maze->crossesWall(arrow->j, arrow->i, pointedJ, pointedI)) {
			for (int fromdir=0; fromdir<4; fromdir++) {
				for (int todir=0; todir<4; todir++) {
					if (todir == arrow->direction) {
						if (arrow->player == player) {
							// if it is our arrow, then it "costs"
							*vertexFromCoords(arrow->i, arrow->j, fromdir)->weight(todir) = arrowCost();
						} else {
							*vertexFromCoords(arrow->i, arrow->j, fromdir)->weight(todir) = 0;
						}
					} else {
						*vertexFromCoords(arrow->i, arrow->j, fromdir)->weight(todir) = infinity();
					}
				}
			}
		}
	}


	// Take in account other's players houses:
	// An animal can't "escape" such a cell.
	for (unsigned k=0; k<maze->houses.size(); k++) {
		House *house = &maze->houses[k];
		if (house->player != player) {
			for (int dir=0; dir<4; dir++) {
				graph[vertexIndexFromCoords(house->i, house->j, dir)].right = infinity();
				graph[vertexIndexFromCoords(house->i, house->j, dir)].left = infinity();
				graph[vertexIndexFromCoords(house->i, house->j, dir)].up = infinity();
				graph[vertexIndexFromCoords(house->i, house->j, dir)].down = infinity();
			}
		}
	}

	// We will make house/right the destination, so we zero the cost
	// from house/anydir to house/right.
	graph[vertexIndexFromCoords(homeI, homeJ, 0)].zeroAllDirs();
	graph[vertexIndexFromCoords(homeI, homeJ, 1)].zeroAllDirs();
	graph[vertexIndexFromCoords(homeI, homeJ, 2)].zeroAllDirs();
	graph[vertexIndexFromCoords(homeI, homeJ, 3)].zeroAllDirs();
}


int AI::nextVertex(int vertexIndex, int direction) {
	int i, j, vdir;
	vertexCoords(vertexIndex, &i, &j, &vdir);
	if (vdir == direction) {
		switch (direction) {
		case 0:
			j++;
			break;
		case 1:
			i++;
			break;
		case 2:
			j--;
			break;
		case 3:
			i--;
			break;
		}
		if (i >= maze->height || i < 0 || j >= maze->width || j < 0) {
			return -1;
		} else {
			return vertexIndexFromCoords(i, j, direction);
		}
	} else {
		return vertexIndexFromCoords(i, j, direction);
	}
}


// Return edge weight u -> v.
int AI::edgeWeight(int u, int v) {
	int u_i, u_j, u_dir, v_i, v_j, v_dir;
	vertexCoords(u, &u_i, &u_j, &u_dir);
	vertexCoords(v, &v_i, &v_j, &v_dir);
	bool ok = true;
	if (u_dir != v_dir) {
		// We want a "change direction and stay on same cell" edge
		ok = ok && u_i == v_i;
		ok = ok && u_j == v_j;
	} else { // u_dir == v_dir
		// We want a "go straight to next cell" edge
		// We know, from u, which vertex v is (check provided v is this one)
		int jp = (u_dir == 0 ? u_j+1 : (u_dir == 2 ? u_j-1 : u_j));
		int ip = (u_dir == 1 ? u_i+1 : (u_dir == 3 ? u_i-1 : u_i));
		ok = ok && ip == v_i;
		ok = ok && jp == v_j;
	}
	if (!ok) {
		Functions::fatalError(string("edgeWeight: No edge from ") + vertexString(u) + " to " + vertexString(v));
	}
	return graph[u].getWeight(v_dir);
}

// For a given vertex u, give all vertices v such that there is an arrow v->u.
// preivousVertices must be a pointer to an int array of length 4.
// Vertices indices will be written to previousVertices.
// Only != -1 such indices count, in case there is left previous vertices than 4.
void AI::getPreviousVertices(int vertexIndex, int previousVertices[4]) {
	int i, j, dir;
	vertexCoords(vertexIndex, &i, &j, &dir);
	for (int k=0; k<4; k++) {
		if (k != dir) {
			previousVertices[k] = vertexIndexFromCoords(i, j, k);
		}
	}
	int jp = (dir == 0 ? j-1 : (dir == 2 ? j+1 : j));
	int ip = (dir == 1 ? i-1 : (dir == 3 ? i+1 : i));
	if (ip >= 0 && ip < maze->height && jp >= 0 && jp < maze->width) {
		previousVertices[dir] = vertexIndexFromCoords(ip, jp, dir);
		//Functions::verify(vertexI(previousVertices[dir]) == ip);
	} else {
		previousVertices[dir] = -1;
	}
}

// Compute distances from house using Dijkstra's algorithm
// distance = number of arrows necessary to reach home
void AI::computeDistances() {
	// Here we use Dijkstra algorithm. Since we want the distance TO the house,
	// we call neighbor what are in fact the previous vertices, because the algorithm
	// we compute destination FROM a source.

	for (unsigned i=0; i<graph.size(); i++) {
		graph[i].distance = infinity();
		pathToHome[i] = -1;
		dijkstraVisited[i] = false;
	}

	dijkstraQ.reset(&graph);

	int source = vertexIndexFromCoords(homeI, homeJ, 0);
	graph[source].distance = 0;
	dijkstraQ.decreaseKey(&graph[source]);

	while (!dijkstraQ.empty()) {

		AIVertex *u_s = dijkstraQ.extractMin();
		int u = u_s->index;
		dijkstraVisited[u] = true;

		if (graph[u].distance == infinity())
			break;

		// for each neighbor v of u
		int previousVertices[4];
		getPreviousVertices(u, previousVertices);
		for (int z=0; z<4; z++){
			int v = previousVertices[z];
			if (v != -1) {
				if (!dijkstraVisited[v]) {
					int alt = graph[u].distance + edgeWeight(v, u);
					if (alt < graph[v].distance) {
						graph[v].distance = alt;
						pathToHome[v] = u;
						dijkstraQ.decreaseKey(&graph[v]);
					}
				}
			}
		}
	}

}


// Allow AI to place one arrow
void AI::play() {
	// Decide between two strategies: gathering mice or sending snakes on others
	snakeMode = maze->snakes.size() >= maze->mice.size()/10;

	// Make a graph that represents the maze
	updateGraph();

	// Compute distances to house from vertex in the graph
	computeDistances();

	vector<AnimalWithDistance> sortedAnimals;
	if (snakeMode) {
		sortedAnimals.reserve(maze->snakes.size());
		for (unsigned m=0; m<maze->snakes.size(); m++) {
			Animal *animal = &maze->snakes[m];
			int i, j;
			animal->getFutureCell(&i, &j);
			int vertexIndex = vertexIndexFromCoords(i, j, animal->direction);
			int dist = getDistance(vertexIndex);
			sortedAnimals.push_back(AnimalWithDistance(animal, dist));
		}
	} else {
		// Sort mice by their distance
		sortedAnimals.reserve(maze->mice.size());
		for (unsigned m=0; m<maze->mice.size(); m++) {
			Animal *animal = &maze->mice[m];
			int i, j;
			animal->getFutureCell(&i, &j);
			int vertexIndex = vertexIndexFromCoords(i, j, animal->direction);
			int dist = getDistance(vertexIndex);
			sortedAnimals.push_back(AnimalWithDistance(animal, dist));
			// debug: color mouse by distance
			/*
			{
				float rdist = dist / 50.0f;
				if (rdist > 1) rdist = 1;
				mouse->color.r = rdist;
				mouse->color.g = 1 - rdist;
				mouse->color.b = 0;
			}
			*/
		}
	}

	sort(sortedAnimals.begin(), sortedAnimals.end());

	for (unsigned m=0; m<sortedAnimals.size(); m++) {
		Animal *animal = sortedAnimals[m].getAnimal();

		// debug: color target mouse
		//mouse->color = Program::getInstance()->playerColors[player];

		int i, j, dir;
		animal->getFutureCell(&i, &j);
		int vertexIndex = vertexIndexFromCoords(i, j, animal->direction);
		while (true) {
			vertexCoords(vertexIndex, &i, &j, &dir);
			if (i == homeI && j == homeJ) {
				break;
			}
			int nextVertexIndex = pathToHome[vertexIndex];
			if (nextVertexIndex < 0) {
				break;
			}
			int w = edgeWeight(vertexIndex, nextVertexIndex);
			if (w == arrowCost()) {
				if (maze->addArrow(i, j, player, vertexDir(nextVertexIndex)) != 0) {
					return; // AI allowed to place/modify one arrow by round
				}
			}
			vertexIndex = nextVertexIndex;
		}

	}

	//cout << "did not find arrow to place " << player << endl;

}

