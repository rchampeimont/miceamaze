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


#include "Functions.h"

void Functions::message(string message) {
#ifdef _WIN32
	MessageBox(NULL, message.c_str(), Functions::getAppName().c_str(), MB_OK | MB_ICONINFORMATION);
#else
	cout << message << endl;
#endif
}
void Functions::message(const char *message) {
#ifdef _WIN32
	MessageBox(NULL, message, Functions::getAppName().c_str(), MB_OK | MB_ICONINFORMATION);
#else
	cout << message << endl;
#endif
}

void Functions::error(string message) {
#ifdef _WIN32
	MessageBox(NULL, message.c_str(), (Functions::getAppName() + " error").c_str(), MB_OK | MB_ICONERROR);
#else
	cerr << "Error: " << message << endl;
#endif
}
void Functions::error(const char *message) {
#ifdef _WIN32
	MessageBox(NULL, message, (Functions::getAppName() + " error").c_str(), MB_OK | MB_ICONERROR);
#else
	cerr << "Error: " << message << endl;
#endif
}

void Functions::fatalError(string message) {
#ifdef _WIN32
	MessageBox(NULL, message.c_str(), (Functions::getAppName() + " error").c_str(), MB_OK | MB_ICONERROR);
#else
	cerr << "Fatal error: " << message << endl;
#endif
	exit(1);
}
void Functions::fatalError(const char *message) {
#ifdef _WIN32
	MessageBox(NULL, message, (Functions::getAppName() + " error").c_str(), MB_OK | MB_ICONERROR);
#else
	cerr << "Fatal error: " << message << endl;
#endif
	exit(1);
}


vector<string> Functions::listFiles(string path) {
	vector<string> files, files2;
#ifdef _WIN32
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hFind;
	hFind = FindFirstFile((path + "\\*").c_str(), &lpFindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		files.push_back(lpFindFileData.cFileName);
		while (FindNextFile(hFind, &lpFindFileData) != 0) {
			files.push_back(lpFindFileData.cFileName);
		}
	}
#else
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(path.c_str())) == NULL) {
		Functions::error(string("could not list files in ") + path);
		cout << "Error(" << errno << ") opening " << path << endl;
		return files;
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(string(dirp->d_name));
	}
	closedir(dp);
#endif

	for (unsigned k=0; k<files.size(); k++) {
		if (files[k].compare(".") != 0 && files[k].compare("..") != 0) {
			files2.push_back(files[k]);
		}
	}
	return files2;
}

string Functions::getLicense() {
	return "All rights reserved.\n"
			"\n"
			"Redistribution and use in source and binary forms, with or without modification,\n"
			"are permitted provided that the following conditions are met:\n"
			"\n"
			"* Redistributions of source code must retain the above copyright notice,\n"
			"  this list of conditions and the following disclaimer.\n"
			"* Redistributions in binary form must reproduce the above copyright notice,\n"
			"  this list of conditions and the following disclaimer in the documentation\n"
			"  and/or other materials provided with the distribution.\n"
			"\n"
			"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
			"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,\n"
			"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"
			"PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS\n"
			"BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n"
			"CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE\n"
			"GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n"
			"HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
			"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
			"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";
}
