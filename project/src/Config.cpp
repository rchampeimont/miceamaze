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

Config::Config() {
#ifdef _WIN32
	fpsBehaviour = 0;
#else
	fpsBehaviour  = 1;
#endif
	showFPS = false;
	path = "";
}

void Config::load() {
#ifndef EMSCRIPTEN
#ifdef _WIN32
	char appDataPath[MAX_PATH];
	Functions::verify(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appDataPath) == S_OK);
	path = string(appDataPath) + "\\MiceAmaze";
	if (GetFileAttributes(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
	  if (CreateDirectory(path.c_str(), NULL) == 0) {
		  Functions::error("Count not create " + path);
	  }
	}

#else
	const char *homeEnv = getenv("HOME");
	string homeDir = homeEnv == NULL ? "" : homeEnv;
	if (homeDir.empty()) {
		struct passwd *entry = getpwuid(getuid());
		homeDir = entry->pw_dir;
	}
	if (homeDir.empty()) {
		Functions::error("Could not find home directory");
	} else {
		path = homeDir + "/.miceamaze";
		struct stat st;
		if (stat(path.c_str(), &st) != 0) {
			if (mkdir(path.c_str(), 0755) != 0) {
				Functions::error("Count not create " + path);
			}
		}
	}
#endif

	if (!path.empty()) {
		ifstream f;
		f.open(configFilePath().c_str(), ios::in | ios::binary);
		if (f.is_open() && !f.eof()) {
			bool inValue = false;
			char c;
			string option = "";
			string value = "";
			bool loop = true;
			while (loop) {
				bool endline = false;
				f.read(&c, 1);
				if (f.eof()) {
					endline = true;
					loop = false;
				} else {
					endline = (c == '\n');
				}
				if (endline) {
					// parse line
					if (option.length() > 0 && value.length() > 0) {
						if (strcasecmp(option.c_str(), "fpsbehaviour") == 0) {
							fpsBehaviour = atoi(value.c_str());
						} else if (strcasecmp(option.c_str(), "fullscreen") == 0) {
							Program::getInstance()->fullscreen = atoi(value.c_str());
						} else if (strcasecmp(option.c_str(), "showfps") == 0) {
							showFPS = atoi(value.c_str()) != 0;
						}
					}
					// prepare for next line
					option = "";
					value = "";
					inValue = false;
				} else if (c == '=') {
					inValue = true;
				} else if (c != '\r') {
					if (inValue) {
						value += c;
					} else {
						option += c;
					}
				}
			}

			f.close();
		}
	}
#else
	// Emscripten mode
	fpsBehaviour = 1;
	Program::getInstance()->fullscreen = true;
	showFPS = true;
#endif

	// Command-line options overwrite config file options
	char **argv = Program::getInstance()->argv;
	int argc = Program::getInstance()->argc;
	for (int i=0; i<argc; i++) {
		if (strcasecmp(argv[i], "-w") == 0 || strcasecmp(argv[i], "-window") == 0 || strcasecmp(argv[i], "--window") == 0) {
			Program::getInstance()->fullscreen = false;
		}
		if (strcasecmp(argv[i], "-f") == 0 || strcasecmp(argv[i], "-fullscreen") == 0 || strcasecmp(argv[i], "--fullscreen") == 0) {
			Program::getInstance()->fullscreen = true;
		}
		if (strcasecmp(argv[i], "-powersave") == 0 || strcasecmp(argv[i], "--powersave") == 0) {
			fpsBehaviour = -1;
		}
		if (strcasecmp(argv[i], "-maxfps") == 0 || strcasecmp(argv[i], "--maxfps") == 0) {
			fpsBehaviour = 1;
		}
		if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0 || strcasecmp(argv[i], "--help") == 0 || strcasecmp(argv[i], "/?") == 0) {
			cout << Functions::getAppName() << " " << Functions::getVersion() << endl;
			cout << "Command-line options:" << endl;
			cout << "--window (or -w): start in window mode (not fullscreen)" << endl;
			cout << "--fullscreen (or -f): start in fullscreen mode" << endl;
			cout << "--powersave: limit number of FPS to save energy" << endl;
			cout << "--maxfps: run in max FPS even if on battery" << endl;
			exit(0);
		}
	}

}

void Config::save() {
#ifndef EMSCRIPTEN
	if (path.empty()) {
		Functions::error("could not save config");
		return;
	}
	ofstream f;
	f.open(configFilePath().c_str(), ios::trunc | ios::out | ios::binary);
	if (!f.is_open()) {
		Functions::error("could not save config");
		return;
	}

	f << "fpsbehaviour=" << fpsBehaviour << "\r\n";
	f << "fullscreen=" << (Program::getInstance()->fullscreen?1:0) << "\r\n";
	f << "showfps=" << (showFPS ? 1 : 0) << "\r\n";

	f.close();
#endif
}
