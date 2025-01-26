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


#include "Sound.h"

Sound::Sound() {
	musicPlaying = -1;
}

Sound::~Sound() {
}

void Sound::init() {
	music.push_back(loadMusic("DST-Azum.ogg"));
	music.push_back(loadMusic("DST-DistantMessage.ogg"));
	music.push_back(loadMusic("Dj_Rkod_Pulse_(George_Ellinas_Remix).ogg"));
}

Mix_Chunk *Sound::loadSample(string filename) {
	Mix_Chunk *sample = Mix_LoadWAV((Program::getInstance()->dataPath + "/sound/" + filename).c_str());
	if (!sample) {
		Functions::error(Mix_GetError());
	}
	return sample;
}

Mix_Music *Sound::loadMusic(string filename) {
	Mix_Music *sample = Mix_LoadMUS((Program::getInstance()->dataPath + "/music/" + filename).c_str());
	if (!sample) {
		Functions::error(Mix_GetError());
	}
	return sample;
}

void Sound::playMusic(int musicIndex) {
	if (Program::getInstance()->config.music) {
		if (musicPlaying != musicIndex) {
			Mix_VolumeMusic(64);
			if (Mix_FadeInMusic(music[musicIndex], -1, 1000) != 0) {
				Functions::error(Mix_GetError());
			}
			musicPlaying = musicIndex;
		}
	}
}

void Sound::playGameMusic() {
	playMusic(1 + rand() % 2);
}

void Sound::playMenuMusic() {
	playMusic(0);
}

void Sound::stopMusic() {
	musicPlaying = -1;
	Mix_HaltMusic();
}
