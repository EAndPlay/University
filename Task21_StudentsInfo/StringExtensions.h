#pragma once

// Returns input line
char* ReadLine();

// Returns index of "value" in "inputText"
int strFindIndex(const char*, const char*);

// Returns array of splitted strings
char** strSplit(const char*, const char*, int&);

//Compares string ignoring register case (ic)
char strcmpic(const char*, const char*);

//Compares string ignoring register case by order (ic)
char strcmpico(const char*, const char*);
