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


#include "Animal.h"
#include "Program.h"

Animal::Animal(Maze *m, int x0, int y0, int dir) {
	maze = m;
	curI = y0;
	curJ = x0;
	x = x0 + 0.5;
	y = y0 + 0.5;
	direction = dir;
	angle = direction * 90;
	speed = 0.02;
	rotating = 0;
	color.r = 1;
	color.g = 1;
	color.b = 1;
	obeyed = false;
	dieAnim = 1;
	dying = false;
	inHouse = 0;
	life = 1;
	drill = false;
}

// Returns 0 if mouse is still walking,
// 1 if it has reached home,
// 2 if it is dead.
int Animal::walk(Game *game) {
	if (life <= 0) {
		dying = true;
	}
	if (dying) {
		// Animal is dying
		dieAnim -= 0.01;
		if (dieAnim <= 0) {
			// R.I.P
			return 2;
		} else {
			return 0;
		}
	} else if (inHouse > 0) {
		inHouse += 0.01;
		if (inHouse >= 1) {
			return 1;
		} else {
			return 0;
		}
	}
	if (rotating == 1) {
		// Mouse is rotating to the left.
		angle += speed*150; // 150 is empirical
		while (angle >= 360)
			angle -= 360;
		if (((int) (angle / 90)) == direction) {
			angle = direction * 90;
			rotating = 0;
		}
	} else if (rotating == -1) {
		// Mouse is rotating to the right.
		angle -= speed*150;
		while (angle < 0)
			angle += 360;
		if ((direction != 0 && angle <= direction*90) || (direction == 0 && angle > 300)) {
			angle = direction * 90;
			rotating = 0;
		}
	} else {
		// Mouse is walking.
		int dxu = direction == 0 ? 1 : (direction == 2 ? -1 : 0);
		int dyu = direction == 1 ? 1 : (direction == 3 ? -1 : 0);
		float dx = dxu * speed;
		float dy = dyu * speed;
		float noseX = x + dxu*0.5;
		float noseY = y + dyu*0.5;
		int i = maze->x2i(y);
		int j = maze->x2i(x);
		//int noseI = maze->x2i(noseY);
		//int noseJ = maze->x2i(noseX);
		int noseFutureI = maze->x2i(noseY+dy);
		int noseFutureJ = maze->x2i(noseX+dx);

		// Is there a house on current cell?
		for (vector<House>::iterator house = maze->houses.begin(); house != maze->houses.end(); ++house) {
			if (house->i == i && house->j == j) {
				if (reachedHouse(game, house->player)) {
					inHouse = 0.01;
				}
			}
		}

		if (isKilled()) {
			// dying
			dying = true;
			return 0;
		}

		// Will the mouse nose cross a new cell?
		if (noseFutureI != i || noseFutureJ != j) {
			// If the mouse has already obeyed an arrow on the current cell,
			// it won't obey it again.
			if (!obeyed) {

				// Is there an arrow on current cell?
				for (vector<Arrow>::iterator arrow = maze->arrows.begin(); arrow != maze->arrows.end(); ++arrow) {
					if (arrow->i == i && arrow->j == j) {
						// There is an arrow, but is the mouse still "enough"
						// on it? We don't want the arrow to apply if has just
						// been created and the mouse has almost left the cell.
						if (x - (int) x > 0.4
								&& x - (int) x < 0.6
								&& y - (int) y > 0.4
								&& y - (int) y < 0.6) {
							// mouse is on an arrow
							obeyed = true;

							// start rotating if the arrow points to a different direction than the mouse
							if (direction != arrow->direction) {
								// is it faster to go right to reach the arrow direction?
								if (arrow->direction + 1 == direction || (arrow->direction == 3 && direction == 0)) {
									rotating = -1;
									x = ((int) x) + 0.5;
									y = ((int) y) + 0.5;
								} else {
									rotating = 1;
									x = ((int) x) + 0.5;
									y = ((int) y) + 0.5;
								}
								direction = arrow->direction;
							}
						}
						break;
					}
				}
			}

			if (!rotating) {
				// Does the mouse face a wall?
				if (maze->crossesWall(j, i,  noseFutureJ, noseFutureI)) {
					bool takeWallIntoAccount = true;
					if (drill) {
						if (maze->destroyWall(j, i, noseFutureJ, noseFutureI))
						{
							takeWallIntoAccount = false;
							drill = false;
						}
					}

					if (takeWallIntoAccount)
					{
						// Smart mice: Can I go left or right without being stopped by a wall?
						IntXY left = maze->nextCell(j, i, direction == 3 ? 0 : direction + 1);
						bool leftOK = !maze->crossesWall(j, i, left.x, left.y);
						IntXY right = maze->nextCell(j, i, direction == 0 ? 3 : direction - 1);
						bool rightOK = !maze->crossesWall(j, i, right.x, right.y);

						if (leftOK && !rightOK) {
							rotating = 1;
						} else if (rightOK && !leftOK) {
							rotating = -1;
						} else if (rightOK && leftOK) {
							// Can go both left and right, so choose at random
							if (rand() % 2 == 0) {
								rotating = 1;
							} else {
								rotating = -1;
							}
						} else {
							// Mouse will have to U-turn
							rotating = 1;
						}

						// Rotate
						if (rotating == 1) {
							// rotate left
							direction++;
							if (direction == 4)
								direction = 0;
						} else {
							// rotate right
							direction--;
							if (direction == -1)
								direction = 3;
						}
						x = ((int) x) + 0.5;
						y = ((int) y) + 0.5;
					}
				}
			}
		}

		if (!rotating) {
			x += dx;
			y += dy;
			curI = maze->x2i(y);
			curJ = maze->x2i(x);
			if (curI != i || curJ != j) {
				obeyed = false;
			}
		}
	}

	return 0;
}

void Animal::getFutureCell(int *i, int *j) {
	if (rotating == 0) {
		int dxu = direction == 0 ? 1 : (direction == 2 ? -1 : 0);
		int dyu = direction == 1 ? 1 : (direction == 3 ? -1 : 0);
		float noseX = x + dxu*0.4;
		float noseY = y + dyu*0.4;
		*i = (int) noseY;
		*j = (int) noseX;
		//cout << "straight" << endl;
	} else {
		*i = (int) y;
		*j = (int) x;
		//cout << "rotating" << endl;
	}
	if (*i >= maze->height) *i = maze->height - 1;
	if (*j >= maze->width) *j = maze->width - 1;
	if (*i < 0) *i = 0;
	if (*j < 0) *j = 0;
}

bool Animal::reachedHouse(Game *game, int player) {
	return true;
}

